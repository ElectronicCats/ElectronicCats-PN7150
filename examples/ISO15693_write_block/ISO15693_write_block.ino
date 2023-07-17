/**
 * Example to write a ISO15693 block 8 and show its information 
 * Authors: 
 *        Salvador Mendoza - @Netxing - salmg.net
 *        For Electronic Cats - electroniccats.com
 * 
 *  November 2020
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

#define BLK_NB_ISO15693 (8)                        //Block to write
#define DATA_WRITE_ISO15693 0x11, 0x22, 0x33, 0x44 //Data to write

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);    // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
RfIntf_t RfInterface;                                              //Intarface to save data for multiple tags

uint8_t mode = 1;                                                  // modes: 1 = Reader/ Writer, 2 = Emulation

void setup(){
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Write ISO15693 data block 8 with PN7150");
  
  Serial.println("Initializing...");                
  if (nfc.connectNCI()) { //Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1);
  }
  
  if (nfc.ConfigureSettings()) {
    Serial.println("The Configure Settings is failed!");
    while (1);
  }
  
  if(nfc.ConfigMode(mode)){ //Set up the configuration mode
    Serial.println("The Configure Mode is failed!!");
    while (1);
  }
  nfc.StartDiscovery(mode); //NCI Discovery mode
  Serial.println("Waiting for an ISO15693 Card ...");
}

void ResetMode(){                                      //Reset the configuration mode after each reading
  Serial.println("Re-initializing...");
  nfc.ConfigMode(mode);                               
  nfc.StartDiscovery(mode);
}

void PrintBuf(const byte * data, const uint32_t numBytes){ //Print hex data buffer in format
  uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++){
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      Serial.print(F("0"));
    Serial.print(data[szPos]&0xff, HEX);
    if ((numBytes > 1) && (szPos != numBytes - 1))
      Serial.print(F(" "));
  }
  Serial.println();
}

void PCD_ISO15693_scenario (void){
    bool status;
    unsigned char Resp[256];
    unsigned char RespSize;
    unsigned char ReadBlock[] = {0x02, 0x20, BLK_NB_ISO15693};
    unsigned char WriteBlock[] = {0x02, 0x21, BLK_NB_ISO15693, DATA_WRITE_ISO15693};

    // Write
    status = nfc.ReaderTagCmd(WriteBlock, sizeof(WriteBlock), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
    {
        Serial.print("Error writing block: "); 
        Serial.print(ReadBlock[2],HEX); 
        Serial.print(" with error: ");
        Serial.print(Resp[RespSize-1],HEX);
        return;
    }
    Serial.print("Wrote: ");
    Serial.println(WriteBlock[2]);
    
    // Read
    status = nfc.ReaderTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x00)){
        Serial.print("Error reading block: "); 
        Serial.print(ReadBlock[2],HEX); 
        Serial.print(" with error: ");
        Serial.print(Resp[RespSize-1],HEX);
        return;
    }
    Serial.print("------------------------Block ");
    Serial.print(BLK_NB_ISO15693, HEX);
    Serial.println("-------------------------");
    PrintBuf(Resp+1, RespSize-2);
}

void loop(){
  if(!nfc.WaitForDiscoveryNotification(&RfInterface)){ // Waiting to detect cards
    switch(RfInterface.Protocol) {
      case PROT_ISO15693:
        Serial.println(" - Found ISO15693 card");
        switch(RfInterface.ModeTech) { //Indetify card technology
            case (MODE_POLL | TECH_PASSIVE_15693):
                char tmp[16];
                Serial.print("\tID = ");
                PrintBuf(RfInterface.Info.NFC_VPP.ID, sizeof(RfInterface.Info.NFC_VPP.ID));
                
                Serial.print("\tAFI = ");
                sprintf(tmp, "0x%.2X",RfInterface.Info.NFC_VPP.AFI);
                Serial.print(tmp); Serial.println(" ");
                
                Serial.print("\tDSFID = ");
                sprintf(tmp, "0x%.2X",RfInterface.Info.NFC_VPP.DSFID);
                Serial.print(tmp); Serial.println(" ");
            break;
          }
          PCD_ISO15693_scenario();
          break;
      
      default:
          Serial.println(" - Found a card, but it is not ISO15693!");
          break;
    }
    
    //* It can detect multiple cards at the same time if they use the same protocol 
    if(RfInterface.MoreTags) {
        nfc.ReaderActivateNext(&RfInterface);
    }
    //* Wait for card removal 
    nfc.ProcessReaderMode(RfInterface, PRESENCE_CHECK);
    Serial.println("CARD REMOVED!");
    
    nfc.StopDiscovery();
    nfc.StartDiscovery(mode);
  }
  ResetMode();
  delay(500);
}
