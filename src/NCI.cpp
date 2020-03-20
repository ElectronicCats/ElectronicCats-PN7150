// #############################################################################
// ###                                                                       ###
// ### NXP PN7150 Driver                                                     ###
// ###                                                                       ###
// ### https://github.com/Strooom/PN7150                                     ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// ###                                                                       ###
// #############################################################################

#include "NCI.h"
#include "NFCTarget.h"
#define MAX_NCI_FRAME_SIZE    258

//, theTagsStatus(TagsPresentStatus::unknown)

NCI::NCI(PN7150Interface &aHardwareInterface) : theHardwareInterface(aHardwareInterface), theState(NciState::HwResetRfc)
    {
    }

void NCI::initialize()
    {
    //Serial.println("Iniciamos ");
    theHardwareInterface.initialize();
    theState = NciState::HwResetRfc;														// re-initializing the state, so we can re-initialize at anytime
    //theTagsStatus = TagsPresentStatus::unknown;
    //nmbrOfTags = 0;
    }

void NCI::run()
    {
    switch (theState)
        {
        case NciState::HwResetRfc:															// after Hardware reset / powerOn
            {
            //Serial.println("Reset - Ready for command ");
            uint8_t payloadData[] = {ResetKeepConfig};										// CORE_RESET-CMD with Keep Configuration
            sendMessage(MsgTypeCommand, GroupIdCore, CORE_RESET_CMD, payloadData, 1);		//
            setTimeOut(10);																	// we should get a RESPONSE within 10 ms (it typically takes 2.3ms)
            theState = NciState::HwResetWfr;												// move to next state, waiting for the matching Response
            }
        break;

        case NciState::HwResetWfr:
            //Serial.println("Reset - Wait for command ");
            if (theHardwareInterface.hasMessage())
                {
                //Serial.println("Reset - Wait for command:Mensaje ");
                getMessage();
                //break;//////
                bool isOk = (6 == rxMessageLength);												// Does the received Msg have the correct lenght ?
                isOk = isOk && isMessageType(MsgTypeResponse, GroupIdCore, CORE_RESET_RSP);		// Is the received Msg the correct type ?
                isOk = isOk && (STATUS_OK == rxBuffer[3]);										// Is the received Status code Status_OK ?

                if (isOk)																		// if everything is OK...
                    {
                    theState = NciState::SwResetRfc;											// ..move to the next state
                    }
                else																			// if not..
                    {
                    theState = NciState::Error;													// goto error state
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;														// time out waiting for response..
                }
            break;

        case NciState::SwResetRfc:
            {
            //Serial.println("Send Core - ready for command ");
            sendMessage(MsgTypeCommand, GroupIdCore, CORE_INIT_CMD);							// CORE_INIT-CMD
            setTimeOut(10);																		// we should get a RESPONSE within 10 ms, typically it takes 0.5ms
            theState = NciState::SwResetWfr;													// move to next state, waiting for response
            }
        break;

        case NciState::SwResetWfr:
            //Serial.println("Core - wait for command ");
            if (theHardwareInterface.hasMessage())
                {
                //Serial.println("Core - wait for command mensaje!");
                getMessage();
                bool isOk = isMessageType(MsgTypeResponse, GroupIdCore, CORE_INIT_RSP);			// Is the received Msg the correct type ?

                if (isOk)																		// if everything is OK...
                    {
                    theState = NciState::EnableCustomCommandsRfc;								// ...move to the next state
                    }
                else																			// if not..
                    {
                    theState = NciState::Error;													// .. goto error state
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;														// time out waiting for response..
                }
            break;

        case NciState::EnableCustomCommandsRfc:
            //Serial.println("Enable - Ready Custom command ");
            sendMessage(MsgTypeCommand, GroupIdProprietary, NCI_PROPRIETARY_ACT_CMD);			// Send NCI_PROPRIETARY_ACT_CMD to activate extra PN7150-NCI features
            setTimeOut(10);																		// we should get a RESPONSE within 10 ms, typically it takes 0.5ms
            theState = NciState::EnableCustomCommandsWfr;										// move to next state, waiting for response
            break;

        case NciState::EnableCustomCommandsWfr:
            //Serial.println("Enable - Wait Custom command ");
            if (theHardwareInterface.hasMessage())
                {
                //Serial.println("Enable - Wait Custom command Mensaje!");
                getMessage();
                bool isOk = isMessageType(MsgTypeResponse, GroupIdProprietary, NCI_PROPRIETARY_ACT_RSP);			// Is the received Msg the correct type ?
                isOk = isOk && (STATUS_OK == rxBuffer[3]);															// Is the received Status code Status_OK ?

                if (isOk)																		// if everything is OK...
                    {
                    theState = NciState::RfIdleCmd;												// ...move to the next state
                    }
                else																			// if not..
                    {
                    theState = NciState::Error;													// .. goto error state
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;															// time out waiting for response..
                }
            break;

        case NciState::RfIdleCmd:
            // After configuring, we are ready to go into Discovery, but we wait for the readerWriter application to give us this trigger
            break;

        case NciState::RfIdleWfr:

            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                bool isOk = (4 == rxMessageLength);															// Does the received Msg have the correct lenght ?
                isOk = isOk && isMessageType(MsgTypeResponse, GroupIdRfManagement, RF_DISCOVER_RSP);		// Is the received Msg the correct type ?
                isOk = isOk && (STATUS_OK == rxBuffer[3]);													// Is the received Status code Status_OK ?
                if (isOk)																					// if everything is OK...
                    {
                    theState = NciState::RfDiscovery;														// ...move to the next state
                    setTimeOut(500);																		// set a timeout of 1 second. If it times out, it means no cards are present..
                    }
                else																						// if not..
                    {
                    theState = NciState::Error;																// .. goto error state
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;																	// time out waiting for response..
                }
            break;

        case NciState::RfDiscovery:

            // TODO : if we have no NTF here, it means no cards are present and we can delete them from the list...
            // Here we don't check timeouts.. we can wait forever for a TAG/CARD to be presented..
            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                if (isMessageType(MsgTypeNotification, GroupIdRfManagement, RF_INTF_ACTIVATED_NTF))
                    {
                    // When a single tag/card is detected, the PN7150 will immediately activate it and send you this type of notification
                    //saveTag(RF_INTF_ACTIVATED_NTF);														// save properties of this Tag in the Tags array
                    //theTagsStatus = TagsPresentStatus::singleTagPresent;
                    theState = NciState::RfPollActive;													// move to PollActive, and wait there for further commands..
                    }
                else if (isMessageType(MsgTypeNotification, GroupIdRfManagement, RF_DISCOVER_NTF))
                    {
                    // When multiple tags/cards are detected, the PN7150 will notify them all and wait for the DH to select one
                    // The first card will have NotificationType == 2 and move the stateMachine to WaitForAllDiscoveries.
                    // More notifications will come in that state
                    //saveTag(RF_DISCOVER_NTF);															// save properties of this Tag in the Tags array
                    setTimeOut(25);																		// we should get more Notifications ubt set a timeout so we don't wait forever
                    //theTagsStatus = TagsPresentStatus::multipleTagsPresent;
                    theState = NciState::RfWaitForAllDiscoveries;
                    }
                }
            else if (isTimeOut())
                {
                //theTagsStatus = TagsPresentStatus::noTagsPresent;										// this means no card has been detected for xxx millisecond, so we can conclude that no cards are present
                }

            break;

        case NciState::RfWaitForAllDiscoveries:
            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                if (isMessageType(MsgTypeNotification, GroupIdRfManagement, RF_DISCOVER_NTF))
                    {
                    notificationType theNotificationType = (notificationType)rxBuffer[rxBuffer[6] + 7];		// notificationType comes in rxBuffer at the end, = 7 bytes + length of RF Technology Specific parameters which are in rxBuffer[6]
                    switch (theNotificationType)
                        {
                        case notificationType::lastNotification:
                        case notificationType::lastNotificationNfccLimit:
                            //saveTag(RF_DISCOVER_NTF);														// save properties of this Tag in the Tags array
                            theState = NciState::RfWaitForHostSelect;
                            break;

                        case notificationType::moreNotification:
                            setTimeOut(25);																	// we should get more Notifications, so set a timeout so we don't wait forever
                            //saveTag(RF_DISCOVER_NTF);														// save properties of this Tag in the Tags array
                            break;

                        default:
                            break;
                        }
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;				// We need a timeout here, in case the final RF_DISCOVER_NTF with Notification Type == 0 or 1 never comes...
                }
            break;

        case NciState::RfWaitForHostSelect:
            // Multiple cards are present. We could select one and move into RfPollActive
            break;

        case NciState::RfPollActive:
            // A card is present, so we can read/write data to it. We could also receive a notification that the card has been removed..
            break;

        case NciState::RfDeActivate1Wfr:
            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                if (isMessageType(MsgTypeResponse, GroupIdRfManagement, RF_DEACTIVATE_RSP))
                    {
                    theState = NciState::RfIdleCmd;
                    }
                else
                    {
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;				// We need a timeout here, in case the RF_DEACTIVATE_RSP never comes...
                }
            break;

        case NciState::RfDeActivate2Wfr:

            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                if (isMessageType(MsgTypeResponse, GroupIdRfManagement, RF_DEACTIVATE_RSP))
                    {
                    setTimeOut(10);
                    theState = NciState::RfDeActivate2Wfn;
                    }
                else
                    {
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;				// We need a timeout here, in case the RF_DEACTIVATE_RSP never comes...
                }
            break;

        case NciState::RfDeActivate2Wfn:
            if (theHardwareInterface.hasMessage())
                {
                getMessage();
                if (isMessageType(MsgTypeNotification, GroupIdRfManagement, RF_DEACTIVATE_NTF))
                    {
                    theState = NciState::RfIdleCmd;
                    }
                else
                    {
                    }
                }
            else if (isTimeOut())
                {
                theState = NciState::Error;				// We need a timeout here, in case the RF_DEACTIVATE_RSP never comes...
                }
            break;

        case NciState::Error:
            // Something went wrong, and we made an emergency landing by moving to this state...
            // To get out of it, the parent application can call initialize(), or we could decide to do that ourselves, maybe after some time-out
            break;

        default:
            break;
        }
    }

/*
void NCI::activate()
    {
    Serial.println("NCI::activate");
    NciState tmpState = getState();
    if (tmpState == NciState::RfIdleCmd)
        {
        uint8_t payloadData[] = { 4, NFC_A_PASSIVE_POLL_MODE, 0x01, NFC_B_PASSIVE_POLL_MODE, 0x01, NFC_F_PASSIVE_POLL_MODE, 0x01, NFC_15693_PASSIVE_POLL_MODE, 0x01 };
        // TODO : instead of setting a fixed scanning for these 4 types, we should pass the to be scanned for types as parameters from the ReaderWriter... https://github.com/Strooom/PN7150/issues/1
        Serial.print("NCI::activate:Dentro: ");
        for (uint32_t index = 0; index < sizeof(payloadData); index++)        // copy the payload
        {
            Serial.print("0x"); Serial.print(payloadData[index],HEX); Serial.print(" ");
        }
        Serial.println("");
        sendMessage(MsgTypeCommand, GroupIdRfManagement, RF_DISCOVER_CMD, payloadData, 9);		//
        setTimeOut(10);																			// we should get a RESPONSE within 10 ms
        theState = NciState::RfIdleWfr;															// move to next state, waiting for Response
        }
    else
        {
            Serial.println("Error:NCI::activate");
        // Error : we can only activate polling when in Idle...
        }
    }

void NCI::deActivate(NciRfDeAcivationMode theMode)
    {
	nmbrOfTags = 0;
    NciState tmpState = getState();
    switch (tmpState)
        {
        case NciState::RfWaitForHostSelect:
            {
            uint8_t payloadData[] = { (uint8_t)NciRfDeAcivationMode::IdleMode };					// in RfWaitForHostSelect, the deactivation type is ignored by the NFCC
            sendMessage(MsgTypeCommand, GroupIdRfManagement, RF_DEACTIVATE_CMD, payloadData, 1);	//
            setTimeOut(10);																			// we should get a RESPONSE within 10 ms
            theTagsStatus = TagsPresentStatus::unknown;
            theState = NciState::RfDeActivate1Wfr;													// move to next state, waiting for response
            }
        break;

        case NciState::RfPollActive:
            {
            uint8_t payloadData[] = { (uint8_t)theMode };
            sendMessage(MsgTypeCommand, GroupIdRfManagement, RF_DEACTIVATE_CMD, payloadData, 1);	//
            setTimeOut(10);																			// we should get a RESPONSE within 10 ms
            theTagsStatus = TagsPresentStatus::unknown;
            theState = NciState::RfDeActivate2Wfr;													// move to next state, waiting for response
            }
        break;

        default:
            break;
        }
    }
*/
NciState NCI::getState() const {
    return theState;
}

/*TagsPresentStatus NCI::getTagsPresentStatus() const
    {
    return theTagsStatus;
    }

uint8_t NCI::getNmbrOfTags() const
    {
    return nmbrOfTags;
    }

Tag* NCI::getTag(uint8_t index)
    {
    return &theTags[index];
    }
*/
void NCI::sendMessage(uint8_t messageType, uint8_t groupId, uint8_t opcodeId)
    {
    txBuffer[0] = (messageType | groupId) & 0xEF;				// put messageType and groupId in first byte, Packet Boundary Flag is always 0
    txBuffer[1] = opcodeId & 0x3F;								// put opcodeId in second byte, clear Reserved for Future Use (RFU) bits
    txBuffer[2] = 0x00;											// payloadLength goes in third byte
    /*Serial.print("Envio 1: ");
    for (uint32_t index = 0; index < 3; index++)        // copy the payload
    {
        Serial.print("0x"); Serial.print(txBuffer[index],HEX); Serial.print(" ");
    }
    Serial.println("");*/
    (void) theHardwareInterface.write(txBuffer, 3);				// TODO :  could make this more robust by checking the return value and go into error is write did not succees
    }

int NCI::ConfigureMode()
{
    unsigned mode = 0 | NXPNCI_MODE_CARDEMU;
    uint8_t Command[MAX_NCI_FRAME_SIZE];
    uint8_t Answer[MAX_NCI_FRAME_SIZE];
    uint16_t AnswerSize;
    uint8_t Item = 0;
    uint8_t NCIDiscoverMap[] = {0x21, 0x00};

    const uint8_t DM_CARDEMU[] = {0x4, 0x2, 0x2};
    const uint8_t R_CARDEMU[] = {0x1, 0x3, 0x0, 0x1, 0x4};

    uint8_t NCIRouting[] = {0x21, 0x01, 0x07, 0x00, 0x01};
    uint8_t NCISetConfig_NFCA_SELRSP[] = {0x20, 0x02, 0x04, 0x01, 0x32, 0x01, 0x00};

    /* Enable Proprietary interface for T4T card presence check procedure */


    //* Building Discovery Map command 
    Item = 0;
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        //Serial.println("mode & NXPNCI_MODE_CARDEMU");
        memcpy(&Command[4+(3*Item)], DM_CARDEMU, sizeof(DM_CARDEMU));
        Item++;
    }

    if (Item != 0)
    {
        //Serial.println("Discovery Map command");
        memcpy(Command, NCIDiscoverMap, sizeof(NCIDiscoverMap));
        Command[2] = 1 + (Item*3);
        Command[3] = Item;
        (void) theHardwareInterface.write(Command, 3 + Command[2]); 
        setTimeOut(10); 
        while(!theHardwareInterface.hasMessage()){}
        getMessage();
        if ((rxBuffer[0] != 0x41) || (rxBuffer[1] != 0x00) || (rxBuffer[3] != 0x00)){
            Serial.println("Error: StartDiscovery");
            return NXPNCI_ERROR;
        }
    }

    //* Configuring routing 
    Item = 0;
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        //Serial.println("mode & NXPNCI_MODE_CARDEMU");
        memcpy(&Command[5+(5*Item)], R_CARDEMU, sizeof(R_CARDEMU));
        Item++;
    }

    if (Item != 0)
    {
        //Serial.println("Entro a routing");
        memcpy(Command, NCIRouting, sizeof(NCIRouting));
        Command[2] = 2 + (Item*5);
        Command[4] = Item;
        (void) theHardwareInterface.write(Command, 3 + Command[2]); 
        setTimeOut(20); 
        while(!theHardwareInterface.hasMessage()){}
        getMessage();
        if ((rxBuffer[0] != 0x41) || (rxBuffer[1] != 0x01) || (rxBuffer[3] != 0x00)) {
            //Serial.println("Error: routing");
            return NXPNCI_ERROR;
        }
    }
    //* Setting NFCA SEL_RSP 
    if (mode & NXPNCI_MODE_CARDEMU)
    {
        NCISetConfig_NFCA_SELRSP[6] += 0x20;
    }

    if (NCISetConfig_NFCA_SELRSP[6] != 0x00)
    {
        //Serial.println("Entro a SELRSP");
        (void) theHardwareInterface.write(NCISetConfig_NFCA_SELRSP, sizeof(NCISetConfig_NFCA_SELRSP)); 
        setTimeOut(10); 
        while(!theHardwareInterface.hasMessage()){}
        getMessage();
        if ((rxBuffer[0] != 0x40) || (rxBuffer[1] != 0x02) || (rxBuffer[3] != 0x00)){
            //Serial.println("Error: SELRSP");
            return NXPNCI_ERROR;
        }else{
            return NXPNCI_SUCCESS;
        }
    }
    return NXPNCI_ERROR;
}

