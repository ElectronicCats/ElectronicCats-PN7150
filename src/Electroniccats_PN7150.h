/**
 * NXP PN7150 Driver
 * Porting authors:
 *        Salvador Mendoza - @Netxing - salmg.net
 *        Andres Sabas - Electronic Cats - electroniccats.com
 *        Francisco Torres - Electronic Cats - electroniccats.com
 *
 *  August 2023
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 *
 * Some methods and ideas were extracted from https://github.com/Strooom/PN7150
 */

#ifndef Electroniccats_PN7150_H
#define Electroniccats_PN7150_H

#include <Arduino.h> // Gives us access to all typical Arduino types and functions
// The HW interface between The PN7150 and the DeviceHost is I2C, so we need the
// I2C library.library
#include "Mode.h"
#include "NdefMessage.h"
#include "NdefRecord.h"
#include "P2P_NDEF.h"
#include "RemoteDevice.h"
#include "T4T_NDEF_emu.h"

#if defined(TEENSYDUINO) &&                                                    \
    defined(KINETISK) // Teensy 3.0, 3.1, 3.2, 3.5, 3.6 :  Special, more
                      // optimized I2C library for Teensy boards
#include <i2c_t3.h> // Credits Brian "nox771" : see https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3
#else
#include <Wire.h>
#endif

#define NO_PN7150_RESET_PIN 255
/* Following definitions specifies which settings will apply when
 * NxpNci_ConfigureSettings() API is called from the application
 */
#define NXP_CORE_CONF 1
#define NXP_CORE_STANDBY 1
#define NXP_CORE_CONF_EXTN 1
#define NXP_CLK_CONF 1  // 1=Xtal, 2=PLL
#define NXP_TVDD_CONF 2 // 1=CFG1, 2=CFG2
#define NXP_RF_CONF 1

#define NFC_FACTORY_TEST 1

#define NFC_SUCCESS 0
#define NFC_ERROR 1
#define TIMEOUT_2S 2000
#define SUCCESS NFC_SUCCESS
#define ERROR NFC_ERROR
#define MAX_NCI_FRAME_SIZE 258

/*
 * Flag definition used for NFC library configuration
 */
#define MODE_CARDEMU (1 << 0)
#define MODE_P2P (1 << 1)
#define MODE_RW (1 << 2)

#define MaxPayloadSize 255 // See NCI specification V1.0, section 3.1
#define MsgHeaderSize 3

enum ChipModel { PN7150 = 0, PN7160 = 1 };

/***** Factory Test dedicated APIs
 * *********************************************/
#ifdef NFC_FACTORY_TEST

/*
 * Definition of technology types
 */
typedef enum { NFC_A, NFC_B, NFC_F } NxpNci_TechType_t;

/*
 * Definition of bitrate
 */
typedef enum { BR_106, BR_212, BR_424, BR_848 } NxpNci_Bitrate_t;
#endif

/*
 * Definition of operations handled when processing Reader mode
 */
typedef enum {
#ifndef NO_NDEF_SUPPORT
  READ_NDEF,
  WRITE_NDEF,
#endif
  PRESENCE_CHECK
} RW_Operation_t;

