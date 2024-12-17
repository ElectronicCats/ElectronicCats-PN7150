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

// #ifdef RW_SUPPORT
// #ifndef NO_NDEF_SUPPORT
#include "RW_NDEF.h"
#include "tool.h"

/*
    TODO: Only simplified scenario is implemented yet:
     NDEF data stored in contiguous sector and started at sector #1
*/

#define MIFARE_FUNCTION_CLUSTER 0xE1
#define MIFARE_NFC_CLUSTER 0x03
#define MIFARE_NDEF_TLV 0x03

typedef enum {
  Initial,
  Authenticated0,
  Reading_GPB,
  Authenticated,
  Reading_FirstBlk,
  Reading_Data,
  Writing_Data1,
  Writing_Data2,
  Writing_Data
} RW_NDEF_MIFARE_state_t;

typedef struct {
  bool WriteOp;
  unsigned char BlkNb;
  unsigned short MessagePtr;
  unsigned short MessageSize;
  unsigned char *pMessage;
} RW_NDEF_MIFARE_Ndef_t;

static RW_NDEF_MIFARE_state_t eRW_NDEF_MIFARE_State = Initial;
static RW_NDEF_MIFARE_Ndef_t RW_NDEF_MIFARE_Ndef;

void RW_NDEF_MIFARE_Reset(void) {
  eRW_NDEF_MIFARE_State = Initial;
  RW_NDEF_MIFARE_Ndef.pMessage = NdefBuffer;
}