int NCI::StartDiscovery()
{
    //Serial.println("Entro a StartDiscovery");

    unsigned char DiscoveryTechnologies[] = {
        NFC_A_PASSIVE_LISTEN_MODE | NFC_A_PASSIVE_POLL_MODE
    };
    unsigned char TechTabSize = sizeof(DiscoveryTechnologies);
    uint8_t Answer[MAX_NCI_FRAME_SIZE];
    uint16_t AnswerSize;
    uint8_t i;

    uint8_t NCIStartDiscovery[30];
    uint8_t NCIStartDiscovery_length = 0;

    NCIStartDiscovery[0] = 0x21;
    NCIStartDiscovery[1] = 0x03;
    NCIStartDiscovery[2] = (TechTabSize * 2) + 1;
    NCIStartDiscovery[3] = TechTabSize;
    for (i=0; i<TechTabSize; i++)
    {
        NCIStartDiscovery[(i*2)+4] = DiscoveryTechnologies[i];
        NCIStartDiscovery[(i*2)+5] = 0x01;
    }

    NCIStartDiscovery_length = (TechTabSize * 2) + 4;
    (void) theHardwareInterface.write(NCIStartDiscovery, NCIStartDiscovery_length); 
    setTimeOut(10); 
    while(!theHardwareInterface.hasMessage()){}
    getMessage();

    if ((rxBuffer[0] != 0x41) || (rxBuffer[1] != 0x03) || (rxBuffer[3] != 0x00)){
        //Serial.println("Error: StartDiscovery");
        return NXPNCI_ERROR;
    } else{
        return NXPNCI_SUCCESS;
    }
}


