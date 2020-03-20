#ifndef PN7150Interface_h // Header Guard
#define PN7150Interface_h

// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

// Summary :
//   This library implements the hardware interface for the NXP PN7150 NFC device.
//   The PN7150 uses I2C plus 2 additional data signals : IRQ and VEN
//     IRQ : output of the PN7150, input for the MCU. Through this signal the PN7150 signals it has data to be read by the MCU
//     VEN : input of the PN7150, output for the MCU. Through this signal the MCU can RESET the PN7150
//
//   The library provides :
//   * init() : Initialization of the interface
//   * read() : Read message from PN7150 over I2C
//   * write() : Write message to PN7150 over I2C
//   * hasMessage() : Check if PN7150 has message waiting for MCU

#include <Arduino.h>                          // Gives us access to all typical Arduino types and functions
                                              // The HW interface between The PN7150 and the DeviceHost is I2C, so we need the I2C library.library
#if defined(TEENSYDUINO) && defined(KINETISK) // Teensy 3.0, 3.1, 3.2, 3.5, 3.6 :  Special, more optimized I2C library for Teensy boards
#include <i2c_t3.h>                           // Credits Brian "nox771" : see https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3
#else
#include <Wire.h>
#if defined(__AVR__) || defined(__i386__) || defined(ARDUINO_ARCH_SAMD) || defined(ESP8266) || defined(ARDUINO_ARCH_STM32)
#define WIRE Wire
#else // Arduino Due
#define WIRE Wire1
#endif
// TODO :   i2c_t3.h ensures a maximum I2C message of 259, which is sufficient. Other I2C implementations have shorter buffers (32 bytes)
//          See : https://github.com/Strooom/PN7150/issues/7
#endif

class PN7150Interface
{
public:
    PN7150Interface(uint8_t IRQpin, uint8_t VENpin, uint8_t I2Caddress); // Constructor with custom I2C address
    PN7150Interface(uint8_t IRQpin, uint8_t VENpin);                     // Constructor with default I2C address
    int initialize();                                                    // Initialize the HW interface at the Device Host
    uint8_t write(uint8_t data[], uint32_t dataLength) const;            // write data from DeviceHost to PN7150. Returns success (0) or Fail (> 0)
    uint32_t read(uint8_t data[]) const;                                 // read data from PN7150, returns the amount of bytes read
    uint32_t version(); // does the PN7150 indicate it has data for the DeviceHost to be read
    bool hasMessage() const;
private:
    uint8_t IRQpin;     // MCU pin to which IRQ is connected
    uint8_t VENpin;     // MCU pin to which VEN is connected
    uint8_t I2Caddress; // I2C Address at which the PN7150 is found. Default is 0x28, but can be adjusted by setting to pins at the device
                        // sending CORE_RESET_CMD and checking if the PN7150 responds with CORE_RESET_RSP
};

#endif // End Header Guard