void RW_NDEF_MIFARE_Read_Next(unsigned char *pRsp, unsigned short Rsp_size,
                              unsigned char *pCmd, unsigned short *pCmd_size) {
  /* By default no further command to be sent */
  *pCmd_size = 0;

  switch (eRW_NDEF_MIFARE_State) {
  case Initial:
    /* Authenticating first sector */
    pCmd[0] = 0x40;
    pCmd[1] = 0x00;
    pCmd[2] = 0x00;
    *pCmd_size = 3;
    eRW_NDEF_MIFARE_State = Authenticated0;
    break;

  case Authenticated0:
    if ((Rsp_size == 2) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Read GPB */
      pCmd[0] = 0x10;
      pCmd[1] = 0x30;
      pCmd[2] = 0x01;
      *pCmd_size = 3;
      eRW_NDEF_MIFARE_State = Reading_GPB;
    }
    break;

  case Reading_GPB:
    if ((Rsp_size == 18) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Is NDEF format ?*/
      if ((pRsp[3] == MIFARE_NFC_CLUSTER) &&
          (pRsp[4] == MIFARE_FUNCTION_CLUSTER)) {
        pCmd[0] = 0x40;
        pCmd[1] = 0x01;
        pCmd[2] = 0x01;
        *pCmd_size = 3;
        eRW_NDEF_MIFARE_State = Authenticated;

        RW_NDEF_MIFARE_Ndef.BlkNb = 4;
      }
    }
    break;

  case Authenticated:
    if ((Rsp_size == 2) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* First NDEF data block to read ?*/
      if (RW_NDEF_MIFARE_Ndef.BlkNb == 4) {
        eRW_NDEF_MIFARE_State = Reading_FirstBlk;
      } else {
        RW_NDEF_MIFARE_Ndef.BlkNb++;
        eRW_NDEF_MIFARE_State = Reading_Data;
      }

      /* Read block */
      pCmd[0] = 0x10;
      pCmd[1] = 0x30;
      pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
      *pCmd_size = 3;
    }
    break;

  case Reading_FirstBlk:
    if ((Rsp_size == 18) && (pRsp[Rsp_size - 1] == 0x00)) {
      unsigned char Tmp = 1;
      /* If not NDEF Type skip TLV */
      while (pRsp[Tmp] != MIFARE_NDEF_TLV) {
        Tmp += 2 + pRsp[Tmp + 1];
        if (Tmp > Rsp_size)
          return;
      }

      if (pRsp[Tmp + 1] == 0xFF) {
        RW_NDEF_MIFARE_Ndef.MessageSize = (pRsp[Tmp + 2] << 8) + pRsp[Tmp + 3];
        Tmp += 2;
      } else
        RW_NDEF_MIFARE_Ndef.MessageSize = pRsp[Tmp + 1];

      /* If provisioned buffer is not large enough or message is empty, notify
       * the application and stop reading */
      if ((RW_NDEF_MIFARE_Ndef.MessageSize > RW_MAX_NDEF_FILE_SIZE) ||
          (RW_NDEF_MIFARE_Ndef.MessageSize == 0)) {
        if (pRW_NDEF_PullCb != NULL)
          pRW_NDEF_PullCb(NULL, 0);

        // Run custom callbacks
        if (updateNdefMessageCallback != NULL) {
          updateNdefMessageCallback(NULL, 0);
        }

        if (ndefReceivedCallback != NULL) {
          ndefReceivedCallback();
        }

        break;
      }

      /* Is NDEF read already completed ? */
      if (RW_NDEF_MIFARE_Ndef.MessageSize <= ((Rsp_size - 1) - Tmp - 2)) {
        memcpy(RW_NDEF_MIFARE_Ndef.pMessage, &pRsp[Tmp + 2],
               RW_NDEF_MIFARE_Ndef.MessageSize);

        /* Notify application of the NDEF reception */
        if (pRW_NDEF_PullCb != NULL) {
          pRW_NDEF_PullCb(RW_NDEF_MIFARE_Ndef.pMessage,
                          RW_NDEF_MIFARE_Ndef.MessageSize);
        }

        // Run custom callbacks
        if (updateNdefMessageCallback != NULL) {
          updateNdefMessageCallback(RW_NDEF_MIFARE_Ndef.pMessage,
                                    RW_NDEF_MIFARE_Ndef.MessageSize);
        }

        if (ndefReceivedCallback != NULL) {
          ndefReceivedCallback();
        }
      } else {
        RW_NDEF_MIFARE_Ndef.MessagePtr = (Rsp_size - 1) - Tmp - 2;
        memcpy(RW_NDEF_MIFARE_Ndef.pMessage, &pRsp[Tmp + 2],
               RW_NDEF_MIFARE_Ndef.MessagePtr);
        RW_NDEF_MIFARE_Ndef.BlkNb++;

        /* Read next block */
        pCmd[0] = 0x10;
        pCmd[1] = 0x30;
        pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
        *pCmd_size = 3;
        eRW_NDEF_MIFARE_State = Reading_Data;
      }
    }
    break;

  case Reading_Data:
    if ((Rsp_size == 18) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Is NDEF read already completed ? */
      if ((RW_NDEF_MIFARE_Ndef.MessageSize - RW_NDEF_MIFARE_Ndef.MessagePtr) <
          16) {
        memcpy(&RW_NDEF_MIFARE_Ndef.pMessage[RW_NDEF_MIFARE_Ndef.MessagePtr],
               pRsp + 1,
               RW_NDEF_MIFARE_Ndef.MessageSize -
                   RW_NDEF_MIFARE_Ndef.MessagePtr);

        /* Notify application of the NDEF reception */
        if (pRW_NDEF_PullCb != NULL) {
          pRW_NDEF_PullCb(RW_NDEF_MIFARE_Ndef.pMessage,
                          RW_NDEF_MIFARE_Ndef.MessageSize);
        }

        // Run custom callbacks
        if (updateNdefMessageCallback != NULL) {
          updateNdefMessageCallback(RW_NDEF_MIFARE_Ndef.pMessage,
                                    RW_NDEF_MIFARE_Ndef.MessageSize);
        }

        if (ndefReceivedCallback != NULL) {
          ndefReceivedCallback();
        }
      } else {
        memcpy(&RW_NDEF_MIFARE_Ndef.pMessage[RW_NDEF_MIFARE_Ndef.MessagePtr],
               pRsp + 1, 16);
        RW_NDEF_MIFARE_Ndef.MessagePtr += 16;
        RW_NDEF_MIFARE_Ndef.BlkNb++;

        /* Is Blk on next sector ?*/
        if (((RW_NDEF_MIFARE_Ndef.BlkNb + 1) % 4) == 0) {
          /* Authenticate next block */
          pCmd[0] = 0x40;
          pCmd[1] = (RW_NDEF_MIFARE_Ndef.BlkNb + 1) / 4;
          pCmd[2] = 0x01;
          *pCmd_size = 3;
          eRW_NDEF_MIFARE_State = Authenticated;
        } else {
          /* Read next block */
          pCmd[0] = 0x10;
          pCmd[1] = 0x30;
          pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
          *pCmd_size = 3;
        }
      }
    }
    break;

  default:
    break;
  }
}

