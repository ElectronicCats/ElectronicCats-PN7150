// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

#ifndef NFCTarget_h // Header Guard
#define NFCTarget_h

#include <Arduino.h> // Gives us access to all typical Arduino types and functions

class NCI; // Forward declaration

enum class NFCTargetState : uint8_t
{
    initializing,
    startDiscovery,
    picc
};

class NFCTarget
{
    private:
        NCI &theNCI; // reference to the NCI instance being used to talk to the NFC device
        NFCTargetState theState;

    public:
        NFCTarget(NCI &theNCI); // constructor
        bool run();            // initialize the Reader/Writer application. Will in its turn initialize the NCI layer and the HW interface on which the application relies
        void initialize();                                  // initialize the Reader/Writer application. Will in its turn initialize the NCI layer and the HW interface on which the application relies

        NFCTargetState getState() const;
};

#endif