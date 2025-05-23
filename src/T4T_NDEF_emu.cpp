/*
 *         Copyright (c), NXP Semiconductors Caen / France
 *
 *                     (C)NXP Semiconductors
 *       All rights are reserved. Reproduction in whole or in part is
 *      prohibited without the written consent of the copyright owner.
 *  NXP reserves the right to make changes without notice at any time.
 * NXP makes no warranty, expressed, implied or statutory, including but
 * not limited to any implied warranty of merchantability or fitness for any
 *particular purpose, or that the use will not infringe any third party patent,
 * copyright or trademark. NXP must not be liable for any loss or damage
 *                          arising from its use.
 */

// #ifdef CARDEMU_SUPPORT
// #ifndef NO_NDEF_SUPPORT
#include "T4T_NDEF_emu.h"

#include "tool.h"

const unsigned char T4T_NDEF_EMU_APP_Select[] = {0x00, 0xA4, 0x04, 0x00, 0x07,
                                                 0xD2, 0x76, 0x00, 0x00, 0x85,
                                                 0x01, 0x01, 0x00};
const unsigned char T4T_NDEF_EMU_CC[] = {0x00, 0x0F, 0x20, 0x00, 0xFF,
                                         0x00, 0xFF, 0x04, 0x06, 0xE1,
                                         0x04, 0x00, 0xFF, 0x00, 0x00};
const unsigned char T4T_NDEF_EMU_CC_Select[] = {0x00, 0xA4, 0x00, 0x0C,
                                                0x02, 0xE1, 0x03};
const unsigned char T4T_NDEF_EMU_NDEF_Select[] = {0x00, 0xA4, 0x00, 0x0C,
                                                  0x02, 0xE1, 0x04};
const unsigned char T4T_NDEF_EMU_Read[] = {0x00, 0xB0};
const unsigned char T4T_NDEF_EMU_Write[] = {0x00, 0xD6};
const unsigned char T4T_NDEF_EMU_OK[] = {0x90, 0x00};
const unsigned char T4T_NDEF_EMU_NOK[] = {0x6A, 0x82};

unsigned char *pT4T_NdefMessage;
unsigned short T4T_NdefMessage_length = 0;

unsigned char T4T_NdefMessageWritten[256];

typedef enum {
  Ready,
  NDEF_Application_Selected,
  CC_Selected,
  NDEF_Selected,
  DESFire_prod
} T4T_NDEF_EMU_state_t;

typedef void T4T_NDEF_EMU_Callback_t(unsigned char *, unsigned short);

static T4T_NDEF_EMU_state_t eT4T_NDEF_EMU_State = Ready;

static T4T_NDEF_EMU_Callback_t *pT4T_NDEF_EMU_PushCb = NULL;
CustomCallback_t *ndefSendCallback;

static void T4T_NDEF_EMU_FillRsp(unsigned char *pRsp, unsigned short offset,
                                 unsigned char length) {
  if (offset == 0) {
    pRsp[0] = (T4T_NdefMessage_length & 0xFF00) >> 8;
    pRsp[1] = (T4T_NdefMessage_length & 0x00FF);
    if (length > 2)
      memcpy(&pRsp[2], &pT4T_NdefMessage[0], length - 2);
  } else if (offset == 1) {
    pRsp[0] = (T4T_NdefMessage_length & 0x00FF);
    if (length > 1)
      memcpy(&pRsp[1], &pT4T_NdefMessage[0], length - 1);
  } else {
    memcpy(pRsp, &pT4T_NdefMessage[offset - 2], length);
  }

  /* Did we reached the end of NDEF message ?*/
  if ((offset + length) >= (T4T_NdefMessage_length + 2)) {
    /* Notify application of the NDEF send */
    if (pT4T_NDEF_EMU_PushCb != NULL)
      pT4T_NDEF_EMU_PushCb(pT4T_NdefMessage, T4T_NdefMessage_length);

    // Notify custom callback
    if (ndefSendCallback != NULL)
      ndefSendCallback();
  }
}

