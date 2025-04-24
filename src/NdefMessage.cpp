/**
 * Library to manage the NDEF message
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

#include "NdefMessage.h"

unsigned char *NdefMessage::content;
unsigned short NdefMessage::contentLength;
unsigned char *NdefMessage::newContent;
unsigned short NdefMessage::newContentLength;
uint8_t NdefMessage::recordCounter;
String NdefMessage::newString;

NdefMessage::NdefMessage() {
  content = NULL;
  contentLength = 0;
  recordCounter = 0;
  newString = "null";
}

void NdefMessage::begin() {
  registerUpdateNdefMessageCallback(NdefMessage::update);
}

String NdefMessage::getHexRepresentation(const byte *data,
                                         const uint32_t dataLength) {
  String hexString;

  if (dataLength == 0) {
    hexString = newString;
  }

  for (uint32_t index = 0; index < dataLength; index++) {
    if (data[index] <= 0xF)
      hexString += "0";
    String hexValue = String(data[index] & 0xFF, HEX);
    hexValue.toUpperCase();
    hexString += hexValue;
    if ((dataLength > 1) && (index != dataLength - 1)) {
      hexString += ":";
    }
  }
  return hexString;
}

void NdefMessage::update(unsigned char *message, unsigned short messageLength) {
  if (content != NULL) {
    free(content);
  }
  content = (unsigned char *)malloc(messageLength);
  memcpy(content, message, messageLength);
  contentLength = messageLength;
}

unsigned char *NdefMessage::getContent() { return content; }

unsigned short NdefMessage::getContentLength() { return contentLength; }

void NdefMessage::setContent(const char *content,
                             unsigned short contentLength) {
  NdefMessage::content = (unsigned char *)content;
  NdefMessage::contentLength = contentLength;

#ifdef DEBUG3
  Serial.println("Content length: " + String(contentLength));
  Serial.println(NdefMessage::getHexRepresentation((byte *)content,
                                                   (uint32_t)contentLength));
#endif
  NdefMessage::updateHeaderFlags();
#ifdef DEBUG3
  Serial.println(NdefMessage::getHexRepresentation((byte *)content,
                                                   (uint32_t)contentLength));
  Serial.println();
#endif
  T4T_NDEF_EMU_SetMsg(content, contentLength);
}

void NdefMessage::updateHeaderFlags() {
  uint8_t headersPositions[recordCounter];
  uint8_t previousHeaders[recordCounter];
  uint8_t recordCounterAux = 0;
#ifdef DEBUG3
  Serial.println("Header positions:");
#endif
  for (uint8_t i = 0; i < contentLength; i++) {
    if (recordCounterAux == recordCounter) {
      break;
    }

    if (isHeaderByte(content[i])) { // New record found
#ifdef DEBUG3
      Serial.println("\t" + String(i) + ": " + String(content[i], HEX) + ",");
#endif
      previousHeaders[recordCounterAux] = content[i];
      headersPositions[recordCounterAux] = i;
      recordCounterAux++;
    }
  }

  // TODO: refactor this to make it more generic
  for (uint8_t i = 0; i < recordCounter; i++) {
    if (i == 0) {
      if ((content[headersPositions[i]] & NDEF_RECORD_TNF_MASK) ==
          NDEF_WELL_KNOWN) {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_SINGLE_RECORD;
      } else {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_SINGLE_MEDIA_RECORD;
      }
    } else if (i == 1) {
      if ((content[headersPositions[i - 1]] & NDEF_RECORD_TNF_MASK) ==
          NDEF_WELL_KNOWN) {
        content[headersPositions[i - 1]] = NDEF_HEADER_FLAGS_FIRST_RECORD;
      } else {
        content[headersPositions[i - 1]] = NDEF_HEADER_FLAGS_FIRST_MEDIA_RECORD;
      }
      if ((content[headersPositions[i]] & NDEF_RECORD_TNF_MASK) ==
          NDEF_WELL_KNOWN) {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_LAST_RECORD;
      } else {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_LAST_MEDIA_RECORD;
      }
    } else {
      if ((content[headersPositions[i - 1]] & NDEF_RECORD_TNF_MASK) ==
          NDEF_WELL_KNOWN) {
        content[headersPositions[i - 1]] = NDEF_HEADER_FLAGS_NEXT_RECORD;
      } else {
        content[headersPositions[i - 1]] = NDEF_HEADER_FLAGS_NEXT_MEDIA_RECORD;
      }

      if ((content[headersPositions[i]] & NDEF_RECORD_TNF_MASK) ==
          NDEF_WELL_KNOWN) {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_LAST_RECORD;
      } else {
        content[headersPositions[i]] = NDEF_HEADER_FLAGS_LAST_MEDIA_RECORD;
      }
    }
  }

#ifdef DEBUG3
  for (uint8_t i = 0; i < recordCounter; i++) {
    Serial.print("Header: " + String(previousHeaders[i], HEX) + " -> ");
    Serial.println(String(content[headersPositions[i]], HEX));
  }
#endif
}

bool NdefMessage::isHeaderByte(unsigned char byte) {
  return (byte == NDEF_HEADER_FLAGS_SINGLE_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_SINGLE_MEDIA_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_FIRST_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_FIRST_MEDIA_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_NEXT_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_NEXT_MEDIA_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_LAST_RECORD) ||
         (byte == NDEF_HEADER_FLAGS_LAST_MEDIA_RECORD);
}

NdefRecord_t NdefMessage::getRecord() {
  NdefRecord_t ndefRecord = DetectNdefRecordType(content);

  if (NdefMessage::isEmpty()) {
    return ndefRecord;
  }

  NdefMessage::getNextRecord();
  return ndefRecord;
}

void NdefMessage::getNextRecord() { content = GetNextRecord(content); }

bool NdefMessage::isEmpty() { return NdefMessage::getContent() == NULL; }

bool NdefMessage::isNotEmpty() { return NdefMessage::getContent() != NULL; }

bool NdefMessage::hasRecord() { return NdefMessage::isNotEmpty(); }

void NdefMessage::addRecord(NdefRecord record) {
#ifdef DEBUG3
  Serial.println("Record length: " + String(record.getContentLength()));
#endif

  uint16_t newLength = contentLength + record.getContentLength();

  if (newLength >= 249) {
#ifdef DEBUG3
    Serial.println("NDEF message is full");
#endif
    updateHeaderFlags();
    return;
  }

  if (record.getContent() == NULL) {
#ifdef DEBUG3
    Serial.println("Record content is NULL");
#endif
    return;
  }

  recordCounter++;

  NdefMessage::newContent = new unsigned char[newLength];
  memcpy(newContent, content, contentLength);
  memcpy(newContent + contentLength, record.getContent(),
         record.getContentLength());
  setContent((const char *)newContent,
             contentLength + record.getContentLength());
}

void NdefMessage::addTextRecord(String text, String languageCode) {
  NdefRecord record;
  record.setHeaderFlags(NDEF_HEADER_FLAGS_SINGLE_RECORD);
  record.setTypeLength(NDEF_TYPE_LENGTH);
  record.setPayloadLength(text.length() +
                          3); // 3 = status + language code length
  record.setRecordType(NDEF_TEXT_RECORD_TYPE);
  record.setStatus(NDEF_STATUS);
  record.setLanguageCode(languageCode);
  record.setPayload(text);

  addRecord(record);
}

void NdefMessage::addTextRecord(String text) {
  addTextRecord(text, NDEF_DEFAULT_LANGUAGE_CODE);
}

void NdefMessage::addUriRecord(String uri) {
  uint8_t statusLength = 1;
  NdefRecord record;
  record.setHeaderFlags(NDEF_HEADER_FLAGS_SINGLE_RECORD);
  record.setTypeLength(NDEF_TYPE_LENGTH);
  record.setRecordType(NDEF_URI_RECORD_TYPE);

  if (uri.startsWith("http://www.")) {
    record.setStatus(NDEF_URI_HTTP_WWWDOT);
    record.setPayload(uri.substring(11).c_str());
    record.setPayloadLength(uri.length() - 11 + statusLength);
  } else if (uri.startsWith("https://www.")) {
    record.setStatus(NDEF_URI_HTTPS_WWWDOT);
    record.setPayload(uri.substring(12).c_str());
    record.setPayloadLength(uri.length() - 12 + statusLength);
  } else if (uri.startsWith("http://")) {
    record.setStatus(NDEF_URI_HTTP);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("https://")) {
    record.setStatus(NDEF_URI_HTTPS);
    record.setPayload(uri.substring(8).c_str());
    record.setPayloadLength(uri.length() - 8 + statusLength);
  } else if (uri.startsWith("tel:")) {
    record.setStatus(NDEF_URI_TEL);
    record.setPayload(uri.substring(4).c_str());
    record.setPayloadLength(uri.length() - 4 + statusLength);
  } else if (uri.startsWith("mailto:")) {
    record.setStatus(NDEF_URI_MAILTO);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("ftp://anonymous:anonymous@")) {
    record.setStatus(NDEF_URI_FTP_ANONIMOUS);
    record.setPayload(uri.substring(26).c_str());
    record.setPayloadLength(uri.length() - 26 + statusLength);
  } else if (uri.startsWith("ftp://ftp.")) {
    record.setStatus(NDEF_URI_FTP_FTPDOT);
    record.setPayload(uri.substring(9).c_str());
    record.setPayloadLength(uri.length() - 9 + statusLength);
  } else if (uri.startsWith("ftps://")) {
    record.setStatus(NDEF_URI_FTPS);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("sftp://")) {
    record.setStatus(NDEF_URI_SFTP);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("smb://")) {
    record.setStatus(NDEF_URI_SMB);
    record.setPayload(uri.substring(6).c_str());
    record.setPayloadLength(uri.length() - 6 + statusLength);
  } else if (uri.startsWith("nfs://")) {
    record.setStatus(NDEF_URI_NFS);
    record.setPayload(uri.substring(6).c_str());
    record.setPayloadLength(uri.length() - 6 + statusLength);
  } else if (uri.startsWith("ftp://")) {
    record.setStatus(NDEF_URI_FTP);
    record.setPayload(uri.substring(6).c_str());
    record.setPayloadLength(uri.length() - 6 + statusLength);
  } else if (uri.startsWith("dav://")) {
    record.setStatus(NDEF_URI_DAV);
    record.setPayload(uri.substring(6).c_str());
    record.setPayloadLength(uri.length() - 6 + statusLength);
  } else if (uri.startsWith("news:")) {
    record.setStatus(NDEF_URI_NEWS);
    record.setPayload(uri.substring(5).c_str());
    record.setPayloadLength(uri.length() - 5 + statusLength);
  } else if (uri.startsWith("telnet://")) {
    record.setStatus(NDEF_URI_TELNET);
    record.setPayload(uri.substring(9).c_str());
    record.setPayloadLength(uri.length() - 9 + statusLength);
  } else if (uri.startsWith("imap:")) {
    record.setStatus(NDEF_URI_IMAP);
    record.setPayload(uri.substring(5).c_str());
    record.setPayloadLength(uri.length() - 5 + statusLength);
  } else if (uri.startsWith("rtsp://")) {
    record.setStatus(NDEF_URI_RTSP);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("pop:")) {
    record.setStatus(NDEF_URI_POP);
    record.setPayload(uri.substring(4).c_str());
    record.setPayloadLength(uri.length() - 4 + statusLength);
  } else if (uri.startsWith("sip:")) {
    record.setStatus(NDEF_URI_SIP);
    record.setPayload(uri.substring(4).c_str());
    record.setPayloadLength(uri.length() - 4 + statusLength);
  } else if (uri.startsWith("sips:")) {
    record.setStatus(NDEF_URI_SIPS);
    record.setPayload(uri.substring(5).c_str());
    record.setPayloadLength(uri.length() - 5 + statusLength);
  } else if (uri.startsWith("tftp:")) {
    record.setStatus(NDEF_URI_TFTP);
    record.setPayload(uri.substring(5).c_str());
    record.setPayloadLength(uri.length() - 5 + statusLength);
  } else if (uri.startsWith("btspp://")) {
    record.setStatus(NDEF_URI_BTSPP);
    record.setPayload(uri.substring(8).c_str());
    record.setPayloadLength(uri.length() - 8 + statusLength);
  } else if (uri.startsWith("btl2cap://")) {
    record.setStatus(NDEF_URI_BTL2CAP);
    record.setPayload(uri.substring(10).c_str());
    record.setPayloadLength(uri.length() - 10 + statusLength);
  } else if (uri.startsWith("btgoep://")) {
    record.setStatus(NDEF_URI_BTGOEP);
    record.setPayload(uri.substring(9).c_str());
    record.setPayloadLength(uri.length() - 9 + statusLength);
  } else if (uri.startsWith("tcpobex://")) {
    record.setStatus(NDEF_URI_TCPOBEX);
    record.setPayload(uri.substring(10).c_str());
    record.setPayloadLength(uri.length() - 10 + statusLength);
  } else if (uri.startsWith("irdaobex://")) {
    record.setStatus(NDEF_URI_IRDAOBEX);
    record.setPayload(uri.substring(11).c_str());
    record.setPayloadLength(uri.length() - 11 + statusLength);
  } else if (uri.startsWith("file://")) {
    record.setStatus(NDEF_URI_FILE);
    record.setPayload(uri.substring(7).c_str());
    record.setPayloadLength(uri.length() - 7 + statusLength);
  } else if (uri.startsWith("urn:epc:id:")) {
    record.setStatus(NDEF_URI_URN_EPC_ID);
    record.setPayload(uri.substring(11).c_str());
    record.setPayloadLength(uri.length() - 11 + statusLength);
  } else if (uri.startsWith("urn:epc:tag:")) {
    record.setStatus(NDEF_URI_URN_EPC_TAG);
    record.setPayload(uri.substring(12).c_str());
    record.setPayloadLength(uri.length() - 12 + statusLength);
  } else if (uri.startsWith("urn:epc:pat:")) {
    record.setStatus(NDEF_URI_URN_EPC_PAT);
    record.setPayload(uri.substring(12).c_str());
    record.setPayloadLength(uri.length() - 12 + statusLength);
  } else if (uri.startsWith("urn:epc:raw:")) {
    record.setStatus(NDEF_URI_URN_EPC_RAW);
    record.setPayload(uri.substring(12).c_str());
    record.setPayloadLength(uri.length() - 12 + statusLength);
  } else if (uri.startsWith("urn:epc:")) {
    record.setStatus(NDEF_URI_URN_EPC);
    record.setPayload(uri.substring(8).c_str());
    record.setPayloadLength(uri.length() - 8 + statusLength);
  } else if (uri.startsWith("urn:nfc:")) {
    record.setStatus(NDEF_URI_URN_NFC);
    record.setPayload(uri.substring(8).c_str());
    record.setPayloadLength(uri.length() - 8 + statusLength);
  } else if (uri.startsWith("urn:")) {
    record.setStatus(NDEF_URI_URN);
    record.setPayload(uri.substring(4).c_str());
    record.setPayloadLength(uri.length() - 4 + statusLength);
  } else {
    record.setStatus(NDEF_URI_NO_PREFIX);
    record.setPayload(uri);
    record.setPayloadLength(uri.length() + statusLength);
  }

  addRecord(record);
}

void NdefMessage::addMimeMediaRecord(String mimeType, const char *payload,
                                     unsigned short payloadLength) {
  NdefRecord record;
  record.setHeaderFlags(NDEF_HEADER_FLAGS_SINGLE_MEDIA_RECORD);
  record.setTypeLength(mimeType.length());
  record.setPayloadLength(payloadLength);
  record.setRecordType(mimeType);
  record.setPayload(payload, payloadLength);

  addRecord(record);
}

void NdefMessage::addWiFiRecord(String ssid, String authenticationType,
                                String encryptionType, String password) {
  NdefRecord record;
  String mimeType = "application/vnd.wfa.wsc";
  unsigned char *payload = new unsigned char[ssid.length() + password.length() +
                                             29]; // TODO: 29 must be calculated

  payload[0] = 0x10;
  payload[1] = 0x0E;
  payload[2] = 0x00;
  payload[3] = 0x36;
  payload[4] = 0x10;
  payload[5] = 0x26;
  payload[6] = 0x00;
  payload[7] = 0x01;
  payload[8] = 0x01;
  payload[9] = 0x10;
  payload[10] = 0x45;
  payload[11] = 0x00;
  payload[12] = ssid.length();
  for (int i = 0; i < ssid.length(); i++) {
    payload[13 + i] = ssid[i];
  }
  payload[13 + ssid.length()] = 0x10;
  payload[14 + ssid.length()] = 0x03;
  payload[15 + ssid.length()] = 0x00;
  payload[16 + ssid.length()] = 0x02;
  payload[17 + ssid.length()] = 0x00;
  payload[18 + ssid.length()] = getWiFiAuthenticationType(authenticationType);
  payload[19 + ssid.length()] = 0x10;
  payload[20 + ssid.length()] = 0x0F;
  payload[21 + ssid.length()] = 0x00;
  payload[22 + ssid.length()] = 0x02;
  payload[23 + ssid.length()] = 0x00;
  payload[24 + ssid.length()] = getWiFiEncryptionType(encryptionType);
  payload[25 + ssid.length()] = 0x10;
  payload[26 + ssid.length()] = 0x27;
  payload[27 + ssid.length()] = 0x00;
  payload[28 + ssid.length()] = password.length();
  for (int i = 0; i < password.length(); i++) {
    payload[29 + ssid.length() + i] = password[i];
  }

  record.setHeaderFlags(NDEF_HEADER_FLAGS_SINGLE_MEDIA_RECORD);
  record.setTypeLength(mimeType.length());
  record.setPayloadLength(ssid.length() + password.length() +
                          29); // TODO: 29 must be calculated
  record.setRecordType(mimeType);
  record.setPayload((const char *)payload, sizeof(payload));

  addRecord(record);
}

uint8_t NdefMessage::getWiFiAuthenticationType(String authenticationType) {
  uint8_t authenticationTypeValue = 0x00;
  authenticationType.trim();
  authenticationType.toUpperCase();

  if (authenticationType == "OPEN") {
    authenticationTypeValue = WIFI_AUTH_OPEN;
  } else if (authenticationType == "WPA PERSONAL") {
    authenticationTypeValue = WIFI_AUTH_WPA_PERSONAL;
  } else if (authenticationType == "SHARED") {
    authenticationTypeValue = WIFI_AUTH_SHARED;
  } else if (authenticationType == "WPA ENTERPRISE") {
    authenticationTypeValue = WIFI_AUTH_WPA_ENTERPRISE;
  } else if (authenticationType == "WPA2 ENTERPRISE") {
    authenticationTypeValue = WIFI_AUTH_WPA2_ENTERPRISE;
  } else if (authenticationType == "WPA2 PERSONAL") {
    authenticationTypeValue = WIFI_AUTH_WPA2_PERSONAL;
  }

  return authenticationTypeValue;
}

uint8_t NdefMessage::getWiFiEncryptionType(String encryptionType) {
  uint8_t encryptionTypeValue = 0x00;
  encryptionType.trim();
  encryptionType.toUpperCase();

  if (encryptionType == "NONE") {
    encryptionTypeValue = WIFI_ENCRYPT_NONE;
  } else if (encryptionType == "WEP") {
    encryptionTypeValue = WIFI_ENCRYPT_WEP;
  } else if (encryptionType == "TKIP") {
    encryptionTypeValue = WIFI_ENCRYPT_TKIP;
  } else if (encryptionType == "AES") {
    encryptionTypeValue = WIFI_ENCRYPT_AES;
  }

  return encryptionTypeValue;
}
