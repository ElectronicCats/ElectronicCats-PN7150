/**
 * Library to manage the remote device properties, a remote device can be a tag
 * or a reader Authors: Francisco Torres - Electronic Cats - electroniccats.com
 *
 *  August 2023
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#include "RemoteDevice.h"

unsigned char RemoteDevice::getInterface() const {
  return this->remoteDeviceStruct.interface;
}

unsigned char RemoteDevice::getProtocol() const {
  return this->remoteDeviceStruct.protocol;
}

unsigned char RemoteDevice::getModeTech() const {
  return this->remoteDeviceStruct.modeTech;
}

bool RemoteDevice::hasMoreTags() const {
  return this->remoteDeviceStruct.moreTagsAvailable;
}

// Getters for device information

const unsigned char *RemoteDevice::getSensRes() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.sensRes;
    break;

  case (tech.PASSIVE_NFCB):
    return this->remoteDeviceStruct.info.nfcBPP.sensRes;
    break;

  case (tech.PASSIVE_NFCF):
    return this->remoteDeviceStruct.info.nfcFPP.sensRes;
    break;

  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getSensResLen() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.sensResLen;
    break;

  case (tech.PASSIVE_NFCB):
    return this->remoteDeviceStruct.info.nfcBPP.sensResLen;
    break;

  case (tech.PASSIVE_NFCF):
    return this->remoteDeviceStruct.info.nfcFPP.sensResLen;
    break;

  case (tech.PASSIVE_NFCV):
  default:
    return 0;
    break;
  }
}

const unsigned char *RemoteDevice::getNFCID() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.nfcId;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getNFCIDLen() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.nfcIdLen;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return 0;
    break;
  }
}

const unsigned char *RemoteDevice::getSelRes() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.selRes;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getSelResLen() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.selResLen;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return 0;
    break;
  }
}

const unsigned char *RemoteDevice::getRats() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.rats;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getRatsLen() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    return this->remoteDeviceStruct.info.nfcAPP.ratsLen;
    break;

  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return 0;
    break;
  }
}

const unsigned char *RemoteDevice::getAttribRes() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCB):
    return this->remoteDeviceStruct.info.nfcBPP.attribRes;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getAttribResLen() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCB):
    return this->remoteDeviceStruct.info.nfcBPP.attribResLen;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCF):
  case (tech.PASSIVE_NFCV):
  default:
    return 0;
    break;
  }
}

unsigned char RemoteDevice::getBitRate() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCF):
    return this->remoteDeviceStruct.info.nfcFPP.bitRate;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCV):
  default:
    return NULL;
    break;
  }
}

unsigned char RemoteDevice::getAFI() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCV):
    return this->remoteDeviceStruct.info.nfcVPP.afi;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  default:
    return 0;
    break;
  }
}

unsigned char RemoteDevice::getDSFID() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCV):
    return this->remoteDeviceStruct.info.nfcVPP.dsfid;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  default:
    return 0;
    break;
  }
}

const unsigned char *RemoteDevice::getID() const {
  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCV):
    return this->remoteDeviceStruct.info.nfcVPP.id;
    break;

  case (tech.PASSIVE_NFCA):
  case (tech.PASSIVE_NFCB):
  case (tech.PASSIVE_NFCF):
  default:
    return NULL;
    break;
  }
}

void RemoteDevice::setInterface(unsigned char interface) {
  this->remoteDeviceStruct.interface = interface;
}

void RemoteDevice::setProtocol(unsigned char protocol) {
  this->remoteDeviceStruct.protocol = protocol;
}

void RemoteDevice::setModeTech(unsigned char modeTech) {
  this->remoteDeviceStruct.modeTech = modeTech;
}

void RemoteDevice::setMoreTagsAvailable(bool moreTags) {
  this->remoteDeviceStruct.moreTagsAvailable = moreTags;
}

void RemoteDevice::setInfo(RfIntf_t *pRfIntf, uint8_t *pBuf) {
  uint8_t i, temp;

  switch (remoteDeviceStruct.modeTech) {
  case (tech.PASSIVE_NFCA):
    memcpy(pRfIntf->Info.NFC_APP.SensRes, &pBuf[0], 2);
    memcpy(remoteDeviceStruct.info.nfcAPP.sensRes, &pBuf[0], 2);
    remoteDeviceStruct.info.nfcAPP.sensResLen = 2;
    temp = 2;
    pRfIntf->Info.NFC_APP.NfcIdLen = pBuf[temp];
    remoteDeviceStruct.info.nfcAPP.nfcIdLen = pBuf[temp];
    temp++;
    memcpy(pRfIntf->Info.NFC_APP.NfcId, &pBuf[3],
           pRfIntf->Info.NFC_APP.NfcIdLen);
    memcpy(remoteDeviceStruct.info.nfcAPP.nfcId, &pBuf[3],
           remoteDeviceStruct.info.nfcAPP.nfcIdLen);
    temp += pBuf[2];
    pRfIntf->Info.NFC_APP.SelResLen = pBuf[temp];
    remoteDeviceStruct.info.nfcAPP.selResLen = pBuf[temp];
    temp++;

    if (remoteDeviceStruct.info.nfcAPP.selResLen == 1) {
      pRfIntf->Info.NFC_APP.SelRes[0] = pBuf[temp];
      remoteDeviceStruct.info.nfcAPP.selRes[0] = pBuf[temp];
    }

    temp += 4;
    if (pBuf[temp] != 0) {
      temp++;
      pRfIntf->Info.NFC_APP.RatsLen = pBuf[temp];
      remoteDeviceStruct.info.nfcAPP.ratsLen = pBuf[temp];
      memcpy(pRfIntf->Info.NFC_APP.Rats, &pBuf[temp + 1], pBuf[temp]);
      memcpy(remoteDeviceStruct.info.nfcAPP.rats, &pBuf[temp + 1], pBuf[temp]);
    } else {
      pRfIntf->Info.NFC_APP.RatsLen = 0;
      remoteDeviceStruct.info.nfcAPP.ratsLen = 0;
    }
    break;

  case (tech.PASSIVE_NFCB):
    pRfIntf->Info.NFC_BPP.SensResLen = pBuf[0];
    remoteDeviceStruct.info.nfcBPP.sensResLen = pBuf[0];
    memcpy(pRfIntf->Info.NFC_BPP.SensRes, &pBuf[1],
           pRfIntf->Info.NFC_BPP.SensResLen);
    memcpy(remoteDeviceStruct.info.nfcBPP.sensRes, &pBuf[1],
           remoteDeviceStruct.info.nfcBPP.sensResLen);
    temp = pBuf[0] + 4;
    if (pBuf[temp] != 0) {
      temp++;
      pRfIntf->Info.NFC_BPP.AttribResLen = pBuf[temp];
      remoteDeviceStruct.info.nfcBPP.attribResLen = pBuf[temp];
      memcpy(pRfIntf->Info.NFC_BPP.AttribRes, &pBuf[temp + 1], pBuf[temp]);
      memcpy(remoteDeviceStruct.info.nfcBPP.attribRes, &pBuf[temp + 1],
             pBuf[temp]);
    } else {
      pRfIntf->Info.NFC_BPP.AttribResLen = 0;
      remoteDeviceStruct.info.nfcBPP.attribResLen = 0;
    }
    break;

  case (tech.PASSIVE_NFCF):
    pRfIntf->Info.NFC_FPP.BitRate = pBuf[0];
    remoteDeviceStruct.info.nfcFPP.bitRate = pBuf[0];
    pRfIntf->Info.NFC_FPP.SensResLen = pBuf[1];
    remoteDeviceStruct.info.nfcFPP.sensResLen = pBuf[1];
    memcpy(pRfIntf->Info.NFC_FPP.SensRes, &pBuf[2],
           pRfIntf->Info.NFC_FPP.SensResLen);
    memcpy(remoteDeviceStruct.info.nfcFPP.sensRes, &pBuf[2],
           remoteDeviceStruct.info.nfcFPP.sensResLen);
    break;

  case (tech.PASSIVE_NFCV):
    pRfIntf->Info.NFC_VPP.AFI = pBuf[0];
    remoteDeviceStruct.info.nfcVPP.afi = pBuf[0];
    pRfIntf->Info.NFC_VPP.DSFID = pBuf[1];
    remoteDeviceStruct.info.nfcVPP.dsfid = pBuf[1];

    for (i = 0; i < 8; i++) {
      pRfIntf->Info.NFC_VPP.ID[7 - i] = pBuf[2 + i];
      remoteDeviceStruct.info.nfcVPP.id[7 - i] = pBuf[2 + i];
    }

    break;

  default:
    break;
  }
}
