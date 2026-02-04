// Mock Wire.h for native unit testing
// SPDX-FileCopyrightText: Copyright (c) 2024 Electronic Cats
// SPDX-License-Identifier: MIT

#ifndef WIRE_H_MOCK
#define WIRE_H_MOCK

#include <cstdint>
#include <cstddef>

class TwoWire {
public:
    void begin() {
    }
    void begin(uint8_t address) {
        (void)address;
    }
    void beginTransmission(uint8_t address) {
        (void)address;
    }
    uint8_t endTransmission() {
        return 0;
    }
    uint8_t endTransmission(bool sendStop) {
        (void)sendStop;
        return 0;
    }

    size_t write(uint8_t data) {
        (void)data;
        return 1;
    }
    size_t write(const uint8_t* data, size_t length) {
        (void)data;
        return length;
    }

    uint8_t requestFrom(uint8_t address, uint8_t quantity) {
        (void)address;
        return quantity;
    }

    int available() {
        return 0;
    }
    int read() {
        return 0;
    }
};

extern TwoWire Wire;

#endif // WIRE_H_MOCK
