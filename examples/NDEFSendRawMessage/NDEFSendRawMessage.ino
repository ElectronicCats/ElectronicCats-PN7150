/**
 * Example to create a custom NDEF message and send it using the PN7150.
 *
 * Note: If you know how to create custom NDEF messages, you can use this example, otherwise, use the NDEFSendMessage example.
 *
 * Authors:
 *        Salvador Mendoza - @Netxing - salmg.net
 *        Francisco Torres - Electronic Cats - electroniccats.com
 *
 * December 2023
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#include <Electroniccats_PN7150.h>

#define PN7150_IRQ (11)
#define PN7150_VEN (13)
#define PN7150_ADDR (0x28)

// Function prototypes
void messageSentCallback();

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR, PN7150);
//Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR, PN7160);
// Creates a global NFC device interface object, attached to pins 11 (IRQ) and 13 (VEN) and using the default I2C address 0x28
NdefMessage message;

// Three records, "Hello", "world" and Uri "https://www.electroniccats.com"
const char ndefMessage[] = {0x91,                                                                                       // MB/ME/CF/1/IL/TNF
                            0x01,                                                                                       // Type length (1 byte)
                            0x08,                                                                                       // Payload length
                            'T',                                                                                        // Type -> 'T' for text, 'U' for URI
                            0x02,                                                                                       // Status
                            'e', 'n',                                                                                   // Language
                            'H', 'e', 'l', 'l', 'o',                                                                    // Message Payload
                            0x11,                                                                                       // MB/ME/CF/1/IL/TNF
                            0x01,                                                                                       // Type length (1 byte)
                            0x08,                                                                                       // Payload length
                            'T',                                                                                        // Type -> 'T' for text, 'U' for URI
                            0x02,                                                                                       // Status
                            'e', 'n',                                                                                   // Language
                            'w', 'o', 'r', 'l', 'd',                                                                    // Message Payload
                            0x51,                                                                                       // MB/ME/CF/1/IL/TNF
                            0x01,                                                                                       // Type length (1 byte)
                            0x13,                                                                                       // Payload length
                            'U',                                                                                        // Type -> 'T' for text, 'U' for URI
                            0x02,                                                                                       // Status
                            'e', 'l', 'e', 'c', 't', 'r', 'o', 'n', 'i', 'c', 'c', 'a', 't', 's', '.', 'c', 'o', 'm'};  // Message Payload

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Send NDEF Message with PN7150/60");

  message.setContent(ndefMessage, sizeof(ndefMessage));
  nfc.setSendMsgCallback(messageSentCallback);

  Serial.println("Initializing...");

  if (nfc.begin()) {
    Serial.println("Error initializing PN7150");
    while (true)
      ;
  }

  // Needed to detect readers
  nfc.setEmulationMode();
  Serial.print("Waiting for an NDEF device");
}

void loop() {
  Serial.print(".");

  if (nfc.isReaderDetected()) {
    Serial.println("\nReader detected!");
    Serial.println("Sending NDEF message...");
    nfc.sendMessage();
    Serial.print("\nWaiting for an NDEF device");
  }
}

void messageSentCallback() {
  Serial.println("NDEF message sent!");
  // Do something...
}
