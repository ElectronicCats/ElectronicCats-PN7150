/**
   Example to NDEF Message
   Authors:
          Salvador Mendoza - @Netxing - salmg.net
          For Electronic Cats - electroniccats.com

    March 2020

   This code is beerware; if you see me (or any other collaborator
   member) at the local, and you've found our code helpful,
   please buy us a round!
   Distributed as-is; no warranty is given.
*/

#include "Electroniccats_PN7150.h"
#include "ndef_helper.h"
// #define PN7150_IRQ (15)  // Original
#define PN7150_IRQ (11)  // BomberCat
// #define PN7150_VEN (14)  // Original
#define PN7150_VEN (13)  // BomberCat
#define PN7150_ADDR (0x28)

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);  // Creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
RfIntf_t RfInterface;                                            // Interface to save data for multiple tags

uint8_t mode = 1;  // modes: 1 = Reader/ Writer, 2 = Emulation, 3 = Peer to peer P2P

// #if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
const char NDEF_MESSAGE[] = {0xD1,      // MB/ME/CF/1/IL/TNF
                             0x01,      // TYPE LENGTH
                             0x07,      // PAYLOAD LENTGH
                             'T',       // TYPE
                             0x02,      // Status
                             'e', 'n',  // Language
                             'T', 'e', 's', 't'};

void ndefPush_Cb(unsigned char *pNdefRecord, unsigned short NdefRecordSize) {
  Serial.println("--- NDEF Record sent");
}
// #endif // if defined P2P_SUPPORT || defined CARDEMU_SUPPORT

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("NDEF Message with PN7150");

  /* Register NDEF message to be sent to remote peer */
  // P2P_NDEF_SetMessage((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), *ndefPush_Cb);

  /* Register callback for reception of NDEF message from remote peer */
  // ndefPull_Cb is called when a NDEF message is received from a remote peer
  P2P_NDEF_RegisterPullCallback(ndefPull_Cb);

  Serial.println("Initializing...");
  if (nfc.connectNCI()) {  // Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1)
      ;
  }

  if (nfc.ConfigureSettings()) {
    Serial.println("The Configure Settings failed!");
    while (1)
      ;
  }

  // TODO: Initialize the NFC controller with mode = 2 or 3 will fail, check this later
  // Temporal fix: Initialize the NFC controller with mode = 1 and then change to mode = 2 or 3 with the resetMode() function
  if (nfc.ConfigMode(mode)) {  // Set up the configuration mode
    Serial.println("The Configure Mode failed!!");
    while (1)
      ;
  }
  nfc.StartDiscovery(mode);  // NCI Discovery mode

  mode = 3;
  resetMode();
  Serial.println("Waiting for an NDEF device...");
}

void loop() {
  Serial.print(".");

  if (!nfc.WaitForDiscoveryNotification(&RfInterface, 1000)) {  // Waiting to detect
    Serial.println();
    Serial.print("RfInterface: ");
    switch (RfInterface.Interface) {
      case INTF_ISODEP:
        Serial.println("ISO-DEP");
        break;
      case INTF_NFCDEP:
        Serial.println("NFC-DEP");
        break;
      case INTF_TAGCMD:
        Serial.println("TAG");
        break;
      case INTF_FRAME:
        Serial.println("FRAME");
        break;
      case INTF_UNDETERMINED:
        Serial.println("UNDETERMINED");
        break;
      default:
        Serial.println("UNKNOWN");
        break;
    }

    if (RfInterface.Interface == INTF_NFCDEP || RfInterface.Interface == INTF_ISODEP) {
      if ((RfInterface.ModeTech & MODE_LISTEN) == MODE_LISTEN) {
        Serial.println(" - P2P TARGET MODE: Activated from remote Initiator");
      } else {
        Serial.println(" - P2P INITIATOR MODE: Remote Target activated");
      }

      /* Process with SNEP for NDEF exchange */
      nfc.ProcessP2pMode(RfInterface);
      Serial.println("Peer lost!");
    }

    // Wait for removal
    nfc.ProcessReaderMode(RfInterface, PRESENCE_CHECK);
    Serial.println("Device removed!");

    nfc.StopDiscovery();
    resetMode();
  }
  delay(500);
}

