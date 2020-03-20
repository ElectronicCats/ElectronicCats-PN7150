#ifndef NCI_h													// Header Guard
#define NCI_h

// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// ### Credits : Thomas Buhot, for his PN7120 library, the predecessor of    ###
// ### the PN7150. Due to the NFC Specs not being available for free, I used ###
// ### his library to reverse-engineer some of the missing information links ###
// ###                                                                       ###
// #############################################################################
//
// Summary :
//   This library implements the NFC Controller Interface (NCI), which is a communication protocol between NFC hardware and a controlling Device Host
//   It was written as part of a NXP PN7150 driver
//
//   Initially I want to simply read NFC cards Unique IDs, using the PN7150, so the NCI implementation will only support a subset needed for this purpose
//   Upon request, the library may be extended to support more advanced use of the PN7150
//

#include <Arduino.h>											// Gives us access to all typical Arduino types and functions
//#include "Tag.h"												// 
#include "PN7150Interface.h"									// NCI protocol runs over a hardware interface.


// ---------------------------------------------------------------------
// NCI Packet Header Definitions. NCI Specification V1.0 - section 3.4.1
// ---------------------------------------------------------------------
#define NFC_SUCCESS          0
#define NFC_ERROR            1
#define TIMEOUT_2S          2000
#define NXPNCI_SUCCESS      NFC_SUCCESS
#define NXPNCI_ERROR        NFC_ERROR

#define MaxPayloadSize							255				// See NCI specification V1.0, section 3.1
#define MsgHeaderSize							3

#define MsgTypeData								0x00
#define MsgTypeCommand							0x20
#define MsgTypeResponse							0x40
#define MsgTypeNotification						0x60

// Packet Boundary Flag
#define PacketBoundaryFlagLastSegment			0x00
#define PacketBoundaryFlagNotLastSegment		0x10


// ---------------------------------------------------------------
// NCI GID and OID Definitions. NCI Specification V1.0 - Table 102
// ---------------------------------------------------------------
// Group Identifier (GID)
#define GroupIdCore						0x00
#define GroupIdRfManagement				0x01
#define GroupIdNfceeManagement			0x02
#define GroupIdProprietary				0x0F

// OpCode Identifier (OID)
#define CORE_RESET_CMD					0x00
#define CORE_RESET_RSP					0x00
#define CORE_RESET_NTF					0x00

#define CORE_INIT_CMD					0x01
#define CORE_INIT_RSP					0x01

#define CORE_SET_CONFIG_CMD				0x02
#define CORE_SET_CONFIG_RSP				0x02

#define CORE_GET_CONFIG_CMD				0x03
#define CORE_GET_CONFIG_RSP				0x03

#define CORE_CONN_CREATE_CMD			0x04
#define CORE_CONN_CREATE_RSP			0x04

#define CORE_CONN_CLOSE_CMD				0x05
#define CORE_CONN_CLOSE_RSP				0x05

#define CORE_CONN_CREDITS_NTF			0x06
#define CORE_GENERIC_ERROR_NTF			0x07
#define CORE_INTERFACE_ERROR_NTF		0x08

// 1001b -1111b RFU

#define RF_DISCOVER_MAP_CMD				0x00
#define RF_DISCOVER_MAP_RSP				0x00

#define RF_SET_LISTEN_MODE_ROUTING_CMD	0x01
#define RF_SET_LISTEN_MODE_ROUTING_RSP	0x01

#define RF_GET_LISTEN_MODE_ROUTING_CMD	0x02
#define RF_GET_LISTEN_MODE_ROUTING_RSP	0x02
#define RF_GET_LISTEN_MODE_ROUTING_NTF	0x02

#define RF_DISCOVER_CMD					0x03
#define RF_DISCOVER_RSP					0x03
#define RF_DISCOVER_NTF					0x03

#define RF_DISCOVER_SELECT_CMD			0x04
#define RF_DISCOVER_SELECT_RSP			0x04

#define RF_INTF_ACTIVATED_NTF			0x05

#define RF_DEACTIVATE_CMD				0x06
#define RF_DEACTIVATE_RSP				0x06
#define RF_DEACTIVATE_NTF				0x06

#define RF_FIELD_INFO_NTF				0x07

#define RF_T3T_POLLING_CMD				0x08
#define RF_T3T_POLLING_RSP				0x08
#define RF_T3T_POLLING_NTF				0x08
#define RF_NFCEE_ACTION_NTF				0x09
#define RF_NFCEE_DISCOVERY_REQ_NTF		0x0A
#define RF_PARAMETER_UPDATE_CMD			0x0B
#define RF_PARAMETER_UPDATE_RSP			0x0B
// 1100b -  1111b RFU

