/**
 * Example to detect P2P device 
 * Authors: 
 *        Salvador Mendoza - @Netxing - salmg.net
 *        For Electronic Cats - electroniccats.com
 * 
 *  March 2020
 * 
 * This code is beerware; if you see me (or any other collaborator 
 * member) at the local, and you've found our code helpful, 
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#include "Electroniccats_PN7150.h"          
#define PN7150_IRQ   (15)
#define PN7150_VEN   (14)
#define PN7150_ADDR  (0x28)

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);    // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
RfIntf_t RfInterface;                                              //Intarface to save data for multiple tags

uint8_t mode = 3;                                                  // modes: 1 = Reader/ Writer, 2 = Emulation, 3 = Peer to peer P2P

void ResetMode(){                                  //Reset the configuration mode after each reading
  Serial.println("Re-initializing...");
  nfc.ConfigMode(mode);                               
  nfc.StartDiscovery(mode);
}


void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Detect P2P devices with PN7150");
  
  Serial.println("Initializing...");                
  if (nfc.connectNCI()) { //Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1);
  }
  
  if (nfc.ConfigureSettings()) {
    Serial.println("The Configure Settings failed!");
    while (1);
  }
  
  if(nfc.ConfigMode(mode)){ //Set up the configuration mode
    Serial.println("The Configure Mode failed!!");
    while (1);
  }
  nfc.StartDiscovery(mode); //NCI Discovery mode
  Serial.println("Waiting for a P2P device...");
}

void loop(){
  if(!nfc.WaitForDiscoveryNotification(&RfInterface)){ // Waiting to detect 
    if (RfInterface.Interface == INTF_NFCDEP) {
      if ((RfInterface.ModeTech & MODE_LISTEN) == MODE_LISTEN) 
        Serial.println(" - P2P TARGET MODE: Activated from remote Initiator");
      else 
        Serial.println(" - P2P INITIATOR MODE: Remote Target activated");
  
      /* Process with SNEP for NDEF exchange */
      nfc.ProcessP2pMode(RfInterface);
      Serial.println("Peer lost!");
    }
    ResetMode();
  }
  delay(500);
}
