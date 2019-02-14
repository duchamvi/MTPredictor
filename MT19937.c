#include "MT19937.h"

static const uint32_t MAGIC = 0x9908b0df;

#define M32(x) (0x80000000 & x)
#define L31(x) (0x7FFFFFFF & x)

#define UNROLL(internal_state_pointer,expr) \
  y = M32(internal_state_pointer->MT[i]) | L31(internal_state_pointer->MT[i+1]); \
  internal_state_pointer->MT[i] = internal_state_pointer->MT[expr] ^ (y >> 1) ^ ((( (int32_t)y << 31) >> 31) & MAGIC); \
  ++i;

void twist(mt_internal_state_t* mt_internal_state)
{
	uint32_t i = 0;
	uint32_t y;
	while ( i < MT_DIFF ) {
		UNROLL(mt_internal_state,i+MT_PERIOD);
	}

  	while ( i < MT_SIZE -1 ) {
		UNROLL(mt_internal_state,i-MT_DIFF);
  	}
  	
	mt_internal_state->index = 0;
}

uint32_t temper(uint32_t y)
{	
	y ^= y >> 11;
	y ^= y << 7  & 0x9d2c5680;
	y ^= y << 15 & 0xefc60000;
	y ^= y >> 18;
	return y;
}

uint32_t mt_generate_number(mt_internal_state_t* mt_internal_state)
{
	if (mt_internal_state -> index >= MT_SIZE){
		twist(mt_internal_state);
	} 
	
	uint32_t word;
	word = temper(mt_internal_state->MT[mt_internal_state->index]);
	(mt_internal_state->index)++; 
	return word;
}


void mt_init(mt_internal_state_t* mt_internal_state, uint32_t seed)
{
	mt_internal_state->MT[0] = seed;
	int i;
	for (i = 1; i < MT_SIZE; i++) {
		mt_internal_state->MT[i] = 0xffffffff & (0x6C078965 *(mt_internal_state->MT[i-1]^(mt_internal_state->MT[i-1]>>30)) + i);
	}
	mt_internal_state->index = MT_SIZE;
}