#define NFCEE_DISCOVER_CMD				0x00
#define NFCEE_DISCOVER_RSP				0x00
#define NFCEE_DISCOVER_NTF				0x00
#define NFCEE_MODE_SET_CMD				0x01
#define NFCEE_MODE_SET_RSP				0x01
// 0010b - 1111b RFU

#define NCI_PROPRIETARY_ACT_CMD			0x02				// See PN7150 Datasheet, section 5.4
#define NCI_PROPRIETARY_ACT_RSP			0x02				// See PN7150 Datasheet, section 5.4, Table 23 and 24


#define ResetKeepConfig					0x00
#define ResetClearConfig				0x01


// ---------------------------------------------------
// NCI Status Codes. NCI Specification V1.0 - Table 94
// ---------------------------------------------------

// Generic Status Codes
#define STATUS_OK							0x00
#define STATUS_REJECTED						0x01
#define STATUS_RF_FRAME_CORRUPTED			0x02
#define STATUS_FAILED						0x03
#define STATUS_NOT_INITIALIZED				0x04
#define STATUS_SYNTAX_ERROR					0x05
#define STATUS_SEMANTIC_ERROR				0x06
// 0x07 – 0x08 RFU
#define STATUS_INVALID_PARAM				0x09
//#define STATUS_MESSAGE_SIZE_EXCEEDED		0x0A
// 0x0B - 0x9F RFU

// RF Discovery Specific Status Codes
#define DISCOVERY_ALREADY_STARTED			0xA0
#define DISCOVERY_TARGET_ACTIVATION_FAILED	0xA1
#define DISCOVERY_TEAR_DOWN					0xA2
// 0xA3 - 0xAF RFU

// RF Interface Specific Status Codes
#define RF_TRANSMISSION_ERROR				0xB0
#define RF_PROTOCOL_ERROR					0xB1
#define RF_TIMEOUT_ERROR					0xB2
// 0xB3 - 0xBF RFU

// NFCEE Interface Specific Status Codes
#define NFCEE_INTERFACE_ACTIVATION_FAILED	0xC0
#define NFCEE_TRANSMISSION_ERROR			0xC1
#define NFCEE_PROTOCOL_ERROR				0xC2
#define NFCEE_TIMEOUT_ERROR					0xC3
// 0xC4 - 0xDF RFU

// Proprietary Status Codes : 0xE0 - 0xFF


// ------------------------------------------------------
// NCI RF Technologies. NCI Specification V1.0 - Table 95
// ------------------------------------------------------

#define NFC_RF_TECHNOLOGY_A					0x00
#define NFC_RF_TECHNOLOGY_B					0x01
#define NFC_RF_TECHNOLOGY_F					0x02
#define NFC_RF_TECHNOLOGY_15693				0x03
// 0x04 – 0x7F RFU
// 0x80 - 0xFE For proprietary use
// 0xFF RFU


// -------------------------------------------------------------
// NCI RF Technology and Mode. NCI Specification V1.0 - Table 96
// -------------------------------------------------------------
#define NXPNCI_MODE_CARDEMU    (1<<0)

#define NFC_A_PASSIVE_POLL_MODE					0x00 //EMU
#define NFC_B_PASSIVE_POLL_MODE					0x01
#define NFC_F_PASSIVE_POLL_MODE					0x02
#define NFC_A_ACTIVE_POLL_MODE					0x03
// RFU	0x04
#define NFC_F_ACTIVE_POLL_MODE					0x05
#define NFC_15693_PASSIVE_POLL_MODE				0x06
// 0x07 – 0x6F RFU
// 0x70 – 0x7F Reserved for Proprietary Technologies in Poll Mode

#define NFC_A_PASSIVE_LISTEN_MODE				0x80 //EMU

#define NFC_B_PASSIVE_LISTEN_MODE				0x81
#define NFC_F_PASSIVE_LISTEN_MODE				0x82
#define NFC_A_ACTIVE_LISTEN_MODE				0x83
// RFU	0x84
#define NFC_F_ACTIVE_LISTEN_MODE				0x85
#define NFC_15693_PASSIVE_LISTEN_MODE			0x86
// 0x87 – 0xEF RFU
// 0xF0 – 0xFF Reserved for Proprietary Technologies in Listen Mode


// ---------------------------------------------------
// NCI RF Protocols. NCI Specification V1.0 - Table 98
// ---------------------------------------------------

#define PROTOCOL_UNDETERMINED	0x00
#define PROTOCOL_T1T			0x01
#define PROTOCOL_T2T			0x02
#define PROTOCOL_T3T			0x03
#define PROTOCOL_ISO_DEP		0x04
#define PROTOCOL_NFC_DEP		0x05
// 0x06 – 0x7F RFU
// 0x80-0xFE For proprietary use
// 0xFF RFU


