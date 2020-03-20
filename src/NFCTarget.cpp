// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

#include "NFCTarget.h"
#include "NCI.h"
int tmpc = 0, tmpd = 0, tmpr = 0;

NFCTarget::NFCTarget(NCI &theNCI) : theNCI(theNCI), theState(NFCTargetState::initializing){
}
void NFCTarget::initialize(){
    theNCI.initialize(); // initialize the NCI stateMachine and other. Will in its turn initialize the HW interface
    theState = NFCTargetState::initializing;
}

bool NFCTarget::run(){
    theNCI.run();
    switch (theState){
        case NFCTargetState::initializing:{
            switch (theNCI.getState()){
                case NciState::RfIdleCmd:{
                    tmpc = theNCI.ConfigureMode();
                    if (!tmpc){
                        Serial.println("Set up config");
                        theState = NFCTargetState::startDiscovery;
                    }
                }
                break;

                default:
                    break;
            }
        }
        break;
        case NFCTargetState::startDiscovery:{
            switch (theNCI.getState()){
                case NciState::RfIdleCmd: {
                    tmpd = theNCI.StartDiscovery();
                    if (!tmpd){
                        Serial.println("Set up discover mode");
                        theState = NFCTargetState::picc;
                    }
                }
                break;
            }
        }
        break;
        case NFCTargetState::picc:{
            switch (theNCI.getState()){
                case NciState::RfIdleCmd:{
                    Serial.println("Hunting reader...");
                    tmpr = theNCI.PICC_ISO14443_4_scenario();
                    if (tmpr){
                        Serial.println("Found it!!!");
                        initialize();
                    }
                }
                break;
            }
        }
        break;

        if (theNCI.getState() == NciState::Error){
            Serial.println("Error : Re-initializing NCI");
            theNCI.initialize(); // initialize the NCI stateMachine and other. Will in its turn initialize the HW interface
            while (1)
                ;
        }
    }
}