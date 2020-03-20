// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

#include "PN7150Interface.h" // NCI protocol runs over a hardware interface, in this case an I2C with 2 extra handshaking signals

PN7150Interface::PN7150Interface(uint8_t IRQpin, uint8_t VENpin) : IRQpin(IRQpin), VENpin(VENpin), I2Caddress(0x28)
{
    // Constructor, initializing IRQpin and VENpin and setting I2Caddress to a default value of 0x28
}

PN7150Interface::PN7150Interface(uint8_t IRQpin, uint8_t VENpin, uint8_t I2Caddress) : IRQpin(IRQpin), VENpin(VENpin), I2Caddress(I2Caddress)
{
    // Constructor, initializing IRQpin and VENpin and initializing I2Caddress to a custom value
}

int PN7150Interface::initialize()
{
    pinMode(IRQpin, INPUT);                                             // IRQ goes from PN7150 to DeviceHost, so is an input
    pinMode(VENpin, OUTPUT);                                            // VEN controls the PN7150's mode, so is an output

    // PN7150 Reset procedure : see PN7150 datasheet 12.6.1, 12.6.2.2, Fig 18 and 16.2.2
    digitalWrite(VENpin, LOW);                                          // drive VEN LOW... 
    delay(1);                                                           // ...for at least 10us
    digitalWrite(VENpin, HIGH);                                         // then VEN HIGH again, and wait for 2.5 ms for the device to boot and allow communication
    delay(3);

    Wire.begin();                                                       // Start I2C interface

    return 1;
}

bool PN7150Interface::hasMessage() const
{
    return (HIGH == digitalRead(IRQpin)); // PN7150 indicates it has data by driving IRQ signal HIGH
}

uint8_t PN7150Interface::write(uint8_t txBuffer[], uint32_t txBufferLevel) const
{
    uint32_t nmbrBytesWritten = 0;
    Wire.beginTransmission((uint8_t)I2Caddress);                                // Setup I2C to transmit
    nmbrBytesWritten = Wire.write(txBuffer, txBufferLevel);             // Copy the data into the I2C transmit buffer
    if (nmbrBytesWritten == txBufferLevel)                              // If this worked..
        {
        byte resultCode;
        resultCode = Wire.endTransmission();                            // .. transmit the buffer, while checking for any errors
        return resultCode;
    }
    else
    {
        return 4; // Could not properly copy data to I2C buffer, so treat as other error, see i2c_t3
        //Serial.print("Error escribiendo");
    }
}

uint32_t PN7150Interface::read(uint8_t rxBuffer[]) const
{
    uint32_t bytesReceived; // keeps track of how many bytes we actually received
    if (hasMessage())       // only try to read something if the PN7150 indicates it has something
    {
        // using 'Split mode' I2C read. See UM10936 section 3.5
        bytesReceived = Wire.requestFrom((int)I2Caddress, 3);           // first reading the header, as this contains how long the payload will be

        rxBuffer[0] = Wire.read();
        rxBuffer[1] = Wire.read();
        rxBuffer[2] = Wire.read();
        uint8_t payloadLength = rxBuffer[2];
        if (payloadLength > 0)
            {
            bytesReceived += Wire.requestFrom((int)I2Caddress, payloadLength);      // then reading the payload, if any
            uint32_t index = 3;
            while (index < bytesReceived)
                {
                rxBuffer[index] = Wire.read();
                index++;
            }
            index = 0;
            /*Serial.print("Reading: ");
            while (index < bytesReceived+3)
                {
                Serial.print("0x");Serial.print(rxBuffer[index],HEX); Serial.print(" ");
                index++;
            }
            Serial.println("");*/
        }
    }
    else
    {
        bytesReceived = 0;
    }
    return bytesReceived;
}

uint32_t PN7150Interface::version()
{
    uint8_t tmpBuffer[] = {0x20, 0x00, 0x01, 0x01};
    write(tmpBuffer, 4);
    uint32_t version = 0;

    delay(5); // How much delay do you need to check if there is an answer from the Device ?

    uint8_t tmpRxBuffer[260];
    uint32_t nmbrBytesReceived;

    nmbrBytesReceived = read(tmpRxBuffer);

    if (6 == nmbrBytesReceived)
    {

        Serial.print("NCI Version = "); // See NCI V1.0 Specification Table 6. 0x17 = V1.7 ?? Not sure about this as I don't have official specs from NCI as they are quite expensive
        Serial.print(tmpRxBuffer[4]);
        Serial.println("");
    }
    else
    {
        Serial.print(nmbrBytesReceived);
        Serial.println(" bytes received, 6 bytes expected - error");
    }
    return version;
}