/**
 * Example to send NDEF messages
 *
 * Note: If you know how to create custom NDEF messages, you can use the NDEFSendRawMessage example, otherwise, use this example.
 *
 * Authors:
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

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);
// Creates a global NFC device interface object, attached to pins 11 (IRQ) and 13 (VEN) and using the default I2C address 0x28
NdefMessage message;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Send NDEF Message with PN7150/60");

  message.addTextRecord("Hello");                          // English by default
  message.addTextRecord("world", "en");                    // English explicitly, the library only supports two letter language codes (ISO 639-1) by now
  message.addTextRecord("Hola mundo!", "es");              // Spanish explicitly, check a language code table at https://www.science.co.il/language/Locale-codes.php
  message.addTextRecord("Bonjour le monde!", "fr");        // French explicitly
  message.addUriRecord("google.com");                      // No prefix explicitly
  message.addUriRecord("https://www.electroniccats.com");  // https://www. prefix explicitly
  message.addUriRecord("tel:1234567890");                  // The library can handle all the prefixes listed at https://github.com/ElectronicCats/ElectronicCats-PN7150/blob/master/API.md#uri-prefixes
  String ssid = "Bomber Cat";                              // SSID of the WiFi network
  String authenticationType = "WPA2 PERSONAL";             // Supported authentication types at https://github.com/ElectronicCats/ElectronicCats-PN7150/blob/master/API.md#wifi-authentication-types
  String encryptionType = "AES";                           // Supported encryption types at https://github.com/ElectronicCats/ElectronicCats-PN7150/blob/master/API.md#wifi-encryption-types
  String password = "Password";                            // Password of the WiFi network
  message.addWiFiRecord(ssid, authenticationType, encryptionType, password);
  message.addUriRecord("mailto:deimoshall@gmail.com");
  message.addMimeMediaRecord("text/plain", "Hello world!", 12);  // Media-type as defined in RFC 2046
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