bool NCI::CardModeSend (unsigned char *pData, unsigned char DataSize)
{
    bool status;
    uint8_t Cmd[MAX_NCI_FRAME_SIZE];
    uint8_t Ans[MAX_NCI_FRAME_SIZE];
    uint16_t AnsSize;

    /* Compute and send DATA_PACKET */
    Cmd[0] = 0x00;
    Cmd[1] = 0x00;
    Cmd[2] = DataSize;
    memcpy(&Cmd[3], pData, DataSize);
    (void) theHardwareInterface.write(Cmd, DataSize+3); 
    return status;
}
bool NCI::CardModeReceive (unsigned char *pData, unsigned char *pDataSize)
{
    bool status = NFC_ERROR;
    uint8_t Ans[MAX_NCI_FRAME_SIZE];
    uint16_t AnsSize;
    (void) theHardwareInterface.write(Ans, sizeof(Ans)); 
    setTimeOut(10);
    while(!theHardwareInterface.hasMessage()){}
    getMessage();

    /* Is data packet ? */
    if ((rxBuffer[0] == 0x00) && (rxBuffer[1] == 0x00))
    {
        *pDataSize = rxBuffer[2];
        memcpy(pData, &rxBuffer[3], *pDataSize);
        status = NXPNCI_SUCCESS;
    }else{
        return NFC_ERROR;
    }
    return NFC_SUCCESS;
}

