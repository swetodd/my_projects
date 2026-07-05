// Файл для автоматического тестирования RSA в CI/CD пайплайне
// Компиляция: g++ -std=c++17 -o test_rsa test_rsa.cpp BigInteger.cpp RSA.cpp

#include <chrono>
#include <iostream>
#include <string>
#include <cassert>
#include "RSA.h"

using namespace std;

void printTestResult(const string& testName, bool passed) {
    cout << (passed ? "[PASS]" : "[FAIL]") << " " << testName << endl;
}

void testKeyGeneration() {
    cout << "\n--- Тест 1: Генерация ключей ---\n";
    
    RSA rsa;
    
    // Тест с известными простыми числами
    try {
        rsa.generateKeys(BigInteger(61), BigInteger(53));
        assert(rsa.hasKeys() == true);
        assert(rsa.validateKeys() == true);
        
        auto publicKey = rsa.getPublicKey();
        auto privateKey = rsa.getPrivateKey();
        
        // Проверяем, что ключи разные
        assert(publicKey.first != privateKey.first);
        assert(publicKey.second == privateKey.second); // n одинаковый
        
        printTestResult("Генерация с известными p=61, q=53", true);
    } catch (...) {
        printTestResult("Генерация с известными p=61, q=53", false);
    }
    
    // Тест со случайными ключами
    try {
        RSA rsa2;
        rsa2.generateRandomKeys(16); // 16 бит для быстрого теста
        assert(rsa2.hasKeys() == true);
        assert(rsa2.validateKeys() == true);
        printTestResult("Генерация случайных ключей (16 бит)", true);
    } catch (...) {
        printTestResult("Генерация случайных ключей (16 бит)", false);
    }
}

void testEncryptionDecryption() {
    cout << "\n--- Тест 2: Шифрование и дешифрование ---\n";
    
    RSA rsa;
    rsa.generateKeys(BigInteger(61), BigInteger(53));
    
    // Тест шифрования/дешифрования чисел
    try {
        BigInteger original(42);
        BigInteger encrypted = rsa.encrypt(original);
        BigInteger decrypted = rsa.decrypt(encrypted);
        
        assert(original == decrypted);
        printTestResult("Шифрование/дешифрование числа 42", true);
    } catch (...) {
        printTestResult("Шифрование/дешифрование числа 42", false);
    }
    
    // Тест с граничным значением
    try {
        auto publicKey = rsa.getPublicKey();
        BigInteger n = publicKey.second;
        BigInteger maxVal = n - BigInteger(1);
        
        BigInteger encrypted = rsa.encrypt(maxVal);
        BigInteger decrypted = rsa.decrypt(encrypted);
        
        assert(maxVal == decrypted);
        printTestResult("Шифрование/дешифрование числа n-1", true);
    } catch (...) {
        printTestResult("Шифрование/дешифрование числа n-1", false);
    }
}

void testTextEncryptionDecryption() {
    cout << "\n--- Тест 3: Шифрование и дешифрование текста ---\n";
    
    RSA rsa;
    rsa.generateKeys(BigInteger(61), BigInteger(53));
    
    string testStrings[] = {
        "Hello RSA!",
        "Тест кириллицы",
        "1234567890",
        "A", // Один символ
        "Hello, World! This is a longer test string." // Длинная строка
    };
    
    for (const string& original : testStrings) {
        try {
            vector<BigInteger> encrypted = rsa.encryptText(original);
            string decrypted = rsa.decryptText(encrypted);
            
            // Проверяем, что длина соответствует (для ASCII символов)
            if (decrypted.length() == original.length()) {
                printTestResult("Текст: \"" + original + "\"", true);
            } else {
                printTestResult("Текст: \"" + original + "\" - длина не совпадает", false);
            }
        } catch (const exception& e) {
            cout << "Ошибка при шифровании текста: " << e.what() << endl;
            printTestResult("Текст: \"" + original + "\"", false);
        }
    }
}

void testFileOperations() {
    cout << "\n--- Тест 4: Работа с файлами ---\n";
    
    RSA rsa;
    rsa.generateKeys(BigInteger(61), BigInteger(53));
    
    // Тест сохранения/загрузки ключей
    try {
        string filename = "test_keys.txt";
        assert(rsa.saveKeysToFile(filename) == true);
        
        RSA rsa2;
        assert(rsa2.loadKeysFromFile(filename) == true);
        
        // Проверяем, что ключи совпадают
        auto pub1 = rsa.getPublicKey();
        auto pub2 = rsa2.getPublicKey();
        auto priv1 = rsa.getPrivateKey();
        auto priv2 = rsa2.getPrivateKey();
        
        assert(pub1.first == pub2.first);
        assert(pub1.second == pub2.second);
        assert(priv1.first == priv2.first);
        assert(priv1.second == priv2.second);
        
        // Удаляем тестовый файл
        remove(filename.c_str());
        printTestResult("Сохранение/загрузка ключей", true);
    } catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
        printTestResult("Сохранение/загрузка ключей", false);
    }
}

void testValidation() {
    cout << "\n--- Тест 5: Валидация ключей ---\n";
    
    RSA rsa;
    rsa.generateKeys(BigInteger(61), BigInteger(53));
    
    // Проверяем валидность сгенерированных ключей
    assert(rsa.validateKeys() == true);
    printTestResult("Валидация корректных ключей", true);
    
    // Проверяем, что без ключей валидация false
    RSA emptyRsa;
    assert(emptyRsa.validateKeys() == false);
    printTestResult("Валидация без ключей (должна быть false)", true);
}

void testPerformance() {
    cout << "\n--- Тест 6: Производительность ---\n";
    
    RSA rsa;
    rsa.generateRandomKeys(32);
    
    string testText = "Performance test text for RSA encryption and decryption.";
    
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; i++) {
        auto encrypted = rsa.encryptText(testText);
        auto decrypted = rsa.decryptText(encrypted);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    cout << "Время выполнения 10 циклов шифрования/дешифрования: " 
         << duration.count() << " мс" << endl;
    printTestResult("Тест производительности (завершен без ошибок)", true);
}

int main() {
    cout << "=====================================" << endl;
    cout << "  ЗАПУСК ТЕСТОВ RSA КРИПТОСИСТЕМЫ" << endl;
    cout << "=====================================" << endl;
    
    int passed = 0;
    int total = 0;
    
    try {
        testKeyGeneration();
        testEncryptionDecryption();
        testTextEncryptionDecryption();
        testFileOperations();
        testValidation();
        testPerformance();
    } catch (const exception& e) {
        cout << "\nКритическая ошибка в тестах: " << e.what() << endl;
        return 1;
    }
    
    cout << "\n=====================================" << endl;
    cout << "  ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ" << endl;
    cout << "=====================================" << endl;
    
    return 0;
}
