/**
 * Library to get the mode tech of the NFC card
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

#ifndef ModeTech_H
#define ModeTech_H

/*
 * Flag definition used as Mode values
 */
#define MODE_POLL 0x00
#define MODE_LISTEN 0x80
#define MODE_MASK 0xF0

class ModeTech {
public:
  enum Value { POLL = 0x00, LISTEN = 0x80, MASK = 0xF0 };
};

#endif
