#ifndef Electroniccats_PN7150_H
#define Electroniccats_PN7150_H
/**
 * NXP PN7150 Driver
 * Porting uthors: 
 *        Salvador Mendoza - @Netxing - salmg.net
 *        Andres Sabas - Electronic Cats - Electroniccats.com
 *
 *  November 2020
 * 
 * This code is beerware; if you see me (or any other collaborator 
 * member) at the local, and you've found our code helpful, 
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 *
 * A few methods and ideas were extract from 
 * https://github.com/Strooom/PN7150
 *
 */

#include <Arduino.h> // Gives us access to all typical Arduino types and functions
                     // The HW interface between The PN7150 and the DeviceHost is I2C, so we need the I2C library.library
//#include "RW_NDEF.h"
#include "P2P_NDEF.h"

#if defined(TEENSYDUINO) && defined(KINETISK) // Teensy 3.0, 3.1, 3.2, 3.5, 3.6 :  Special, more optimized I2C library for Teensy boards
#include <i2c_t3.h>                           // Credits Brian "nox771" : see https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3
#else
#include <Wire.h>
#if defined(__AVR__) || defined(__i386__) || defined(ARDUINO_ARCH_SAMD) || defined(ESP8266) || defined(ARDUINO_ARCH_STM32)
#define WIRE Wire
#else // Arduino Due
#define WIRE Wire1
#endif // TODO :   i2c_t3.h ensures a maximum I2C message of 259, which is sufficient. Other I2C implementations have shorter buffers (32 bytes)
#endif

/* Following definitions specifies which settings will apply when NxpNci_ConfigureSettings()
 * API is called from the application
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

#define IRQ_INTERRUPT 1

/*
 * Flag definition used for NFC library configuration
 */
#define MODE_CARDEMU (1 << 0)
#define MODE_P2P (1 << 1)
#define MODE_RW (1 << 2)

/*
 * Flag definition used as Mode values
 */
#define MODE_POLL 0x00
#define MODE_LISTEN 0x80
#define MODE_MASK 0xF0

/*
 * Flag definition used as Technologies values
 */
#define TECH_PASSIVE_NFCA 0
#define TECH_PASSIVE_NFCB 1
#define TECH_PASSIVE_NFCF 2
#define TECH_ACTIVE_NFCA 3
#define TECH_ACTIVE_NFCF 5
#define TECH_PASSIVE_15693 6

/*
 * Flag definition used as Protocol values
 */
#define PROT_UNDETERMINED 0x0
#define PROT_T1T 0x1
#define PROT_T2T 0x2
#define PROT_T3T 0x3
#define PROT_ISODEP 0x4
#define PROT_NFCDEP 0x5
#define PROT_ISO15693 0x6
#define PROT_MIFARE 0x80

/*
 * Flag definition used as Interface values
 */
#define INTF_UNDETERMINED 0x0
#define INTF_FRAME 0x1
#define INTF_ISODEP 0x2
#define INTF_NFCDEP 0x3
#define INTF_TAGCMD 0x80

#define MaxPayloadSize 255 // See NCI specification V1.0, section 3.1
#define MsgHeaderSize 3

/***** Factory Test dedicated APIs *********************************************/
#ifdef NFC_FACTORY_TEST

/*
 * Definition of technology types
 */
typedef enum
{
    NFC_A,
    NFC_B,
    NFC_F
} NxpNci_TechType_t;

/*
 * Definition of bitrate
 */
typedef enum
{
    BR_106,
    BR_212,
    BR_424,
    BR_848
} NxpNci_Bitrate_t;
#endif
/*
 * Definition of discovered remote device properties information
 */

/* POLL passive type A */
struct RfIntf_info_APP_t
{
    unsigned char SensRes[2];
    unsigned char NfcIdLen;
    unsigned char NfcId[10];
    unsigned char SelResLen;
    unsigned char SelRes[1];
    unsigned char RatsLen;
    unsigned char Rats[20];
};

/* POLL passive type B */
struct RfIntf_info_BPP_t
{
    unsigned char SensResLen;
    unsigned char SensRes[12];
    unsigned char AttribResLen;
    unsigned char AttribRes[17];
};

/* POLL passive type F */
struct RfIntf_info_FPP_t
{
    unsigned char BitRate;
    unsigned char SensResLen;
    unsigned char SensRes[18];
};

/* POLL passive type ISO15693 */
struct RfIntf_info_VPP_t
{
    unsigned char AFI;
    unsigned char DSFID;
    unsigned char ID[8];
};

typedef union
{
    RfIntf_info_APP_t NFC_APP;
    RfIntf_info_BPP_t NFC_BPP;
    RfIntf_info_FPP_t NFC_FPP;
    RfIntf_info_VPP_t NFC_VPP;
} RfIntf_Info_t;

/*
 * Definition of discovered remote device properties
 */