bool T4T_NDEF_EMU_SetMessage(unsigned char *pMessage,
                             unsigned short messageLength, void *pCb) {
  pT4T_NdefMessage = pMessage;
  T4T_NdefMessage_length = messageLength;
  pT4T_NDEF_EMU_PushCb = (T4T_NDEF_EMU_Callback_t *)pCb;

  return true;
}

void T4T_NDEF_EMU_SetMsg(const char *pMessage, unsigned short messageLength) {
  pT4T_NdefMessage = (unsigned char *)pMessage;
  T4T_NdefMessage_length = messageLength;
}

void T4T_NDEF_EMU_SetCallback(CustomCallback_t function) {
  ndefSendCallback = function;
}

void T4T_NDEF_EMU_Reset(void) { eT4T_NDEF_EMU_State = Ready; }

void T4T_NDEF_EMU_Next(unsigned char *pCmd, unsigned short Cmd_size,
                       unsigned char *pRsp, unsigned short *pRsp_size) {
  bool eStatus = false;

  if (!memcmp(pCmd, T4T_NDEF_EMU_APP_Select, sizeof(T4T_NDEF_EMU_APP_Select))) {
    *pRsp_size = 0;
    eStatus = true;
    eT4T_NDEF_EMU_State = NDEF_Application_Selected;
  } else if (!memcmp(pCmd, T4T_NDEF_EMU_CC_Select,
                     sizeof(T4T_NDEF_EMU_CC_Select))) {
    if (eT4T_NDEF_EMU_State == NDEF_Application_Selected) {
      *pRsp_size = 0;
      eStatus = true;
      eT4T_NDEF_EMU_State = CC_Selected;
    }
  } else if (!memcmp(pCmd, T4T_NDEF_EMU_NDEF_Select,
                     sizeof(T4T_NDEF_EMU_NDEF_Select))) {
    *pRsp_size = 0;
    eStatus = true;
    eT4T_NDEF_EMU_State = NDEF_Selected;
  } else if (!memcmp(pCmd, T4T_NDEF_EMU_Read, sizeof(T4T_NDEF_EMU_Read))) {
    if (eT4T_NDEF_EMU_State == CC_Selected) {
      unsigned short offset = (pCmd[2] << 8) + pCmd[3];
      unsigned char length = pCmd[4];

      if (length <= (sizeof(T4T_NDEF_EMU_CC) + offset + 2)) {
        memcpy(pRsp, &T4T_NDEF_EMU_CC[offset], length);
        *pRsp_size = length;
        eStatus = true;
      }
    } else if (eT4T_NDEF_EMU_State == NDEF_Selected) {
      unsigned short offset = (pCmd[2] << 8) + pCmd[3];
      unsigned char length = pCmd[4];

      if (length <= (T4T_NdefMessage_length + offset + 2)) {
        T4T_NDEF_EMU_FillRsp(pRsp, offset, length);
        *pRsp_size = length;
        eStatus = true;
      }
    }
  } else if (!memcmp(pCmd, T4T_NDEF_EMU_Write, sizeof(T4T_NDEF_EMU_Write))) {
    if (eT4T_NDEF_EMU_State == NDEF_Selected) {
      unsigned short offset = (pCmd[2] << 8) + pCmd[3];
      unsigned char length = pCmd[4];
      if (offset + length <= sizeof(T4T_NdefMessageWritten)) {
        memcpy(&T4T_NdefMessageWritten[offset - 2], &pCmd[5], length);
        pT4T_NdefMessage = T4T_NdefMessageWritten;
        T4T_NdefMessage_length = (pCmd[5] << 8) + pCmd[6];
        *pRsp_size = 0;
        eStatus = true;
      }
    }
  }

  if (eStatus == true) {
    memcpy(&pRsp[*pRsp_size], T4T_NDEF_EMU_OK, sizeof(T4T_NDEF_EMU_OK));
    *pRsp_size += sizeof(T4T_NDEF_EMU_OK);
  } else {
    memcpy(pRsp, T4T_NDEF_EMU_NOK, sizeof(T4T_NDEF_EMU_NOK));
    *pRsp_size = sizeof(T4T_NDEF_EMU_NOK);
    T4T_NDEF_EMU_Reset();
  }
}
// #endif
// #endif
