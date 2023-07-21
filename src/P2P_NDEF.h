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
#include <Arduino.h>

#define P2P_NDEF_MAX_NDEF_MESSAGE_SIZE 240

typedef void P2P_NDEF_Callback_t(unsigned char *, unsigned short);

void P2P_NDEF_Reset(void);
void P2P_NDEF_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *Rsp, unsigned short *pRsp_size);
void P2P_NDEF_RegisterPullCallback(P2P_NDEF_Callback_t pCb);
bool P2P_NDEF_SetMessage(unsigned char *pMessage, unsigned short Message_size, void *pCb);
//void Sleep (unsigned int ms);
