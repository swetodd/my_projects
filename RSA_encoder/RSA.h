#ifndef RSA_H
#define RSA_H

#include "BigInteger.h"
#include <vector>
#include <string>
#include <utility>

class RSA {
private:
    BigInteger n;           // Модуль n = p * q
    BigInteger e;           // Открытая экспонента
    BigInteger d;           // Закрытая экспонента
    BigInteger p;           // Простое число p
    BigInteger q;           // Простое число q
    BigInteger phi;         // Функция Эйлера φ(n) = (p-1)*(q-1)
    bool keysGenerated;     // Флаг наличия ключей
    
    // Вспомогательные методы
    bool areCoprime(const BigInteger& a, const BigInteger& b);
    BigInteger findPublicExponent(const BigInteger& phi_n);

public:
    // Конструктор
    RSA();
    
    // Генерация ключей
    void generateKeys(const BigInteger& p, const BigInteger& q);
    void generateRandomKeys(int bits = 32);
    
    // Шифрование/дешифрование
    BigInteger encrypt(const BigInteger& message);
    BigInteger decrypt(const BigInteger& cipher);
    
    // Шифрование/дешифрование текста
    std::vector<BigInteger> encryptText(const std::string& text);
    std::string decryptText(const std::vector<BigInteger>& cipher);
    
    // Работа с ключами
    std::pair<BigInteger, BigInteger> getPublicKey() const;
    std::pair<BigInteger, BigInteger> getPrivateKey() const;
    void displayKeys() const;
    
    // Сохранение/загрузка ключей
    bool saveKeysToFile(const std::string& filename) const;
    bool loadKeysFromFile(const std::string& filename);
    
    // Проверка
    bool hasKeys() const { return keysGenerated; }
    bool validateKeys() const;
};

#endif