#ifndef Tag_h												// Header Guard
#define Tag_h

// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// ### Credits : Thomas Buhot, for his PN7120 library, the predecessor of    ###
// ### the PN7150. Due to the NFC Specs not being available for free, I used ###
// ### his library to reverse-engineer some of the missing information links ###
// ###                                                                       ###
// #############################################################################


#include <Arduino.h>										// Gives us access to all typical Arduino types and functions


class Tag
    {
    public:
        uint8_t uniqueIdLength = 0;							// How long is the NFCID1 of the tag. Can be 4, 7 or 10 bytes. Typically 4 or 7.
        uint8_t uniqueId[10];								// array to store the NFCID1. Maximum length is 10 bytes at this time..
        unsigned long detectionTimestamp;					// remembers the time at which the tag was detected

    public:
		void print() const;									// prints all properties of the tag to Serial
    };


#endif														// End Header Guard
