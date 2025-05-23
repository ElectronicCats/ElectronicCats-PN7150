/**
 * Example to read a ISO15693 block 8 and show its information
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
#define PN7150_IRQ (11)
#define PN7150_VEN (13)
#define PN7150_ADDR (0x28)

#define BLK_NB_ISO15693 (8)  // Block to be read it

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR, PN7150); // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28,specify PN7150 or PN7160 in constructor

void PrintBuf(const byte* data, const uint32_t numBytes) {  // Print hex data buffer in format
  uint32_t szPos;
  for (szPos = 0; szPos < numBytes; szPos++) {
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      Serial.print(F("0"));
    Serial.print(data[szPos] & 0xff, HEX);
    if ((numBytes > 1) && (szPos != numBytes - 1))
      Serial.print(F(" "));
  }
  Serial.println();
}

void PCD_ISO15693_scenario(void) {
#define BLK_NB_ISO15693 8

  bool status;
  unsigned char Resp[256];
  unsigned char RespSize;
  unsigned char ReadBlock[] = {0x02, 0x20, BLK_NB_ISO15693};

  status = nfc.readerTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
  if ((status == NFC_ERROR) || (Resp[RespSize - 1] != 0x00)) {
    Serial.print("Error reading block: ");
    Serial.print(ReadBlock[2], HEX);
    Serial.print(" with error: ");
    Serial.print(Resp[RespSize - 1], HEX);
    return;
  }
  Serial.print("------------------------Block ");
  Serial.print(BLK_NB_ISO15693, HEX);
  Serial.println("-------------------------");
  PrintBuf(Resp + 1, RespSize - 2);
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Read ISO15693 data block 8 with PN7150/60");

  Serial.println("Initializing...");
  if (nfc.connectNCI()) {  // Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1)
      ;
  }

  if (nfc.configureSettings()) {
    Serial.println("The Configure Settings is failed!");
    while (1)
      ;
  }

  if (nfc.configMode()) {  // Set up the configuration mode
    Serial.println("The Configure Mode is failed!!");
    while (1)
      ;
  }
  nfc.startDiscovery();  // NCI Discovery mode
  Serial.println("Waiting for an ISO15693 Card...");
}

void loop() {
  if (nfc.isTagDetected()) {
    switch (nfc.remoteDevice.getProtocol()) {
      case nfc.protocol.ISO15693:
        Serial.println(" - Found ISO15693 card");
        switch (nfc.remoteDevice.getModeTech()) {  // Indetify card technology
          case (nfc.tech.PASSIVE_15693):
            char tmp[16];
            Serial.print("\tID = ");
            PrintBuf(nfc.remoteDevice.getID(), sizeof(nfc.remoteDevice.getID()));

            Serial.print("\tAFI = ");
            sprintf(tmp, "0x%.2X", nfc.remoteDevice.getAFI());
            Serial.print(tmp);
            Serial.println(" ");

            Serial.print("\tDSFID = ");
            sprintf(tmp, "0x%.2X", nfc.remoteDevice.getDSFID());
            Serial.print(tmp);
            Serial.println(" ");
            break;
        }
        PCD_ISO15693_scenario();
        break;

      default:
        Serial.println(" - Found a card, but it is not ISO15693!");
        break;
    }

    //* It can detect multiple cards at the same time if they use the same protocol
    if (nfc.remoteDevice.hasMoreTags()) {
      nfc.activateNextTagDiscovery();
    }

    Serial.println("Remove the Card");
    nfc.waitForTagRemoval();
    Serial.println("CARD REMOVED!");;
  }
  nfc.reset();
  Serial.println("Waiting for an ISO15693 Card...");
  delay(500);
}
