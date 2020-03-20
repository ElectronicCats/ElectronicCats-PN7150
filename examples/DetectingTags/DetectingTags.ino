/**
 * Example detect tags and show their unique ID 
 * Authors: 
 *        Pascal Roobrouck - @strooom
 *        Andres Sabas - Electronic Cats
 *  March 2020
 * 
 * This code is beerware; if you see me (or any other collaborator 
 * member) at the local, and you've found our code helpful, 
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */
#include "PN7150Interface.h"									// NCI protocol runs over a hardware interface, in this case an I2C with 2 extra handshaking signals
#include "NCI.h"												// Talking to the NFC module is done in an NCI language
#include "NFCReaderWriter.h"									// Implementing a NFC Reader/Writer application


PN7150Interface	theInterface = PN7150Interface(8, 7, 0x28);	    // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
NCI theNCI(theInterface);										// creates a global NCI object, referring to the underlaying HW interface object. Application mode is set default to NciApplicationMode::CardReadWrite
NFCReaderWriter theReaderWriter(theNCI);						// creates a global NFC Reader/Writer application object, referring to the underlaying NCI object.

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Example detect tags PN7150");
  
  theReaderWriter.begin();								// initialize the application object. It will will in its turn initialize the underlaying NCI object, and this one in its turn the HW interface object
}

void loop()
{
    theReaderWriter.run();										// give the application object some CPU time to do its job. This is a non-blocking function
}