// -----------------------------------------------------
// NCI RF Interfacess. NCI Specification V1.0 - Table 99
// -----------------------------------------------------

#define NFCEE_Direct_RF_Interface	0x00
#define Frame_RF_interface			0x01
#define ISO_DEP_RF_interface		0x02
#define NFC_DEP_RF_interface		0x03
// 0x04 – 0x7F RFU
// 0x80 - 0xFE For proprietary use
// 0xFF RFU


// ---------------------------------------------------------------
// NFCEE Protocol / Interfaces. NCI Specification V1.0 - Table 100
// ---------------------------------------------------------------

#define APDU	0x00
#define HCI Access	0x01
#define Type 3 Tag Command Set	0x02
#define Transparent	0x03
// 0x04 - 0x7F RFU
// 0x80 - 0xFE For proprietary use
// 0xFF RFU


// --------------------------------------------
// Bit Rates. NCI Specification V1.0 - Table 97
// --------------------------------------------

#define NFC_BIT_RATE_106	0x00
#define NFC_BIT_RATE_212	0x01
#define NFC_BIT_RATE_424	0x02
#define NFC_BIT_RATE_848	0x03
#define NFC_BIT_RATE_1695	0x04
#define NFC_BIT_RATE_3390	0x05
#define NFC_BIT_RATE_6780	0x06
// 0x07 – 0x7F RFU
// 0x80 - 0xFE For proprietary use
// 0xFF RFU


// ----------------------------------------------------------------------
// Dectivation Types for RF_DEACTIVATE_ NCI Specification V1.0 - Table 63
// ----------------------------------------------------------------------

enum class NciRfDeAcivationMode : uint8_t
    {
    IdleMode =0x00,
    SleepMode = 0x01,
    Sleep_AFMode = 0x02,
    Discovery = 0x03
// 0x04 – 0xFF RFU
    };

// ----------------------------------------------------------------------
// Dectivation Reason RF_DEACTIVATE_NTF NCI Specification V1.0 - Table 64
// ----------------------------------------------------------------------

#define DH_Request			0x00
#define Endpoint_Request	0x01
#define RF_Link_Loss		0x02
#define NFC_B_Bad_AFI		0x03
// 0x04 – 0xFF RFU


/* Discovery Types/Detected Technology and Mode */
#define NCI_DISCOVERY_TYPE_POLL_A               0x00
#define NCI_DISCOVERY_TYPE_POLL_B               0x01
#define NCI_DISCOVERY_TYPE_POLL_F               0x02
#define NCI_DISCOVERY_TYPE_POLL_A_ACTIVE        0x03
#define NCI_DISCOVERY_TYPE_POLL_F_ACTIVE        0x05
#define NCI_DISCOVERY_TYPE_POLL_ISO15693        0x06

enum class notificationType : uint8_t
    {
    lastNotification = 0x00,
    lastNotificationNfccLimit = 0x01,
    moreNotification = 0x02
// 	3 - 255 RFU
    };


// ------------------------------------------------------------------------------------------
// NFC Defines 3 types of applications. The NP7150 can operate them all in parallel if needed
// ------------------------------------------------------------------------------------------

//enum class NciApplicationMode : uint8_t
//    {
//    None = 0x00,
//    CardReadWrite = 0x01,			// Using the PN7150 to simply read NFC or RFID cards
//    CardEmulate = 0x02,			// For Future Use, not yet implemented
//    PeerToPeer = 0x04				// For Future Use, not yet implemented
//    };

//enum class CardType : uint8_t
//    {
//    T1T,
//    T2T,
//    T3T,
//    ISO_DEP,
//    NFC_DEP
//    };


// ------------------------------------------------------------------------------------------
// NFC Requires the device to behave according to a certain State Machine.
// Furthermore the NCI protocol consists of Commands, to be followed by Responses
// All this behaviour is implemented with a Finite State Machine, with following states
// ------------------------------------------------------------------------------------------

