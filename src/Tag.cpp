// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################


#include "Tag.h"


void Tag::print() const
    {
    Serial.print("UniqueID = ");
    for (uint8_t index = 0; index < uniqueIdLength; index++)
        {
        uint8_t theValue = uniqueId[index];
        if (theValue < 16)
            {
            Serial.print("0x0");
            }
        else
            {
            Serial.print("0x");
            }
        Serial.print(theValue, HEX);
        Serial.print(" ");
        }
    }
