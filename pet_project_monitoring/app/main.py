
import random
import logging
import json
from datetime import datetime
from flask import Flask, jsonify, request
from flask_restful import Api, Resource
import time
import socket

# ===== ОТПРАВКА В LOGSTASH =====
class LogstashHandler(logging.Handler):
    def emit(self, record):
        try:
            log_entry = {
                "timestamp": datetime.utcnow().isoformat(),
                "level": record.levelname,
                "message": record.getMessage(),
                "module": record.module,
                "function": record.funcName,
                "line": record.lineno
            }
            if hasattr(record, 'user_id'):
                log_entry["user_id"] = record.user_id
            if hasattr(record, 'endpoint'):
                log_entry["endpoint"] = record.endpoint
                
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(2)
            sock.connect(('logstash', 5001))
            sock.send((json.dumps(log_entry) + '\n').encode())
            sock.close()
            print(f"Log sent to Logstash: {log_entry['level']} - {log_entry['message']}")
        except Exception as e:
            print(f"Logstash send error: {e}")

# ===== ПРИЛОЖЕНИЕ =====
app = Flask(__name__)
api = Api(app)

# ===== ОТКЛЮЧАЕМ ЛОГИ FLASK (WERKZEUG) =====
# Добавляем ЭТОТ КОД СРАЗУ ПОСЛЕ СОЗДАНИЯ app
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

# Настройка логгера
handler = logging.StreamHandler()
handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
app.logger.addHandler(handler)

# Добавляем отправку в Logstash
logstash_handler = LogstashHandler()
logstash_handler.setLevel(logging.INFO)
app.logger.addHandler(logstash_handler)
app.logger.setLevel(logging.INFO)

# ===== СЧЕТЧИКИ =====
request_counter = {"total": 0, "weather": 0, "error": 0, "errors_500": 0}

# ===== ЭНДПОИНТЫ =====
class WeatherAPI(Resource):
    def get(self):
        request_counter["total"] += 1
        request_counter["weather"] += 1
        temperature = round(random.uniform(-30, 40), 1)
        time.sleep(random.uniform(0.1, 0.5))
        response = {
            "city": "Moscow",
            "temperature": temperature,
            "feels_like": round(temperature + random.uniform(-5, 5), 1),
            "timestamp": datetime.utcnow().isoformat(),
            "endpoint": "/api/weather"
        }
        app.logger.info(
            f"Weather requested: {temperature}°C",
            extra={"endpoint": "/api/weather", "temperature": temperature}
        )
        return jsonify(response)

class ErrorAPI(Resource):
    def get(self):
        request_counter["total"] += 1
        request_counter["error"] += 1
        chance = random.randint(1, 100)
        user_id = request.args.get('user_id', 'unknown')
        if chance <= 20:
            request_counter["errors_500"] += 1
            error_types = [
                "Database connection timeout",
                "Invalid user session",
                "External service unavailable",
                "Data validation failed",
                "Rate limit exceeded"
            ]
            error_msg = random.choice(error_types)
            app.logger.error(
                f"Error occurred: {error_msg}",
                extra={"endpoint": "/api/error", "user_id": user_id, "error_type": error_msg, "status_code": 500}
            )
            return {"status": "error", "message": error_msg, "timestamp": datetime.utcnow().isoformat()}, 500
        else:
            response = {
                "status": "success",
                "message": "Operation completed successfully",
                "data": {
                    "processed_at": datetime.utcnow().isoformat(),
                    "user_id": user_id,
                    "random_value": random.randint(1, 1000)
                }
            }
            app.logger.info(
                f"Success response for user {user_id}",
                extra={"endpoint": "/api/error", "user_id": user_id, "status_code": 200}
            )
            return jsonify(response)

class HealthCheck(Resource):
    def get(self):
        return {"status": "healthy", "timestamp": datetime.utcnow().isoformat()}

class MetricsAPI(Resource):
    def get(self):
        return jsonify({"request_counter": request_counter, "timestamp": datetime.utcnow().isoformat()})

api.add_resource(WeatherAPI, '/api/weather')
api.add_resource(ErrorAPI, '/api/error')
api.add_resource(HealthCheck, '/api/health')
api.add_resource(MetricsAPI, '/api/metrics')

@app.errorhandler(Exception)
def handle_exception(e):
    app.logger.error(
        f"Unhandled exception: {str(e)}",
        extra={"endpoint": request.path, "method": request.method, "error": str(e)}
    )
    return {"status": "error", "message": "Internal server error", "timestamp": datetime.utcnow().isoformat()}, 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
