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

#ifndef NdefMessage_H
#define NdefMessage_H

#include <Arduino.h>

#include "NdefRecord.h"
#include "RW_NDEF.h"
#include "T4T_NDEF_emu.h"
#include "ndef_helper.h"

/*
 * MB = Message Begin
 * ME = Message End
 * CF = Chunk Flag
 * SR = Short Record
 * IL = ID Length present
 * TNF = Type Name Format
 */
#define NDEF_HEADER_FLAGS_SINGLE_RECORD                                        \
  0xD1 // 1101 0001 -> MB = 1, ME = 1, CF = 0, SR = 1, IL = 0, TNF = 001
#define NDEF_HEADER_FLAGS_SINGLE_MEDIA_RECORD                                  \
  0xD2 // 1101 0010 -> MB = 1, ME = 1, CF = 0, SR = 1, IL = 0, TNF = 010
#define NDEF_HEADER_FLAGS_FIRST_RECORD                                         \
  0x91 // 1001 0001 -> MB = 1, ME = 0, CF = 0, SR = 1, IL = 0, TNF = 001
#define NDEF_HEADER_FLAGS_FIRST_MEDIA_RECORD                                   \
  0x92 // 1001 0010 -> MB = 1, ME = 0, CF = 0, SR = 1, IL = 0, TNF = 010
#define NDEF_HEADER_FLAGS_NEXT_RECORD                                          \
  0x11 // 0001 0001 -> MB = 0, ME = 0, CF = 0, SR = 1, IL = 0, TNF = 001
#define NDEF_HEADER_FLAGS_NEXT_MEDIA_RECORD                                    \
  0x12 // 0001 0010 -> MB = 0, ME = 0, CF = 0, SR = 1, IL = 0, TNF = 010
#define NDEF_HEADER_FLAGS_LAST_RECORD                                          \
  0x51 // 0101 0001 -> MB = 0, ME = 1, CF = 0, SR = 1, IL = 0, TNF = 001
#define NDEF_HEADER_FLAGS_LAST_MEDIA_RECORD                                    \
  0x52 // 0101 0010 -> MB = 0, ME = 1, CF = 0, SR = 1, IL = 0, TNF = 010
#define NDEF_TYPE_LENGTH 0x01
#define NDEF_TEXT_RECORD_TYPE 'T'
#define NDEF_URI_RECORD_TYPE 'U'
#define NDEF_STATUS 0x02
#define NDEF_DEFAULT_LANGUAGE_CODE "en"

#define NDEF_URI_NO_PREFIX 0x00
#define NDEF_URI_HTTP_WWWDOT 0x01
#define NDEF_URI_HTTPS_WWWDOT 0x02
#define NDEF_URI_HTTP 0x03
#define NDEF_URI_HTTPS 0x04
#define NDEF_URI_TEL 0x05
#define NDEF_URI_MAILTO 0x06
#define NDEF_URI_FTP_ANONIMOUS 0x07
#define NDEF_URI_FTP_FTPDOT 0x08
#define NDEF_URI_FTPS 0x09
#define NDEF_URI_SFTP 0x0A
#define NDEF_URI_SMB 0x0B
#define NDEF_URI_NFS 0x0C
#define NDEF_URI_FTP 0x0D
#define NDEF_URI_DAV 0x0E
#define NDEF_URI_NEWS 0x0F
#define NDEF_URI_TELNET 0x10
#define NDEF_URI_IMAP 0x11
#define NDEF_URI_RTSP 0x12
#define NDEF_URI_URN 0x13
#define NDEF_URI_POP 0x14
#define NDEF_URI_SIP 0x15
#define NDEF_URI_SIPS 0x16
#define NDEF_URI_TFTP 0x17
#define NDEF_URI_BTSPP 0x18
#define NDEF_URI_BTL2CAP 0x19
#define NDEF_URI_BTGOEP 0x1A
#define NDEF_URI_TCPOBEX 0x1B
#define NDEF_URI_IRDAOBEX 0x1C
#define NDEF_URI_FILE 0x1D
#define NDEF_URI_URN_EPC_ID 0x1E
#define NDEF_URI_URN_EPC_TAG 0x1F
#define NDEF_URI_URN_EPC_PAT 0x20
#define NDEF_URI_URN_EPC_RAW 0x21
#define NDEF_URI_URN_EPC 0x22
#define NDEF_URI_URN_NFC 0x23

// WiFi authentication types
#define WIFI_AUTH_OPEN 0x01
#define WIFI_AUTH_WPA_PERSONAL 0x02
#define WIFI_AUTH_SHARED 0x04
#define WIFI_AUTH_WPA_ENTERPRISE 0x08
#define WIFI_AUTH_WPA2_ENTERPRISE 0x10
#define WIFI_AUTH_WPA2_PERSONAL 0x20

// WiFi encryption types
#define WIFI_ENCRYPT_NONE 0x01
#define WIFI_ENCRYPT_WEP 0x02
#define WIFI_ENCRYPT_TKIP 0x04
#define WIFI_ENCRYPT_AES 0x08

class NdefMessage {
private:
  static uint8_t recordCounter;
  static unsigned char *content;
  static unsigned short contentLength;
  static unsigned char *newContent;
  static unsigned short newContentLength;
  static void update(unsigned char *message, unsigned short messageLength);
  void getNextRecord();
  static String getHexRepresentation(const byte *data,
                                     const uint32_t dataLength);
  static String newString;
  void addRecord(NdefRecord record);
  static void updateHeaderFlags();
  static bool isHeaderByte(unsigned char byte);
  uint8_t getWiFiAuthenticationType(String authenticationType);
  uint8_t getWiFiEncryptionType(String encryptionType);

public:
  NdefMessage();
  void begin();
  static unsigned char *getContent();
  static unsigned short getContentLength();
  static void setContent(const char *content, unsigned short contentLength);
  NdefRecord_t getRecord();
  bool isEmpty();
  bool isNotEmpty();
  bool hasRecord();
  void addTextRecord(String text);
  void addTextRecord(String text, String languageCode);
  void addUriRecord(String uri);
  void addMimeMediaRecord(String mimeType, const char *payload,
                          unsigned short payloadLength);
  void addWiFiRecord(String ssid, String authenticationType,
                     String encryptionType, String password);
};

#endif