int NCI::PICC_ISO14443_4_scenario ()
{
    unsigned char OK[] = {0x90, 0x00};
    unsigned char Cmd[256];
    unsigned char CmdSize;
    int foundReader = 0;
    //Serial.println("NCI::PICC_ISO14443_4_scenario--");
    while (1)
    {
        if(CardModeReceive(Cmd, &CmdSize) == NFC_SUCCESS)
        {
            if ((CmdSize >= 2) && (Cmd[0] == 0x00))
            {
                switch (Cmd[1])
                {
                    case 0xA4:
                        foundReader = 1;
                        break;

                    /*case 0xB0:
                        break;

                    case 0xD0:
                        break;
                    */
                    default:
                        break;
                }
                if (foundReader)
                    return foundReader;
                CardModeSend(OK, sizeof(OK));
            }
        }
        else
        {
            //return 0;
        }
    }
}

void NCI::sendMessage(uint8_t messageType, uint8_t groupId, uint8_t opcodeId, uint8_t payloadData[], uint8_t payloadLength)
    {
    txBuffer[0] = (messageType | groupId) & 0xEF;					// put messageType and groupId in first byte, Packet Boundary Flag is always 0
    txBuffer[1] = opcodeId & 0x3F;									// put opcodeId in second byte, clear Reserved for Future Use (RFU) bits
    txBuffer[2] = payloadLength;									// payloadLength goes in third byte
    for (uint32_t index = 0; index < payloadLength; index++)		// copy the payload
        {
        txBuffer[index + 3] = payloadData[index];
        }
    /*Serial.print("Envio 2: ");
    for (uint32_t index2 = 0; index2 < payloadLength+3; index2++)        // copy the payload
    {
        Serial.print("0x"); Serial.print(txBuffer[index2],HEX); Serial.print(" ");
    }
    Serial.println("");*/
	(void) theHardwareInterface.write(txBuffer, 3 + payloadLength);	// TODO :  could make this more robust by checking the return value and go into error is write did not succees
    }

