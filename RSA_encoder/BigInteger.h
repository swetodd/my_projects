#ifndef BIGINTEGER_H //чтобы если определен в другом файле, то не переопределился, т.к будет ошибка иначе
#define BIGINTEGER_H

#include <iostream> //для >> <<<
#include <string> //чтение чисел из файлов, работа со строками
#include <vector> //динам массив
#include <algorithm>// Нужен для std::min, std::max, std::reverse и др.
#include <stdexcept> //std::runtime_error — для ошибок времени выполнения
#include <random> //BigInteger::generateRandomPrime() — генерация случайных простых чисел для ключей RSA

class BigInteger {
private:
    std::vector<int> digits;// Храним цифры числа в обратном порядке. <int> это шаблон, внутри класса тип переменной будет int
    bool isNegative;
    
    // Вспомогательные методы
    void removeLeadingZeros();
    static std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b);// std тут в роли пространства имен(как и везде), нужна для того, чтобы конкретно определить из какой библиотеки взят vector 
    static std::vector<int> subtract(const std::vector<int>& a, const std::vector<int>& b);//&- считай хардлинк просто, чтобы копия не создавалась
    static std::vector<int> multiply(const std::vector<int>& a, const std::vector<int>& b);//static- метод Вызывается у классе, а не у объекта: a.add(b)	Вызывается у класса: BigInteger::add(x, y)
    static std::vector<int> divide(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& remainder);//остаток ремайндер
    static bool less(const std::vector<int>& a, const std::vector<int>& b);
    static bool equal(const std::vector<int>& a, const std::vector<int>& b);

public:
    // Конструкторы
    BigInteger();
    BigInteger(long long num);//8 байт на число. int обычно 4
    BigInteger(const std::string& str);
    BigInteger(const BigInteger& other);// копирование
    
    // Операторы присваивания
    BigInteger& operator=(const BigInteger& other);
    BigInteger& operator=(long long num);
    BigInteger& operator=(const std::string& str);
    
    // Арифметические операторы
    BigInteger operator+(const BigInteger& other) const;// нн изменит поле первого вектора
    BigInteger operator-(const BigInteger& other) const;
    BigInteger operator*(const BigInteger& other) const;
    BigInteger operator/(const BigInteger& other) const;
    BigInteger operator%(const BigInteger& other) const;
    
    // Операторы сравнения
    bool operator==(const BigInteger& other) const; //BigInteger c = a.operator+(b);  // Вызов метода operator+ у объекта a
    bool operator!=(const BigInteger& other) const;
    bool operator<(const BigInteger& other) const;
    bool operator<=(const BigInteger& other) const;
    bool operator>(const BigInteger& other) const;
    bool operator>=(const BigInteger& other) const;
    
    // Операторы присваивания с арифметикой
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator-=(const BigInteger& other);
    BigInteger& operator*=(const BigInteger& other);
    BigInteger& operator/=(const BigInteger& other);
    BigInteger& operator%=(const BigInteger& other);
    
    // Унарные операторы
    BigInteger operator-() const;// если нет ссылки, то создает новый объект.
    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);// так надо чтобы различить от оператора без ссылки. ничего int не делает
    
    // Методы для RSA
    BigInteger modPow(const BigInteger& exponent, const BigInteger& modulus) const; //c = m^e mod n
    BigInteger modInverse(const BigInteger& modulus) const; //(this * x) % modulus = 1
    BigInteger gcd(const BigInteger& other) const;
    
    // Проверка на простоту (тест Миллера-Рабина)
    bool isPrime(int iterations = 10) const;
    
    // Генерация случайного простого числа
    static BigInteger generateRandomPrime(int bits);
    
    // Преобразование в строку
    std::string toString() const;
    
    // Получение размера в битах
    int bitLength() const;
    
    // Дружественные функции
    friend std::ostream& operator<<(std::ostream& os, const BigInteger& num);// friend функция не является методом класса, но может видеть поля private
    friend std::istream& operator>>(std::istream& is, BigInteger& num);
};

// Математические функции для работы с BigInteger
class BigIntegerMath {
public:
    // Расширенный алгоритм Евклида
    static BigInteger extendedGCD(const BigInteger& a, const BigInteger& b, 
                                  BigInteger& x, BigInteger& y); // для нахождения коэффицентов, с которые необходимы для поиска d
    
    // Наименьшее общее кратное
    static BigInteger lcm(const BigInteger& a, const BigInteger& b);
    
    // Случайное число в диапазоне
    static BigInteger random(const BigInteger& min, const BigInteger& max);
    
    // Функция Эйлера для произведения простых чисел
    static BigInteger eulerPhi(const BigInteger& p, const BigInteger& q);
};

#endif