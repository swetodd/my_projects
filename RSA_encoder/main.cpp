#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "BigInteger.h"
#include "RSA.h"

using namespace std;

void clearInputStream() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayMenu() {
    cout << "\n========== RSA КРИПТОСИСТЕМА ==========\n";
    cout << "1. Сгенерировать ключи\n";
    cout << "2. Показать текущие ключи\n";
    cout << "3. Зашифровать текст (ввод с клавиатуры)\n";
    cout << "4. Расшифровать текст (ввод с клавиатуры)\n";
    cout << "5. Зашифровать число\n";
    cout << "6. Расшифровать число\n";
    cout << "7. Сохранить ключи в файл\n";
    cout << "8. Загрузить ключи из файла\n";
    cout << "9. Зашифровать текст ИЗ ФАЙЛА\n";
    cout << "10. Расшифровать текст ИЗ ФАЙЛА\n";
    cout << "11. Зашифровать текст и сохранить В ФАЙЛ\n";
    cout << "12. Расшифровать текст и сохранить В ФАЙЛ\n";
    cout << "13. Тестирование алгоритма\n";
    cout << "0. Выход\n";
    cout << "Выберите опцию: ";
}

// Функция для чтения текста из файла
string readTextFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Функция для записи текста в файл
void writeTextToFile(const string& filename, const string& text) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось создать файл: " + filename);
    }
    file << text;
}

// Функция для записи зашифрованных чисел в файл
void writeEncryptedToFile(const string& filename, const vector<BigInteger>& encrypted) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось создать файл: " + filename);
    }
    
    for (const auto& num : encrypted) {
        file << num.toString() << "\n";
    }
}

// Функция для чтения зашифрованных чисел из файла
vector<BigInteger> readEncryptedFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    
    vector<BigInteger> encrypted;
    string line;
    
    while (getline(file, line)) {
        if (!line.empty()) {
            encrypted.push_back(BigInteger(line));
        }
    }
    
    return encrypted;
}

// Функция для отображения содержимого файла
void displayFileContent(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Не удалось открыть файл: " << filename << endl;
        return;
    }
    
    cout << "\n--- Содержимое файла " << filename << " ---\n";
    string line;
    int lineNum = 1;
    while (getline(file, line)) {
        cout << "Строка " << lineNum++ << ": " << line << endl;
    }
    cout << "--------------------------------\n";
}

void testMode() {
    cout << "\n--- Тестирование RSA ---\n";
    
    RSA rsa;
    
    cout << "Генерация тестовых ключей (p=61, q=53)...\n";
    rsa.generateKeys(BigInteger(61), BigInteger(53));
    rsa.displayKeys();
    
    string testMessage = "Hello RSA!";
    cout << "\nИсходное сообщение: " << testMessage << endl;
    
    vector<BigInteger> encrypted = rsa.encryptText(testMessage);
    cout << "Зашифрованное сообщение (числа): ";
    for (const auto& num : encrypted) {
        cout << num.toString() << " ";
    }
    cout << endl;
    
    string decrypted = rsa.decryptText(encrypted);
    cout << "Расшифрованное сообщение: " << decrypted << endl;
    
    if (testMessage == decrypted) {
        cout << "\n✓ Тест пройден успешно!\n";
    } else {
        cout << "\n✗ Ошибка в шифровании/расшифровании!\n";
    }
}

