#include "BigInteger.h"
#include <cmath>
#include <sstream>//для работы с файлами и преобразованием строк
#include <iomanip>// мб не использовалась

using namespace std;

// Вспомогательные методы
void BigInteger::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.size() == 1 && digits[0] == 0) {
        isNegative = false;
    }
}

bool BigInteger::less(const vector<int>& a, const vector<int>& b) {
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = a.size() - 1; i >= 0; --i) {
        if (a[i] != b[i]) return a[i] < b[i];
    }
    return false;
}

bool BigInteger::equal(const vector<int>& a, const vector<int>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

vector<int> BigInteger::add(const vector<int>& a, const vector<int>& b) {
    vector<int> result;
    int carry = 0;
    size_t maxSize = max(a.size(), b.size());
    
    for (size_t i = 0; i < maxSize || carry; ++i) {// пока есть разряды или есть carry
        int sum = carry;
        if (i < a.size()) sum += a[i];
        if (i < b.size()) sum += b[i];
        
        result.push_back(sum % 10);
        carry = sum / 10;
    }
    
    return result;
}

vector<int> BigInteger::subtract(const vector<int>& a, const vector<int>& b) {
    vector<int> result;
    int borrow = 0;
    
    for (size_t i = 0; i < a.size(); ++i) {
        int sub = a[i] - borrow;
        if (i < b.size()) sub -= b[i];
        
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result.push_back(sub);
    }
    
    while (result.size() > 1 && result.back() == 0) {
        result.pop_back();
    }
    
    return result;
}

vector<int> BigInteger::multiply(const vector<int>& a, const vector<int>& b) {
    vector<int> result(a.size() + b.size(), 0);//заполняет нулями
    
    for (size_t i = 0; i < a.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < b.size() || carry; ++j) {
            int prod = result[i + j] + a[i] * (j < b.size() ? b[j] : 0) + carry;
            result[i + j] = prod % 10;
            carry = prod / 10;
        }
    }
    
    while (result.size() > 1 && result.back() == 0) {
        result.pop_back();
    }
    
    return result;
}

vector<int> BigInteger::divide(const vector<int>& a, const vector<int>& b, vector<int>& remainder) {
    vector<int> quotient;
    remainder.clear();
    
    for (int i = a.size() - 1; i >= 0; --i) {
        remainder.insert(remainder.begin(), a[i]);
        while (remainder.size() > 1 && remainder.back() == 0) {
            remainder.pop_back();
        }
        
        int count = 0;
        while (!less(remainder, b)) {
            remainder = subtract(remainder, b);
            count++;
        }
        
        quotient.insert(quotient.begin(), count);
    }
    
    while (quotient.size() > 1 && quotient.back() == 0) {
        quotient.pop_back();
    }
    
    return quotient;
}

// Конструкторы
BigInteger::BigInteger() : isNegative(false) {
    digits.push_back(0);
}

BigInteger::BigInteger(long long num) {
    isNegative = num < 0;
    num = abs(num);
    
    if (num == 0) {
        digits.push_back(0);
    } else {
        while (num > 0) {
            digits.push_back(num % 10);
            num /= 10;
        }
    }
}

BigInteger::BigInteger(const string& str) {
    if (str.empty()) {
        digits.push_back(0);
        isNegative = false;
        return;
    }
    
    size_t start = 0;
    if (str[0] == '-') {
        isNegative = true;
        start = 1;
    } else {
        isNegative = false;
    }
    
    for (int i = str.length() - 1; i >= (int)start; --i) {
        if (!isdigit(str[i])) {
            throw invalid_argument("Invalid character in number string");
        }
        digits.push_back(str[i] - '0');
    }
    
    removeLeadingZeros();
}

BigInteger::BigInteger(const BigInteger& other) : digits(other.digits), isNegative(other.isNegative) {}// копирование

// Операторы присваивания
BigInteger& BigInteger::operator=(const BigInteger& other) {
    if (this != &other) {
        digits = other.digits;
        isNegative = other.isNegative;
    }
    return *this;
}

BigInteger& BigInteger::operator=(long long num) {
    *this = BigInteger(num);
    return *this;
}

BigInteger& BigInteger::operator=(const string& str) {
    *this = BigInteger(str);
    return *this;
}

// Арифметические операторы
BigInteger BigInteger::operator+(const BigInteger& other) const {
    if (isNegative == other.isNegative) {
        BigInteger result;
        result.digits = add(digits, other.digits);
        result.isNegative = isNegative;
        result.removeLeadingZeros();
        return result;
    }
    
    if (isNegative) {
        return other - (-(*this));
    } else {
        return *this - (-other);
    }
}

