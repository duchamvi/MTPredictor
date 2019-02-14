#ifndef __UNTWIST_H__
#define __UNTWIST_H__

#include <stdint.h>
#include <stdio.h>
#include "MT19937.h"

uint32_t reverseRightShiftXor(uint32_t number, int shift);
uint32_t reverseLeftShiftXor(uint32_t number, int shift, uint32_t mask);
uint32_t reverseWord(uint32_t number);

#endif