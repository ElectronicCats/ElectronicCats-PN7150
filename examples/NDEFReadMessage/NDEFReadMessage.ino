/**
 * Example to read NDEF messages
 * Authors:
 *        Salvador Mendoza - @Netxing - salmg.net
 *        Francisco Torres - Electronic Cats - electroniccats.com
 *
 *  August 2023
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

// Function prototypes
void messageReceivedCallback();
String getHexRepresentation(const byte *data, const uint32_t dataSize);
void displayDeviceInfo();
void displayRecordInfo(NdefRecord record);

// Create a global NFC device interface object, attached to pins 11 (IRQ) and 13 (VEN) and using the default I2C address 0x28
Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);

NdefMessage message;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Detect NFC tags with PN7150/60");

  // Register a callback function to be called when an NDEF message is received
  nfc.setReadMsgCallback(messageReceivedCallback);

  Serial.println("Initializing...");

  if (nfc.begin()) {
    Serial.println("Error initializing PN7150");
    while (true)
      ;
  }

  message.begin();
  nfc.setReaderWriterMode();
  Serial.print("Waiting for a Card...");
}

void loop() {
  if (nfc.isTagDetected()) {
    displayDeviceInfo();
    switch (nfc.remoteDevice.getProtocol()) {
      // Read NDEF message from NFC Forum Type 1, 2, 3, 4, 5 tags
      case nfc.protocol.T1T:
      case nfc.protocol.T2T:
      case nfc.protocol.T3T:
      case nfc.protocol.ISODEP:
      case nfc.protocol.MIFARE:
        nfc.readNdefMessage();
        break;
      case nfc.protocol.ISO15693:
      default:
        break;
    }

    // It can detect multiple cards at the same time if they are the same technology
    if (nfc.remoteDevice.hasMoreTags()) {
      nfc.activateNextTagDiscovery();
      Serial.println("Multiple cards are detected!");
    }
    Serial.println("Remove the Card");
    nfc.waitForTagRemoval();
    Serial.println("Card removed!");
    Serial.println("Restarting...");
    nfc.reset();
    Serial.print("Waiting for a Card");
  }

  Serial.print(".");
  delay(500);
}

/// @brief Callback function called when an NDEF message is received
void messageReceivedCallback() {
  NdefRecord record;
  Serial.println("Processing Callback...");

  // message is automatically updated when a new NDEF message is received
  // only if we call message.begin() in setup()
  if (message.isEmpty()) {
    Serial.println("--- Provisioned buffer size too small or NDEF message empty");
    return;
  }

  Serial.print("NDEF message: ");
  Serial.println(getHexRepresentation(message.getContent(), message.getContentLength()));

  // Show NDEF message details, it is composed of records
  do {
    record.create(message.getRecord());  // Get a new record every time we call getRecord()
    displayRecordInfo(record);
  } while (record.isNotEmpty());
}

String getHexRepresentation(const byte *data, const uint32_t dataSize) {
  String hexString;

  if (dataSize == 0) {
    hexString = "null";
  }

  for (uint32_t index = 0; index < dataSize; index++) {
    if (data[index] <= 0xF)
      hexString += "0";
    String hexValue = String(data[index] & 0xFF, HEX);
    hexValue.toUpperCase();
    hexString += hexValue;
    if ((dataSize > 1) && (index != dataSize - 1)) {
      hexString += ":";
    }
  }
  return hexString;
}

void displayDeviceInfo() {
  Serial.println();
  Serial.print("Interface: ");
  switch (nfc.remoteDevice.getInterface()) {
    case nfc.interface.ISODEP:
      Serial.println("ISO-DEP");
      break;
    case nfc.interface.NFCDEP:
      Serial.println("NFC-DEP");
      break;
    case nfc.interface.TAGCMD:
      Serial.println("TAG");
      break;
    case nfc.interface.FRAME:
      Serial.println("FRAME");
      break;
    case nfc.interface.UNDETERMINED:
      Serial.println("UNDETERMINED");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }

  Serial.print("Protocol: ");
  switch (nfc.remoteDevice.getProtocol()) {
    case nfc.protocol.UNDETERMINED:
      Serial.println("UNDETERMINED");
      break;
    case nfc.protocol.T1T:
      Serial.println("T1T");
      break;
    case nfc.protocol.T2T:
      Serial.println("T2T");
      break;
    case nfc.protocol.T3T:
      Serial.println("T3T");
      break;
    case nfc.protocol.ISODEP:
      Serial.println("ISO-DEP");
      break;
    case nfc.protocol.NFCDEP:
      Serial.println("NFC-DEP");
      break;
    case nfc.protocol.ISO15693:
      Serial.println("ISO15693");
      break;
    case nfc.protocol.MIFARE:
      Serial.println("MIFARE");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }

  Serial.print("Technology: ");
  switch (nfc.remoteDevice.getModeTech()) {
    case nfc.modeTech.POLL | nfc.tech.PASSIVE_NFCA:
      Serial.println("PASSIVE NFC A");
      break;
    case nfc.modeTech.POLL | nfc.tech.PASSIVE_NFCB:
      Serial.println("PASSIVE NFC B");
      break;
    case nfc.modeTech.POLL | nfc.tech.PASSIVE_NFCF:
      Serial.println("PASSIVE NFC F");
      break;
    case nfc.modeTech.POLL | nfc.tech.PASSIVE_15693:
      Serial.println("PASSIVE 15693");
      break;
  }
}

void displayRecordInfo(NdefRecord record) {
  if (record.isEmpty()) {
    Serial.println("No more records, exiting...");
    return;
  }

  uint8_t *payload = record.getPayload();
  Serial.println("--- NDEF record received:");

  switch (record.getType()) {
    case record.type.MEDIA_VCARD:
      Serial.println("vCard:");
      Serial.println(record.getVCardContent());
      break;

    case record.type.WELL_KNOWN_SIMPLE_TEXT:
      Serial.println("\tWell known simple text");
      Serial.println("\t- Text record: " + record.getText());
      break;

    case record.type.WELL_KNOWN_SIMPLE_URI:
      Serial.println("\tWell known simple URI");
      Serial.print("\t- URI record: ");
      Serial.println(record.getUri());
      break;

    case record.type.MEDIA_HANDOVER_WIFI:
      Serial.println("\tReceived WIFI credentials:");
      Serial.println("\t- SSID: " + record.getWiFiSSID());
      Serial.println("\t- Network key: " + record.getWiFiPassword());
      Serial.println("\t- Authentication type: " + record.getWiFiAuthenticationType());
      Serial.println("\t- Encryption type: " + record.getWiFiEncryptionType());
      break;

    case record.type.WELL_KNOWN_HANDOVER_SELECT:
      Serial.print("\tHandover select version: ");
      Serial.print(*payload >> 4);
      Serial.print(".");
      Serial.println(*payload & 0xF);
      break;

    case record.type.WELL_KNOWN_HANDOVER_REQUEST:
      Serial.print("\tHandover request version: ");
      Serial.print(*payload >> 4);
      Serial.print(".");
      Serial.println(*payload & 0xF);
      break;

    case record.type.MEDIA_HANDOVER_BT:
      Serial.println("\tBluetooth handover");
      Serial.println("\t- Bluetooth name: " + record.getBluetoothName());
      Serial.println("\t- Bluetooth address: " + record.getBluetoothAddress());
      break;

    case record.type.MEDIA_HANDOVER_BLE:
      Serial.print("\tBLE Handover");
      Serial.println("\t- Payload size: " + String(record.getPayloadLength()) + " bytes");
      Serial.print("\t- Payload = ");
      Serial.println(getHexRepresentation(record.getPayload(), record.getPayloadLength()));
      break;

    case record.type.MEDIA_HANDOVER_BLE_SECURE:
      Serial.print("\tBLE secure Handover");
      Serial.println("\t- Payload size: " + String(record.getPayloadLength()) + " bytes");
      Serial.print("\t- Payload = ");
      Serial.println(getHexRepresentation(record.getPayload(), record.getPayloadLength()));
      break;

    default:
      Serial.println("\tUnsupported NDEF record, cannot parse");
      break;
  }

  Serial.println("");
}