void NCI::getMessage()
    {
    rxMessageLength = theHardwareInterface.read(rxBuffer);
    }

bool NCI::isMessageType(uint8_t messageType, uint8_t groupId, uint8_t opcodeId) const
    {
    return (((messageType | groupId) & 0xEF) == rxBuffer[0]) && ((opcodeId & 0x3F) == rxBuffer[1]);
    }

bool NCI::isTimeOut() const
    {
    return ((millis() - timeOutStartTime) >= timeOut);
    }

void NCI::setTimeOut(unsigned long theTimeOut)
    {
    timeOutStartTime = millis();
    timeOut = theTimeOut;
    }

/*void NCI::saveTag(uint8_t msgType)
    {
    // Store the properties of detected TAGs in the Tag array.
    // Tag info can come in two different NCI messages : RF_DISCOVER_NTF and RF_INTF_ACTIVATED_NTF and the Tag properties are in slightly different location inside these messages

    if (nmbrOfTags < maxNmbrTags)
        {
        uint8_t offSet;	// Offset in the NCI message where we can find the UniqueID
        switch (msgType)
            {
            case RF_INTF_ACTIVATED_NTF:
                offSet = 12;
                break;

            case RF_DISCOVER_NTF:
                offSet = 9;
                break;

            default:
                return;		// unknown type of msg sent here ?? we just ignore it..
                break;
            }

        uint8_t NfcId1Length = rxBuffer[offSet];
        if (NfcId1Length > 10)
            {
            NfcId1Length = 10;														// limit the length to 10, so in case of whatever error we don't write beyond the boundaries of the array
            }
        uint8_t newTagIndex = nmbrOfTags;											// index to the array item where we will store the info

        theTags[newTagIndex].uniqueIdLength = NfcId1Length;							// copy the length of the unique ID, is 4, 7 or 10
        for (uint8_t index = 0; index < NfcId1Length; index++)						// copy all bytes of the unique ID
            {
            theTags[newTagIndex].uniqueId[index] = rxBuffer[offSet + 1 + index];
            }
        theTags[newTagIndex].detectionTimestamp = millis();

        nmbrOfTags++;																// one more tag in the array now
        }
    }
*/