enum class NciState : uint8_t
// Note :	Rfc means "Ready For Command" : a state where you would (typically) send an NCI command from the DH to the NFC
//			Wfr means "Waiting For Response" : a state where you would wait for a response from the NFC to the DH
//			Wfn means "Waiting For Notification" : a state where you would wait for a notification from the NFC to the DH
    {
    HwResetRfc,						// you start in this state after a hardware reset of the PN7150, then you send the CORE_RESET_CMD
    HwResetWfr,						// waiting for CORE_RESET_RSP
    SwResetRfc,						// send CORE_INIT_CMD
    SwResetWfr,						// waiting for CORE_INIT_RSP
    EnableCustomCommandsRfc,		// Enabling PN7150-extensions
    EnableCustomCommandsWfr,		// waiting for response/confirmation
    RfIdleCmd,						// Core initialized, now waiting for RF configuration commands
    RfIdleWfr,
    RfGoToDiscoveryWfr,
    RfDiscovery,					// polling / detecting cards/tags
    RfWaitForAllDiscoveries,		// busy enumerating multiple cards/tags being detected
    RfWaitForHostSelect,			// done detecting multiple cards/tags, waiting for the DH to select one
    RfPollActive,					// detected 1 card/tag, and activated it for reading/writing

    RfDeActivate1Wfr,				// waiting for deactivation response, no notification will come (dactivation in RfWaitForHostSelect)
    RfDeActivate2Wfr,				// waiting for deactivation response, additionally a notification will come (deactivation in RfPollActive)
    RfDeActivate2Wfn,				// waiting for deactivation notifiation
    Error,
    End
    };

enum class NciError : uint8_t
    {
    responseNOK,							// we received a response with somethin wrong in it, eg Status_NOK
    responseTimeout,						// we did not receive a response in time
    none
    };

enum class TagsPresentStatus : uint8_t
    {
    unknown,
    noTagsPresent,
    singleTagPresent,
    multipleTagsPresent
    };


class NCI
    {
    public:
        NCI(PN7150Interface &theHardwareInterface);				// Constructor, with mode default set to CardReadwrite
        void initialize();										// See NCI specification V1.0, section 4.1 & 4.2
        void run();												// runs the NCI stateMachine
        void activate();										// moves the StateMachine from Idle to Discover and starts the polling
        void deActivate(NciRfDeAcivationMode theMode);			// moves the StateMachine from PollActive or WaitingForHostSelect back into Idle
        NciState getState() const;								// find out in which state the NCI stateMachine is
        //TagsPresentStatus getTagsPresentStatus() const;			// read-only get function for the (private) property
        //uint8_t getNmbrOfTags() const;
        //Tag* getTag(uint8_t index);								// TODO : improve this with 'const' so the Tag properties are read-only
        int ConfigureMode();
        int StartDiscovery();
        int PICC_ISO14443_4_scenario();
        bool CardModeSend (unsigned char *pData, unsigned char DataSize);
        bool CardModeReceive (unsigned char *pData, unsigned char *pDataSize);
     
    private:
        PN7150Interface &theHardwareInterface;					// reference to the object handling the hardware interface

        NciState theState;										// keeps track of the state of the NCI stateMachine - FSM
        //TagsPresentStatus theTagsStatus;						// how many Tag/Cards are currently present

        unsigned long timeOut;									// keeps track of time-outs when waiting for responses from the NFC device
        unsigned long timeOutStartTime;							// keeps track of time-outs when waiting for responses from the NFC device

        uint8_t rxBuffer[MaxPayloadSize + MsgHeaderSize];		// buffer where we store bytes received until they form a complete message
        uint32_t rxMessageLength;								// length of the last message received. As these are not 0x00 terminated, we need to remember the length
        uint8_t txBuffer[MaxPayloadSize + MsgHeaderSize];		// buffer where we store the message to be transmitted
        void sendMessage(uint8_t messageType, uint8_t groupId, uint8_t opcodeId, uint8_t payloadData[], uint8_t payloadLength);
        void sendMessage(uint8_t messageType, uint8_t groupId, uint8_t opcodeId);														// Variant for msg with no payload
        void getMessage();																												// read message from I2C into rxBuffer
        bool isMessageType(uint8_t messageType, uint8_t groupId, uint8_t opcodeId) const; 												// Is the msg in the rxBuffer of this type ?
        void setTimeOut(unsigned long);																									// set a timeOut for an expected next event, eg reception of Response after sending a Command
        bool isTimeOut() const;																											// Chech if we have exceeded the timeOut

        static constexpr unsigned long scanPeriod = 1000;		// lenght of a scan for tags cycle, in milliseconds. Note : setting this to very short times, eg. < 100 ms will not work, because the NFC discovery loop has a certain minumum constrained by the HW protocols
        static constexpr uint8_t maxNmbrTags = 3;				// maximum number of (simultaneously present) tags we can keep track of. PN7150 is limited to 3
        //Tag theTags[maxNmbrTags];								// array to store the data of a number of currently present tags. When uniqueIdLenght == 0 it means invalid data in this position of the array
        //uint8_t nmbrOfTags = 0;									// how many tags are actually in the array
        //void saveTag(uint8_t msgType);
    };


#endif															// End Header Guard