struct RfIntf_t
{
    unsigned char Interface;
    unsigned char Protocol;
    unsigned char ModeTech;
    bool MoreTags;
    RfIntf_Info_t Info;
};

/*
 * Definition of operations handled when processing Reader mode
 */
typedef enum
{
#ifndef NO_NDEF_SUPPORT
    READ_NDEF,
    WRITE_NDEF,
#endif
    PRESENCE_CHECK
} RW_Operation_t;

/*
 * Definition of discovered remote device properties information
 */
/* POLL passive type A */
typedef struct
{
    unsigned char SensRes[2];
    unsigned char NfcIdLen;
    unsigned char NfcId[10];
    unsigned char SelResLen;
    unsigned char SelRes[1];
    unsigned char RatsLen;
    unsigned char Rats[20];
} NxpNci_RfIntf_info_APP_t;

/* POLL passive type B */
typedef struct
{
    unsigned char SensResLen;
    unsigned char SensRes[12];
    unsigned char AttribResLen;
    unsigned char AttribRes[17];
} NxpNci_RfIntf_info_BPP_t;

/* POLL passive type F */
typedef struct
{
    unsigned char BitRate;
    unsigned char SensResLen;
    unsigned char SensRes[18];
} NxpNci_RfIntf_info_FPP_t;

/* POLL passive type ISO15693 */
typedef struct
{
    unsigned char AFI;
    unsigned char DSFID;
    unsigned char ID[8];
} NxpNci_RfIntf_info_VPP_t;

typedef union
{
    NxpNci_RfIntf_info_APP_t NFC_APP;
    NxpNci_RfIntf_info_BPP_t NFC_BPP;
    NxpNci_RfIntf_info_FPP_t NFC_FPP;
    NxpNci_RfIntf_info_VPP_t NFC_VPP;
} NxpNci_RfIntf_Info_t;

class Electroniccats_PN7150
{
private:
    uint8_t _IRQpin, _VENpin, _I2Caddress;
    uint8_t rxBuffer[MaxPayloadSize + MsgHeaderSize]; // buffer where we store bytes received until they form a complete message
    void setTimeOut(unsigned long);                   // set a timeOut for an expected next event, eg reception of Response after sending a Command
    bool isTimeOut() const;
    bool getMessage(uint16_t timeout = 5); // 5 miliseconds as default to wait for interrupt responses
    unsigned long timeOut;
    unsigned long timeOutStartTime;
    uint32_t rxMessageLength; // length of the last message received. As these are not 0x00 terminated, we need to remember the length
    uint8_t gNfcController_generation = 0;
    uint8_t gNfcController_fw_version[3] = {0};
    static void hasMessage();

public:
    Electroniccats_PN7150(uint8_t IRQpin, uint8_t VENpin, uint8_t I2Caddress);
    int GetFwVersion();
    uint8_t begin(void);
    uint8_t writeData(uint8_t data[], uint32_t dataLength) const; // write data from DeviceHost to PN7150. Returns success (0) or Fail (> 0)
    uint32_t readData(uint8_t data[]) const;                      // read data from PN7150, returns the amount of bytes read
    uint8_t ConfigMode(uint8_t modeSE);
    uint8_t StartDiscovery(uint8_t modeSE);
    uint8_t connectNCI();
    uint8_t wakeupNCI();
    bool CardModeSend(unsigned char *pData, unsigned char DataSize);
    bool CardModeReceive(unsigned char *pData, unsigned char *pDataSize);
    bool WaitForDiscoveryNotification(RfIntf_t *pRfIntf, uint8_t tout = 0);
    void FillInterfaceInfo(RfIntf_t *pRfIntf, uint8_t *pBuf);
    bool ReaderTagCmd(unsigned char *pCommand, unsigned char CommandSize, unsigned char *pAnswer, unsigned char *pAnswerSize);
    bool StopDiscovery(void);
    void ProcessReaderMode(RfIntf_t RfIntf, RW_Operation_t Operation);
    void PresenceCheck(RfIntf_t RfIntf);
    bool ReaderReActivate(RfIntf_t *pRfIntf);
    void PrintBuf(const byte *data, const uint32_t numBytes);
    bool ReaderActivateNext(RfIntf_t *pRfIntf);
    bool ConfigureSettings(void);
    void NdefPull_Cb(unsigned char *pNdefMessage, unsigned short NdefMessageSize);
    void NdefPush_Cb(unsigned char *pNdefRecord, unsigned short NdefRecordSize);
    bool NxpNci_FactoryTest_Prbs(NxpNci_TechType_t type, NxpNci_Bitrate_t bitrate);
    bool NxpNci_FactoryTest_RfOn(void);
    void ProcessP2pMode(RfIntf_t RfIntf);
    void ReadNdef(RfIntf_t RfIntf);
    void WriteNdef(RfIntf_t RfIntf);
    void ProcessCardMode(RfIntf_t RfIntf);
};

#endif
