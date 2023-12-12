/**
 * Library to manage the NDEF record
 * Authors:
 *        Francisco Torres - Electronic Cats - electroniccats.com
 *
 *  August 2023
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#ifndef NdefRecord_H
#define NdefRecord_H

#include <Arduino.h>

#include "ndef_helper.h"

class NdefRecord {
 private:
  NdefRecordType_e _type;
  uint8_t headerFlags;
  uint8_t typeLength;
  unsigned short payloadSize;
  uint8_t recordType;
  uint8_t status;
  unsigned char *languageCode;
  unsigned char *payload;
  String getHexRepresentation(const byte *data, const uint32_t dataSize);
  String newString;

 public:
  NdefRecord();
  RecordType type;
  void create(NdefRecord_t record);
  bool isEmpty();
  bool isNotEmpty();
  NdefRecordType_e getType();
  unsigned char *getPayload();
  unsigned short getPayloadSize();
  String getText();
  String getBluetoothName();
  String getBluetoothAddress();
  String getWiFiSSID();
  String getWiFiAuthenticationType();
  String getWiFiEncryptionType();
  String getWiFiPassword();
  String getVCardContent();
  String getUri();
  void setPayload(String payload);
  void setHeaderFlags(uint8_t headerFlags);
  void setTypeLength(uint8_t typeLength);
  void setRecordType(uint8_t recordType);
  void setStatus(uint8_t status);
  void setLanguageCode(String languageCode);
  void setPayloadSize(uint8_t payloadSize);
  const char *getContent();
  unsigned short getContentSize();
};

#endif