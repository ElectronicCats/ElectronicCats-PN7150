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

// #include <time.h>

/* LOOP_REF and CLOCK_CALL_TIME must be adapted according to the CPU execution
 * time */
#define LOOP_REF 6800
#define CLOCK_CALL_TIME 40

void Sleep(unsigned int ms) {
  int i;
#ifndef DEBUG_SEMIHOSTING
  for (i = 0; i < (ms * LOOP_REF); i++)
    asm("nop");
#else
  if (ms <= CLOCK_CALL_TIME) {
    for (i = 0; i < (ms * LOOP_REF); i++)
      asm("nop");
  } else {
    clock_t time = clock() + ((ms - CLOCK_CALL_TIME) / 10);
    while (clock() <= time)
      ;
  }
#endif
}