int main() {
    RSA rsa;
    int choice;
    string input;
    
    cout << "Добро пожаловать в программу RSA шифрования с поддержкой файлов!\n";
    
    do {
        displayMenu();
        cin >> choice;
        clearInputStream();
        
        try {
            switch(choice) {
                case 1: {
                    cout << "Генерация новых ключей...\n";
                    cout << "Введите размер ключа в битах (рекомендуется 32-64 для демо): ";
                    int bits;
                    cin >> bits;
                    clearInputStream();
                    
                    if (bits < 16) {
                        cout << "Слишком маленький размер ключа. Использую 32 бита.\n";
                        bits = 32;
                    }
                    
                    rsa.generateRandomKeys(bits);
                    rsa.displayKeys();
                    break;
                }
                
                case 2: {
                    rsa.displayKeys();
                    break;
                }
                
                case 3: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите текст для шифрования: ";
                    getline(cin, input);
                    
                    vector<BigInteger> encrypted = rsa.encryptText(input);
                    cout << "Зашифрованный текст (числа через пробел):\n";
                    for (const auto& num : encrypted) {
                        cout << num.toString() << " ";
                    }
                    cout << endl;
                    break;
                }
                
                case 4: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите зашифрованные числа (через пробел, окончание - пустая строка):\n";
                    vector<BigInteger> encrypted;
                    string line;
                    getline(cin, line);
                    
                    if (line.empty()) {
                        cout << "Нет данных для расшифровки.\n";
                        break;
                    }
                    
                    stringstream ss(line);
                    string token;
                    while (ss >> token) {
                        encrypted.push_back(BigInteger(token));
                    }
                    
                    string decrypted = rsa.decryptText(encrypted);
                    cout << "Расшифрованный текст: " << decrypted << endl;
                    break;
                }
                
                case 5: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите число для шифрования: ";
                    getline(cin, input);
                    
                    BigInteger num(input);
                    BigInteger encrypted = rsa.encrypt(num);
                    cout << "Зашифрованное число: " << encrypted.toString() << endl;
                    break;
                }
                
                case 6: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите число для расшифрования: ";
                    getline(cin, input);
                    
                    BigInteger num(input);
                    BigInteger decrypted = rsa.decrypt(num);
                    cout << "Расшифрованное число: " << decrypted.toString() << endl;
                    break;
                }
                
                case 7: {
                    cout << "Введите имя файла для сохранения ключей: ";
                    getline(cin, input);
                    
                    if (rsa.saveKeysToFile(input)) {
                        cout << "Ключи успешно сохранены в файл: " << input << endl;
                        displayFileContent(input);
                    } else {
                        cout << "Ошибка при сохранении ключей!\n";
                    }
                    break;
                }
                
                case 8: {
                    cout << "Введите имя файла для загрузки ключей: ";
                    getline(cin, input);
                    
                    if (rsa.loadKeysFromFile(input)) {
                        cout << "Ключи успешно загружены из файла: " << input << endl;
                        rsa.displayKeys();
                    } else {
                        cout << "Ошибка при загрузке ключей!\n";
                    }
                    break;
                }
                
                case 9: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите имя файла с текстом для шифрования: ";
                    getline(cin, input);
                    
                    string fileContent = readTextFromFile(input);
                    cout << "Прочитано из файла: " << fileContent.length() << " символов\n";
                    cout << "Первые 100 символов: " << fileContent.substr(0, 100) << endl;
                    
                    vector<BigInteger> encrypted = rsa.encryptText(fileContent);
                    
                    cout << "Зашифрованные числа (первые 10): ";
                    for (int i = 0; i < min(10, (int)encrypted.size()); i++) {
                        cout << encrypted[i].toString() << " ";
                    }
                    cout << (encrypted.size() > 10 ? "..." : "") << endl;
                    cout << "Всего чисел: " << encrypted.size() << endl;
                    break;
                }
                
                case 10: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите имя файла с зашифрованными числами: ";
                    getline(cin, input);
                    
                    vector<BigInteger> encrypted = readEncryptedFromFile(input);
                    cout << "Прочитано чисел из файла: " << encrypted.size() << endl;
                    
                    string decrypted = rsa.decryptText(encrypted);
                    cout << "Расшифрованный текст (первые 200 символов):\n";
                    cout << decrypted.substr(0, 200) << endl;
                    if (decrypted.length() > 200) {
                        cout << "... (всего " << decrypted.length() << " символов)\n";
                    }
                    break;
                }
                
                case 11: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите имя файла с текстом для шифрования: ";
                    getline(cin, input);
                    string inputFile = input;
                    
                    cout << "Введите имя файла для сохранения зашифрованных чисел: ";
                    getline(cin, input);
                    string outputFile = input;
                    
                    string fileContent = readTextFromFile(inputFile);
                    cout << "Прочитано из файла: " << fileContent.length() << " символов\n";
                    
                    vector<BigInteger> encrypted = rsa.encryptText(fileContent);
                    
                    writeEncryptedToFile(outputFile, encrypted);
                    cout << "Зашифрованные числа сохранены в файл: " << outputFile << endl;
                    cout << "Количество чисел: " << encrypted.size() << endl;
                    
                    displayFileContent(outputFile);
                    break;
                }
                
                case 12: {
                    if (!rsa.hasKeys()) {
                        cout << "Сначала сгенерируйте ключи (опция 1)!\n";
                        break;
                    }
                    
                    cout << "Введите имя файла с зашифрованными числами: ";
                    getline(cin, input);
                    string inputFile = input;
                    
                    cout << "Введите имя файла для сохранения расшифрованного текста: ";
                    getline(cin, input);
                    string outputFile = input;
                    
                    vector<BigInteger> encrypted = readEncryptedFromFile(inputFile);
                    cout << "Прочитано чисел из файла: " << encrypted.size() << endl;
                    
                    string decrypted = rsa.decryptText(encrypted);
                    
                    writeTextToFile(outputFile, decrypted);
                    cout << "Расшифрованный текст сохранен в файл: " << outputFile << endl;
                    cout << "Количество символов: " << decrypted.length() << endl;
                    
                    displayFileContent(outputFile);
                    break;
                }
                
                case 13: {
                    testMode();
                    break;
                }
                
                case 0: {
                    cout << "Выход из программы. До свидания!\n";
                    break;
                }
                
                default:
                    cout << "Неверный выбор. Попробуйте снова.\n";
            }
        } catch (const exception& e) {
            cout << "ОШИБКА: " << e.what() << endl;
        }
        
        if (choice != 0) {
            cout << "\nНажмите Enter для продолжения...";
            cin.get();
        }
        
    } while (choice != 0);
    
    return 0;
}