void RW_NDEF_MIFARE_Write_Next(unsigned char *pRsp, unsigned short Rsp_size,
                               unsigned char *pCmd, unsigned short *pCmd_size) {
  /* By default no further command to be sent */
  *pCmd_size = 0;

  switch (eRW_NDEF_MIFARE_State) {
  case Initial:
    /* Authenticating first sector */
    pCmd[0] = 0x40;
    pCmd[1] = 0x00;
    pCmd[2] = 0x00;
    *pCmd_size = 3;
    eRW_NDEF_MIFARE_State = Authenticated0;
    break;

  case Authenticated0:
    if ((Rsp_size == 2) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Read MAD */
      pCmd[0] = 0x10;
      pCmd[1] = 0x30;
      pCmd[2] = 0x01;
      *pCmd_size = 3;
      eRW_NDEF_MIFARE_State = Reading_GPB;
    }
    break;

  case Reading_GPB:
    if ((Rsp_size == 18) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Is NDEF format ?*/
      if ((pRsp[3] == MIFARE_NFC_CLUSTER) &&
          (pRsp[4] == MIFARE_FUNCTION_CLUSTER)) {
        pCmd[0] = 0x40;
        pCmd[1] = 0x01;
        pCmd[2] = 0x01;
        *pCmd_size = 3;
        eRW_NDEF_MIFARE_State = Writing_Data1;
        RW_NDEF_MIFARE_Ndef.MessagePtr = 0;
        RW_NDEF_MIFARE_Ndef.BlkNb = 4;
      }
    }
    break;

  case Writing_Data1:
    if ((pRsp[Rsp_size - 1] == 0x00)) {
      /* Is NDEF write already completed ? */
      if (RW_NdefMessage_size <= RW_NDEF_MIFARE_Ndef.MessagePtr) {
        /* Notify application of the NDEF send completion */
        if (pRW_NDEF_PushCb != NULL)
          pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
      } else if (((RW_NDEF_MIFARE_Ndef.BlkNb + 1) % 4) == 0) {
        /* Authenticate next block */
        pCmd[0] = 0x40;
        pCmd[1] = (RW_NDEF_MIFARE_Ndef.BlkNb + 1) / 4;
        pCmd[2] = 0x01;
        *pCmd_size = 3;
        eRW_NDEF_MIFARE_State = Writing_Data1;
        RW_NDEF_MIFARE_Ndef.BlkNb++;
      } else {
        pCmd[0] = 0x10;
        pCmd[1] = 0xA0;
        pCmd[2] = RW_NDEF_MIFARE_Ndef.BlkNb;
        *pCmd_size = 3;
        eRW_NDEF_MIFARE_State = Writing_Data2;
      }
    }
    break;

  case Writing_Data2:
    if ((Rsp_size == 3) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* First block to write ? */
      if (RW_NDEF_MIFARE_Ndef.BlkNb == 4) {
        pCmd[0] = 0x10;
        pCmd[1] = 0x00;
        pCmd[2] = 0x00;
        pCmd[3] = 0x03;
        if (RW_NdefMessage_size > 0xFF) {
          pCmd[4] = 0xFF;
          pCmd[5] = (RW_NdefMessage_size & 0xFF00) >> 8;
          pCmd[6] = RW_NdefMessage_size & 0xFF;
          memcpy(&pCmd[7], pRW_NdefMessage, 10);
          RW_NDEF_MIFARE_Ndef.MessagePtr = 10;
        } else {
          pCmd[4] = (unsigned char)RW_NdefMessage_size;
          memcpy(&pCmd[5], pRW_NdefMessage, 12);
          RW_NDEF_MIFARE_Ndef.MessagePtr = 12;
        }
      } else {
        pCmd[0] = 0x10;
        memcpy(&pCmd[1], pRW_NdefMessage + RW_NDEF_MIFARE_Ndef.MessagePtr, 16);
        RW_NDEF_MIFARE_Ndef.MessagePtr += 16;
      }
      *pCmd_size = 17;
      RW_NDEF_MIFARE_Ndef.BlkNb++;
      eRW_NDEF_MIFARE_State = Writing_Data1;
    }
    break;

  case Writing_Data:
    if ((Rsp_size == 2) && (pRsp[Rsp_size - 1] == 0x00)) {
      /* Is NDEF write already completed ? */
      if (RW_NdefMessage_size <= RW_NDEF_MIFARE_Ndef.MessagePtr) {
        /* Notify application of the NDEF reception */
        if (pRW_NDEF_PushCb != NULL)
          pRW_NDEF_PushCb(pRW_NdefMessage, RW_NdefMessage_size);
      } else {
        /* Write NDEF content */
        pCmd[0] = 0xA2;
        pCmd[1] = RW_NDEF_MIFARE_Ndef.BlkNb;
        memcpy(&pCmd[2], pRW_NdefMessage + RW_NDEF_MIFARE_Ndef.MessagePtr, 4);
        *pCmd_size = 6;

        RW_NDEF_MIFARE_Ndef.MessagePtr += 4;
        RW_NDEF_MIFARE_Ndef.BlkNb++;
      }
    }
    break;

  default:
    break;
  }
}
// #endif
// #endif
