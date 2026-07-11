FROM python:3.10-slim

WORKDIR /app
COPY app/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY app/ .

RUN useradd -m -u 1000 appuser && chown -R appuser:appuser /app
USER appuser


EXPOSE 5000

CMD ["python", "main.py"]
