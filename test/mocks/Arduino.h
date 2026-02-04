// Mock Arduino.h for native unit testing
// SPDX-FileCopyrightText: Copyright (c) 2024 Electronic Cats
// SPDX-License-Identifier: MIT

#ifndef ARDUINO_H_MOCK
#define ARDUINO_H_MOCK

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

// Arduino types
typedef uint8_t byte;
typedef bool boolean;

// String class mock
class String {
public:
    String()
        : _str("") {
    }
    String(const char* str)
        : _str(str ? str : "") {
    }
    String(const String& other)
        : _str(other._str) {
    }

    const char* c_str() const {
        return _str.c_str();
    }
    size_t length() const {
        return _str.length();
    }
    bool equals(const String& other) const {
        return _str == other._str;
    }

    String operator+(const String& other) const {
        return String((_str + other._str).c_str());
    }

    String& operator+=(const String& other) {
        _str += other._str;
        return *this;
    }

    String& operator+=(char c) {
        _str += c;
        return *this;
    }

    bool operator==(const String& other) const {
        return _str == other._str;
    }

    char charAt(unsigned int index) const {
        if(index < _str.length()) return _str[index];
        return 0;
    }

    String substring(unsigned int beginIndex) const {
        if(beginIndex >= _str.length()) return String("");
        return String(_str.substr(beginIndex).c_str());
    }

    String substring(unsigned int beginIndex, unsigned int endIndex) const {
        if(beginIndex >= _str.length()) return String("");
        return String(_str.substr(beginIndex, endIndex - beginIndex).c_str());
    }

private:
    std::string _str;
};

// Arduino functions
inline unsigned long millis() {
    static unsigned long mock_millis = 0;
    return mock_millis++;
}

inline void delay(unsigned long ms) {
    (void)ms;
}

// Serial mock
class SerialMock {
public:
    void begin(unsigned long baud) {
        (void)baud;
    }
    void print(const char* str) {
        (void)str;
    }
    void print(int val) {
        (void)val;
    }
    void println(const char* str) {
        (void)str;
    }
    void println(int val) {
        (void)val;
    }
    void println() {
    }
};

extern SerialMock Serial;

#endif // ARDUINO_H_MOCK