BigInteger BigInteger::operator-(const BigInteger& other) const {
    if (isNegative != other.isNegative) {
        return *this + (-other);
    }
    
    if (isNegative) {
        return (-other) - (-(*this));
    }
    
    if (less(digits, other.digits)) {
        BigInteger result;
        result.digits = subtract(other.digits, digits);
        result.isNegative = true;
        result.removeLeadingZeros();
        return result;
    }
    
    BigInteger result;
    result.digits = subtract(digits, other.digits);//вычитает два вектора
    result.isNegative = false;
    result.removeLeadingZeros();
    return result;
}

BigInteger BigInteger::operator*(const BigInteger& other) const {
    BigInteger result;
    result.digits = multiply(digits, other.digits);
    result.isNegative = isNegative != other.isNegative;
    result.removeLeadingZeros();
    return result;
}

BigInteger BigInteger::operator/(const BigInteger& other) const {
    if (other == BigInteger(0)) {
        throw runtime_error("Division by zero");
    }
    
    vector<int> remainder;
    BigInteger result;
    result.digits = divide(digits, other.digits, remainder);
    result.isNegative = isNegative != other.isNegative;
    result.removeLeadingZeros();
    return result;
}

BigInteger BigInteger::operator%(const BigInteger& other) const {
    if (other == BigInteger(0)) {
        throw runtime_error("Modulo by zero");
    }
    
    vector<int> remainder;
    divide(digits, other.digits, remainder);
    BigInteger result;
    result.digits = remainder;
    result.isNegative = isNegative;
    result.removeLeadingZeros();
    return result;
}

// Операторы сравнения
bool BigInteger::operator==(const BigInteger& other) const {
    return isNegative == other.isNegative && equal(digits, other.digits);
}

bool BigInteger::operator!=(const BigInteger& other) const {
    return !(*this == other);
}

bool BigInteger::operator<(const BigInteger& other) const {
    if (isNegative != other.isNegative) {
        return isNegative;
    }
    
    if (isNegative) {
        return !less(digits, other.digits) && !equal(digits, other.digits);
    }
    
    return less(digits, other.digits);
}

bool BigInteger::operator<=(const BigInteger& other) const {
    return *this < other || *this == other;
}

bool BigInteger::operator>(const BigInteger& other) const {
    return other < *this;
}

bool BigInteger::operator>=(const BigInteger& other) const {
    return *this > other || *this == other;
}

// Операторы присваивания с арифметикой
BigInteger& BigInteger::operator+=(const BigInteger& other) {
    *this = *this + other;
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
    *this = *this - other;
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
    *this = *this * other;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
    *this = *this / other;
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
    *this = *this % other;
    return *this;
}

// Унарные операторы
BigInteger BigInteger::operator-() const {
    BigInteger result(*this);
    if (result != BigInteger(0)) {
        result.isNegative = !result.isNegative;
    }
    return result;
}

BigInteger& BigInteger::operator++() {
    *this = *this + BigInteger(1);
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger temp(*this);
    ++(*this);
    return temp;
}

BigInteger& BigInteger::operator--() {
    *this = *this - BigInteger(1);
    return *this;
}

BigInteger BigInteger::operator--(int) {
    BigInteger temp(*this);
    --(*this);
    return temp;
}

// Методы для RSA
BigInteger BigInteger::modPow(const BigInteger& exponent, const BigInteger& modulus) const {
    if (modulus == BigInteger(0)) {
        throw runtime_error("Modulus cannot be zero");
    }
    
    BigInteger result(1);
    BigInteger base = *this % modulus;
    BigInteger exp = exponent;
    
    while (exp > BigInteger(0)) {
        if (exp % BigInteger(2) == BigInteger(1)) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exp = exp / BigInteger(2);
    }
    
    return result;
}

BigInteger BigInteger::modInverse(const BigInteger& modulus) const {
    BigInteger a = *this % modulus;
    BigInteger m = modulus;
    BigInteger x, y;
    
    BigInteger g = BigIntegerMath::extendedGCD(a, m, x, y);
    
    if (g != BigInteger(1)) {
        throw runtime_error("Modular inverse does not exist");
    }
    
    return (x % m + m) % m;
}

BigInteger BigInteger::gcd(const BigInteger& other) const {
    BigInteger a = *this;
    BigInteger b = other;
    
    while (b != BigInteger(0)) {
        BigInteger temp = b;
        b = a % b;
        a = temp;
    }
    
    return a;
}