class Electroniccats_PN7150 : public Mode {
private:
  bool _hasBeenInitialized;
  uint8_t _IRQpin, _VENpin, _I2Caddress;
  ChipModel _chipModel;
  TwoWire *_wire;
  RfIntf_t dummyRfInterface;
  uint8_t rxBuffer[MaxPayloadSize +
                   MsgHeaderSize]; // buffer where we store bytes received until
                                   // they form a complete message
  unsigned long timeOut;
  unsigned long timeOutStartTime;
  uint32_t
      rxMessageLength; // length of the last message received. As these are not
                       // 0x00 terminated, we need to remember the length
  uint8_t gNfcController_generation = 0;
  uint8_t gNfcController_fw_version[3] = {0};
  void
  setTimeOut(unsigned long); // set a timeOut for an expected next event, eg
                             // reception of Response after sending a Command
  bool isTimeOut() const;
  uint8_t wakeupNCI();
  bool
  getMessage(uint16_t timeout =
                 5); // 5 miliseconds as default to wait for interrupt responses

public:
  Electroniccats_PN7150(uint8_t IRQpin, uint8_t VENpin, uint8_t I2Caddress,
                        ChipModel chipModel = PN7150, TwoWire *wire = &Wire);
  uint8_t begin(void);
  RemoteDevice remoteDevice;
  Protocol protocol;
  Tech tech;
  ModeTech modeTech;
  Interface interface;
  bool hasMessage() const;
  uint8_t
  writeData(uint8_t data[],
            uint32_t dataLength) const; // write data from DeviceHost to PN7150.
                                        // Returns success (0) or Fail (> 0)
  uint32_t readData(uint8_t data[])
      const; // read data from PN7150, returns the amount of bytes read
  int getFirmwareVersion();
  int GetFwVersion(); // Deprecated, use getFirmwareVersion() instead
  uint8_t connectNCI();
  uint8_t
  ConfigMode(uint8_t modeSE); // Deprecated, use configMode(void) instead
  uint8_t configMode(void);
  bool setReaderWriterMode();
  bool setEmulationMode();
  bool setP2PMode();
  bool configureSettings(void);
  bool
  ConfigureSettings(void); // Deprecated, use configureSettings(void) instead
  bool configureSettings(uint8_t *nfcuid, uint8_t uidlen);
  bool ConfigureSettings(
      uint8_t *nfcuid,
      uint8_t uidlen); // Deprecated, use configureSettings() instead
  uint8_t startDiscovery();
  uint8_t
  StartDiscovery(uint8_t modeSE); // Deprecated, use startDiscovery() instead
  bool stopDiscovery();
  bool StopDiscovery(); // Deprecated, use stopDiscovery() instead
  bool WaitForDiscoveryNotification(
      RfIntf_t *pRfIntf,
      uint16_t tout = 0); // Deprecated, use isTagDetected() instead
  bool isTagDetected(uint16_t tout = 500);
  bool cardModeSend(unsigned char *pData, unsigned char DataSize);
  bool CardModeSend(
      unsigned char *pData,
      unsigned char DataSize); // Deprecated, use cardModeSend() instead
  bool cardModeReceive(unsigned char *pData, unsigned char *pDataSize);
  bool CardModeReceive(
      unsigned char *pData,
      unsigned char *pDataSize); // Deprecated, use cardModeReceive() instead
  void handleCardEmulation();
  void ProcessCardMode(
      RfIntf_t RfIntf); // Deprecated, use handleCardEmulation() instead
  void processReaderMode(
      RfIntf_t RfIntf,
      RW_Operation_t
          Operation); // Deprecated, use waitForTagRemoval(), readNdefMessage()
                      // or writeNdefMessage() and readNdefMessage() instead
  void ProcessReaderMode(
      RfIntf_t RfIntf,
      RW_Operation_t Operation); // Deprecated, use processReaderMode() instead
  void processP2pMode(RfIntf_t RfIntf); // TODO: rename it
  void
  ProcessP2pMode(RfIntf_t RfIntf); // Deprecated, use processP2pMode() instead
  void
  presenceCheck(RfIntf_t RfIntf); // Deprecated, use waitForTagRemoval() instead
  void
  PresenceCheck(RfIntf_t RfIntf); // Deprecated, use waitForTagRemoval() instead
  void waitForTagRemoval();
  bool readerTagCmd(unsigned char *pCommand, unsigned char CommandSize,
                    unsigned char *pAnswer, unsigned char *pAnswerSize);
  bool ReaderTagCmd(
      unsigned char *pCommand, unsigned char CommandSize,
      unsigned char *pAnswer,
      unsigned char *pAnswerSize); // Deprecated, use readerTagCmd() instead
  bool readerReActivate();
  bool ReaderReActivate(
      RfIntf_t *pRfIntf); // Deprecated, use readerReActivate() instead
  bool activateNextTagDiscovery();
  bool ReaderActivateNext(
      RfIntf_t *pRfIntf); // Deprecated, use activateNextTagDiscovery() instead
  void readNdef(RfIntf_t RfIntf); // TODO: remove it
  void readNdefMessage();
  void ReadNdef(RfIntf_t RfIntf);  // Deprecated, use readNdefMessage() instead
  void writeNdef(RfIntf_t RfIntf); // TODO: remove it
  void writeNdefMessage();
  void WriteNdef(RfIntf_t RfIntf); // Deprecated, use writeNdefMessage() instead
  bool nciFactoryTestPrbs(NxpNci_TechType_t type, NxpNci_Bitrate_t bitrate);
  bool NxpNci_FactoryTest_Prbs(
      NxpNci_TechType_t type,
      NxpNci_Bitrate_t bitrate); // Deprecated, use nciFactoryTestPrbs() instead
  bool nciFactoryTestRfOn();
  bool
  NxpNci_FactoryTest_RfOn(); // Deprecated, use nciFactoryTestRfOn() instead
  bool reset();
  void setReadMsgCallback(CustomCallback_t function);
  void setSendMsgCallback(CustomCallback_t function);
  bool isReaderDetected();
  void closeCommunication();
  void sendMessage();
};

#endif