void resetMode() {  // Reset the configuration mode after each reading
  Serial.println("\nRe-initializing...");
  nfc.ConfigMode(mode);
  nfc.StartDiscovery(mode);
}

void ndefPull_Cb(unsigned char *pNdefMessage, unsigned short NdefMessageSize) {
  unsigned char *pNdefRecord = pNdefMessage;
  NdefRecord_t NdefRecord;
  unsigned char save;

  if (pNdefMessage == NULL) {
    Serial.println("--- Provisioned buffer size too small or NDEF message empty");
    return;
  }

  while (pNdefRecord != NULL) {
    Serial.println("--- NDEF record received:");

    NdefRecord = DetectNdefRecordType(pNdefRecord);

    switch (NdefRecord.recordType) {
      case MEDIA_VCARD: {
        save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
        Serial.print("vCard:");
        // Serial.println(NdefRecord.recordPayload);
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
      } break;

      case WELL_KNOWN_SIMPLE_TEXT: {
        save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
        Serial.print("Text record:");
        // Serial.println(&NdefRecord.recordPayload[NdefRecord.recordPayload[0]+1]);
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
      } break;

      case WELL_KNOWN_SIMPLE_URI: {
        save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
        Serial.print("URI record: ");
        // Serial.println(ndef_helper_UriHead(NdefRecord.recordPayload[0]), &NdefRecord.recordPayload[1]);
        NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
      } break;

      case MEDIA_HANDOVER_WIFI: {
        unsigned char index = 0, i;

        Serial.println("--- Received WIFI credentials:");
        if ((NdefRecord.recordPayload[index] == 0x10) && (NdefRecord.recordPayload[index + 1] == 0x0e))
          index += 4;
        while (index < NdefRecord.recordPayloadSize) {
          if (NdefRecord.recordPayload[index] == 0x10) {
            if (NdefRecord.recordPayload[index + 1] == 0x45) {
              Serial.print("- SSID = ");
              for (i = 0; i < NdefRecord.recordPayload[index + 3]; i++)
                Serial.print(NdefRecord.recordPayload[index + 4 + i]);
              Serial.println("");
            } else if (NdefRecord.recordPayload[index + 1] == 0x03) {
              Serial.print("- Authenticate Type = ");
              Serial.println(ndef_helper_WifiAuth(NdefRecord.recordPayload[index + 5]));
            } else if (NdefRecord.recordPayload[index + 1] == 0x0f) {
              Serial.print("- Encryption Type = ");
              Serial.println(ndef_helper_WifiEnc(NdefRecord.recordPayload[index + 5]));
            } else if (NdefRecord.recordPayload[index + 1] == 0x27) {
              Serial.print("- Network key = ");
              for (i = 0; i < NdefRecord.recordPayload[index + 3]; i++)
                Serial.print("#");
              Serial.println("");
            }
            index += 4 + NdefRecord.recordPayload[index + 3];
          } else
            continue;
        }
      } break;

      case WELL_KNOWN_HANDOVER_SELECT:
        Serial.print("Handover select version ");
        Serial.print(NdefRecord.recordPayload[0] >> 4);
        Serial.println(NdefRecord.recordPayload[0] & 0xF);
        break;

      case WELL_KNOWN_HANDOVER_REQUEST:
        Serial.print("Handover request version ");
        Serial.print(NdefRecord.recordPayload[0] >> 4);
        Serial.println(NdefRecord.recordPayload[0] & 0xF);
        break;

      case MEDIA_HANDOVER_BT:
        Serial.print("BT Handover payload = ");
        // Serial.print(NdefRecord.recordPayload);
        // Serial.println(NdefRecord.recordPayloadSize);
        break;

      case MEDIA_HANDOVER_BLE:
        Serial.print("BLE Handover payload = ");
        // Serial.print(NdefRecord.recordPayload);
        // Serial.println(NdefRecord.recordPayloadSize);
        break;

      case MEDIA_HANDOVER_BLE_SECURE:
        Serial.print("BLE secure Handover payload = ");
        // Serial.println(NdefRecord.recordPayload, NdefRecord.recordPayloadSize);
        break;

      default:
        Serial.println("Unsupported NDEF record, cannot parse");
        break;
    }
    pNdefRecord = GetNextRecord(pNdefRecord);
  }

  Serial.println("");
}
// #endif // if defined P2P_SUPPORT || defined RW_SUPPORT
