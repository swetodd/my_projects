#include "RSA.h"
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Конструктор
RSA::RSA() : keysGenerated(false) {}

// Проверка на взаимную простоту
bool RSA::areCoprime(const BigInteger& a, const BigInteger& b) {
    return a.gcd(b) == BigInteger(1);
}

// Поиск открытой экспоненты
BigInteger RSA::findPublicExponent(const BigInteger& phi_n) {
    // Обычно используют 65537 как стандартное значение
    BigInteger e(65537);
    
    // Если 65537 меньше phi и взаимно просто с ней, используем его
    if (e < phi_n && areCoprime(e, phi_n)) {
        return e;
    }
    
    // Иначе ищем следующее подходящее
    e = BigInteger(3);
    while (e < phi_n) {
        if (areCoprime(e, phi_n)) {
            return e;
        }
        e = e + BigInteger(2); // Только нечетные
    }
    
    throw runtime_error("Cannot find suitable public exponent");
}

// Генерация ключей из заданных простых чисел
void RSA::generateKeys(const BigInteger& p, const BigInteger& q) {
    // Проверяем, что p и q простые
    if (!p.isPrime() || !q.isPrime()) {
        throw runtime_error("Both numbers must be prime");
    }
    
    this->p = p;
    this->q = q;
    
    // Вычисляем модуль n = p * q
    n = p * q;
    
    // Вычисляем функцию Эйлера φ(n) = (p-1)*(q-1)
    phi = (p - BigInteger(1)) * (q - BigInteger(1));
    
    // Находим открытую экспоненту e
    e = findPublicExponent(phi);
    
    // Вычисляем закрытую экспоненту d (обратная к e по модулю φ)
    d = e.modInverse(phi);
    
    keysGenerated = true;
}

// Генерация случайных ключей
void RSA::generateRandomKeys(int bits) {
    // Генерируем два случайных простых числа
    BigInteger p = BigInteger::generateRandomPrime(bits / 2);
    BigInteger q = BigInteger::generateRandomPrime(bits / 2);
    
    // Убеждаемся, что p и q различны
    while (p == q) {
        q = BigInteger::generateRandomPrime(bits / 2);
    }
    
    generateKeys(p, q);
}

// Шифрование одного числа
BigInteger RSA::encrypt(const BigInteger& message) {
    if (!keysGenerated) {
        throw runtime_error("Keys not generated");
    }
    
    if (message >= n) {
        throw runtime_error("Message must be less than n");
    }
    
    return message.modPow(e, n);
}

// Дешифрование одного числа
BigInteger RSA::decrypt(const BigInteger& cipher) {
    if (!keysGenerated) {
        throw runtime_error("Keys not generated");
    }
    
    return cipher.modPow(d, n);
}

// Шифрование текста
vector<BigInteger> RSA::encryptText(const string& text) {
    vector<BigInteger> encrypted;
    
    for (char c : text) {
        // Преобразуем символ в число (ASCII код)
        BigInteger message(static_cast<long long>(c));
        encrypted.push_back(encrypt(message));
    }
    
    return encrypted;
}

// Дешифрование текста
string RSA::decryptText(const vector<BigInteger>& cipher) {
    string decrypted;
    
    for (const auto& num : cipher) {
        BigInteger message = decrypt(num);
        
        // Преобразуем число обратно в символ
        long long charCode = 0;
        string numStr = message.toString();
        
        // Простое преобразование строки в число (для демо)
        // В реальном проекте нужна более надежная конвертация
        try {
            charCode = stoll(numStr);
        } catch (...) {//ловит исключения
            // Если не удалось преобразовать, пробуем интерпретировать как ASCII
            if (numStr.length() == 1) {
                charCode = numStr[0];
            } else {
                charCode = 0;
            }
        }
        
        if (charCode >= 0 && charCode <= 255) {
            decrypted += static_cast<char>(charCode);
        } else {
            decrypted += '?';
        }
    }
    
    return decrypted;
}

// Получение ключей
pair<BigInteger, BigInteger> RSA::getPublicKey() const {
    return make_pair(e, n);
}

pair<BigInteger, BigInteger> RSA::getPrivateKey() const {
    return make_pair(d, n);
}

// Отображение ключей
void RSA::displayKeys() const {
    if (!keysGenerated) {
        cout << "Ключи не сгенерированы.\n";
        return;
    }
    
    cout << "\n--- Ключи RSA ---\n";
    cout << "Простое число p: " << p.toString() << endl;
    cout << "Простое число q: " << q.toString() << endl;
    cout << "Модуль n = p * q: " << n.toString() << " (длина: " << n.bitLength() << " бит)\n";
    cout << "Функция Эйлера φ(n): " << phi.toString() << endl;
    cout << "Открытая экспонента e: " << e.toString() << endl;
    cout << "Закрытая экспонента d: " << d.toString() << endl;
    cout << "Открытый ключ (e, n): (" << e.toString() << ", " << n.toString() << ")\n";
    cout << "Закрытый ключ (d, n): (" << d.toString() << ", " << n.toString() << ")\n";
}

// Сохранение ключей в файл
bool RSA::saveKeysToFile(const string& filename) const {
    if (!keysGenerated) {
        return false;
    }
    
    ofstream file(filename);
    if (!file) {
        return false;
    }
    
    file << p.toString() << endl;
    file << q.toString() << endl;
    file << n.toString() << endl;
    file << phi.toString() << endl;
    file << e.toString() << endl;
    file << d.toString() << endl;
    
    file.close();
    return true;
}

// Загрузка ключей из файла
bool RSA::loadKeysFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        return false;
    }
    
    string pStr, qStr, nStr, phiStr, eStr, dStr;
    
    getline(file, pStr);
    getline(file, qStr);
    getline(file, nStr);
    getline(file, phiStr);
    getline(file, eStr);
    getline(file, dStr);
    
    try {
        p = BigInteger(pStr);
        q = BigInteger(qStr);
        n = BigInteger(nStr);
        phi = BigInteger(phiStr);
        e = BigInteger(eStr);
        d = BigInteger(dStr);
        keysGenerated = true;
    } catch (...) {
        return false;
    }
    
    file.close();
    return true;
}

// Проверка корректности ключей
bool RSA::validateKeys() const {
    if (!keysGenerated) return false;
    
    // Проверяем, что e * d ≡ 1 mod φ
    BigInteger ed = e * d;
    return (ed % phi) == BigInteger(1);
}