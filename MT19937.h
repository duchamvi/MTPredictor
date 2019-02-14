#ifndef __MT19937_H__
#define __MT19937_H__

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdio.h>

#define MT_SIZE 624
#define MT_PERIOD 397
#define MT_DIFF (MT_SIZE - MT_PERIOD)

typedef struct{
  uint32_t MT[MT_SIZE];
  uint32_t index;
} mt_internal_state_t;

void twist(mt_internal_state_t* mt_internal_state);
uint32_t temper(uint32_t y);

void mt_init(mt_internal_state_t* mt_internal_state, uint32_t seed);
uint32_t mt_generate_number(mt_internal_state_t* mt_internal_state);

#endif