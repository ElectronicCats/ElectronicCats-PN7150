/**
 * Library to manage the mode of the NFC chip
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

#ifndef Mode_H
#define Mode_H

#include "Arduino.h"

struct Mode_t {
  enum { READER_WRITER = 1, EMULATION = 2, P2P = 3 };
};

class Mode {
private:
  int _mode;

protected:
  bool setMode(int mode); // Only for internal use

public:
  Mode();
  Mode_t mode;
  int getMode();
};

#endif
