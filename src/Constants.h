/**
 * Constants for the PN7150 NFC library
 * Authors:
 *        Electronic Cats - electroniccats.com
 *
 * This code is beerware; if you see me (or any other collaborator
 * member) at the local, and you've found our code helpful,
 * please buy us a round!
 * Distributed as-is; no warranty is given.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stddef.h>
#include <stdint.h>

// WiFi record constants
#define WIFI_RECORD_HEADER_SIZE 29

// Timeout values
#define TIMEOUT_INFINITE 0
#define TIMEOUT_100MS    100
#define TIMEOUT_500MS    500
#define TIMEOUT_1S       1000
#define TIMEOUT_2S       2000

// Special timeout marker for infinite wait in getMessage
#define TIMEOUT_INFINITE_MARKER 1337

// NCI constants
#define NCI_MAX_FRAME_SIZE 255
#define NCI_HEADER_SIZE    3

// I2C constants
#define DEFAULT_I2C_ADDRESS 0x28

// NDEF constants
#define MAX_NDEF_RECORDS      32
#define MAX_NDEF_MESSAGE_SIZE 249

#endif // CONSTANTS_H
