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

#include "Mode.h"

Mode::Mode() { this->_mode = mode.READER_WRITER; }

bool Mode::setMode(int mode) {
  if (mode < 1 || mode > 3) {
    return false;
  }

  this->_mode = mode;
  return true;
}

int Mode::getMode() { return this->_mode; }