// Проверка на простоту (тест Миллера-Рабина)
bool BigInteger::isPrime(int iterations) const {
    if (*this < BigInteger(2)) return false;
    if (*this == BigInteger(2) || *this == BigInteger(3)) return true;
    if (*this % BigInteger(2) == BigInteger(0)) return false;
    
    // Представляем n-1 как d * 2^r
    BigInteger nMinus1 = *this - BigInteger(1);
    BigInteger d = nMinus1;
    int r = 0;
    
    while (d % BigInteger(2) == BigInteger(0)) {
        d = d / BigInteger(2);
        r++;
    }
    
    // Тестируем iterations раз
    for (int i = 0; i < iterations; ++i) {
        // Выбираем случайное основание a в диапазоне [2, n-2]
        BigInteger a;
        do {
            a = BigIntegerMath::random(BigInteger(2), *this - BigInteger(2));
        } while (a < BigInteger(2) || a > *this - BigInteger(2));
        
        BigInteger x = a.modPow(d, *this);
        
        if (x == BigInteger(1) || x == nMinus1) {
            continue;
        }
        
        bool composite = true;
        for (int j = 0; j < r - 1; ++j) {
            x = (x * x) % *this;
            if (x == nMinus1) {
                composite = false;// composite=составное
                break;
            }
        }
        
        if (composite) {
            return false;
        }
    }
    
    return true;
}

// Генерация случайного простого числа
BigInteger BigInteger::generateRandomPrime(int bits) {
    if (bits < 2) bits = 8;
    
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<int> digitDist(0, 9);
    
    while (true) {
        string numStr;
        
        // Генерируем первую цифру (не ноль)
        numStr += to_string(digitDist(gen) % 9 + 1);
        
        // Генерируем остальные цифры
        for (int i = 1; i < bits / 3.32; ++i) { // 3.32 - приблизительное соотношение бит/десятичная цифра
            numStr += to_string(digitDist(gen));
        }
        
        BigInteger candidate(numStr);
        
        // Делаем нечетным
        if (candidate % BigInteger(2) == BigInteger(0)) {
            candidate = candidate + BigInteger(1);
        }
        
        // Проверяем на простоту
        if (candidate.isPrime()) {
            return candidate;
        }
    }
}

// Преобразование в строку
string BigInteger::toString() const {
    if (digits.empty()) return "0";
    
    string result;
    if (isNegative) {
        result += '-';
    }
    
    for (int i = digits.size() - 1; i >= 0; --i) {
        result += char('0' + digits[i]);
    }
    
    return result;
}

// Получение размера в битах
int BigInteger::bitLength() const {
    BigInteger temp = *this;
    if (temp < BigInteger(0)) {
        temp = -temp;
    }
    
    int bits = 0;
    while (temp > BigInteger(0)) {
        temp = temp / BigInteger(2);
        bits++;
    }
    
    return bits;
}

// Дружественные функции
ostream& operator<<(ostream& os, const BigInteger& num) {
    os << num.toString();
    return os;
}

istream& operator>>(istream& is, BigInteger& num) {
    string str;
    is >> str;
    num = BigInteger(str);
    return is;
}

// Реализация BigIntegerMath
BigInteger BigIntegerMath::extendedGCD(const BigInteger& a, const BigInteger& b, 
                                       BigInteger& x, BigInteger& y) {
    if (b == BigInteger(0)) {
        x = BigInteger(1);
        y = BigInteger(0);
        return a;
    }
    
    BigInteger x1, y1;
    BigInteger g = extendedGCD(b, a % b, x1, y1);
    
    x = y1;
    y = x1 - (a / b) * y1;
    
    return g;
}

BigInteger BigIntegerMath::lcm(const BigInteger& a, const BigInteger& b) {
    return (a * b) / a.gcd(b);
}

BigInteger BigIntegerMath::random(const BigInteger& min, const BigInteger& max) {
    if (min > max) {
        throw invalid_argument("min cannot be greater than max");
    }
    
    static random_device rd;
    static mt19937 gen(rd());
    
    BigInteger range = max - min + BigInteger(1);
    string rangeStr = range.toString();
    
    // Генерируем случайное число в диапазоне
    BigInteger result;
    do {
        string numStr;
        for (size_t i = 0; i < rangeStr.length(); ++i) {
            int digit = gen() % 10;
            if (i == 0 && digit == 0) digit = 1;
            numStr += to_string(digit);
        }
        result = BigInteger(numStr);
    } while (result >= range);
    
    return min + result;
}

BigInteger BigIntegerMath::eulerPhi(const BigInteger& p, const BigInteger& q) {
    return (p - BigInteger(1)) * (q - BigInteger(1));
}