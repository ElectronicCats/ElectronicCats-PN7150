/**
 * Library to get the interface type of the NFC card
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

#ifndef Interface_H
#define Interface_H

/*
 * Flag definition used as Interface values
 */
#define INTF_UNDETERMINED 0x0
#define INTF_FRAME 0x1
#define INTF_ISODEP 0x2
#define INTF_NFCDEP 0x3
#define INTF_TAGCMD 0x80

class Interface {
public:
  enum Value {
    UNDETERMINED = 0x0,
    FRAME = 0x1,
    ISODEP = 0x2,
    NFCDEP = 0x3,
    TAGCMD = 0x80
  };
};

#endif
