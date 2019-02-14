#include "MT19937.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int nbOut = 10;
	mt_internal_state_t mt_internal_state;
	uint32_t seed = 5489;
	
	// Mersenne Twister
	mt_init(&mt_internal_state, seed);

	uint32_t word;
	int i;
	for (i = 0; i < nbOut; i++)
	{
		word = mt_generate_number(&mt_internal_state);
		printf("%p\n", (void *)word);
	}
	
	return 0;
}
