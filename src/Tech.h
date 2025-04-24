/**
 * Library to get the technology of the NFC card
 * Authors:
 *        Francisco Torres - Electronic Cats - electroniccats.com
 *
 *  August 2023
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#ifndef Technology_H
#define Technology_H

/*
 * Flag definition used as Technologies values
 */
#define TECH_PASSIVE_NFCA 0
#define TECH_PASSIVE_NFCB 1
#define TECH_PASSIVE_NFCF 2
#define TECH_ACTIVE_NFCA 3
#define TECH_ACTIVE_NFCF 5
#define TECH_PASSIVE_15693 6

class Tech {
public:
  enum Value {
    PASSIVE_NFCA = 0,
    PASSIVE_NFCB = 1,
    PASSIVE_NFCF = 2,
    ACTIVE_NFCA = 3,
    ACTIVE_NFCF = 5,
    PASSIVE_15693 = 6,
    PASSIVE_NFCV = 6
  };
};

#endif
