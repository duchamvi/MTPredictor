#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_SEED 5489


/* Test the MT */
void testMT(mt_internal_state_t mt_internal_state, 	int nbOut)
{
	uint32_t word;
	int i;
	for (i = 0; i < nbOut; i++)
	{
		word = mt_generate_number(&mt_internal_state);
		printf("0x%x\n", word);
	}
}


/* Test basic untwist */
void testUntwist(mt_internal_state_t* mt_internal_state)
{
	uint32_t output[MT_SIZE];
	int i;
	for (i=0; i<MT_SIZE; i++){
		output[i] = mt_generate_number(mt_internal_state);
	}

	mt_internal_state_t creation;

	reverseState(output, creation.MT);
	creation.index = 624;

	for (i=0; i<10; i++){
		printf("0x%x == 0x%x ?\n", mt_generate_number(mt_internal_state), mt_generate_number(&creation));
	}

}


/* Test untwist file */
void readValuesFile(char* filename, int n, uint32_t output[]){
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(output[i]))!= EOF) && i < n) {
		/*printf("%u\n", output[i]);*/
		i++;
	}
	printf("Closing file %s, read %d values\n", filename, n);
	fclose(mtfile);
}


void untwistFile(char* filename)
{
	// extract values
	uint32_t output[MT_SIZE];
	readValuesFile(filename, MT_SIZE, output);

	printf("\nPredictions :\n");
	mt_internal_state_t creation;
	reverseState(output, creation.MT);
	creation.index = 624;
	int i;
	for (i=0; i<10; i++){
		printf("%u\n", mt_generate_number(&creation));
	}
}

/* clean untwist file TODO */

/* Test untwist truncated TODO */
uint32_t reconstructState(uint32_t statei0, uint32_t statei1, uint32_t statei397)
{
	uint32_t state, temp;
	temp = (statei0 & 0x80000000) + (statei1 & 0x7fffffff);
	state =  temp >> 1; 
	if (temp%2 != 0){
		state ^= 0x9908b0df;
	}
	state ^= statei397;
	return state;
}

void truncatebits(uint32_t output[], int n, int k)
/* n = length of output, k = amount of bits to remove */
{
	uint32_t mask = 0xffffffff >> k;
	int i;
	for (i=0; i<n; i++){
		output[i] &= mask;
		/*printf("0x%8x\n", output[i]);*/
	}
}

void guess0naive(uint32_t output[], int k) /* Il realise here that there is alwys an ouput[397] correct for output[1] correct*/
{
	uint32_t i0, i1, i2, state, word;
	uint32_t bound = 0x1 << k;
	int debug = 1;

	for (i0 = 0; i0 <= 1; i0++) {
		uint32_t bit0 = (i0 << 31);
		/*printf("0x%x\n", bit0);*/

		for (i1 = 0; i1 < bound; i1++){
			/*printf("0x%x\n", i1 <<(32-k));*/
			uint32_t state1 = reverseWord(output[1] ^ (i1 << (32 - k)));
			for (i2 = 0; i2 < bound; i2++){
				/*printf("0x%x\n", i1 <<(32-k));*/
				uint32_t state397 = reverseWord(output[397] ^ (i2 << (32 - k)));

				/* RECONSTRUCTION */
				state = reconstructState(bit0, state1, state397);
				word = temper(state);
				/*printf("Word = 0x%x\n", word);*/
				if ((word & (0xffffffff >> k)) == output[624]){
					printf("GOT IT %4d : bit0 = 0x%x,  i1 = 0x%x, i2 = 0x%x, new state[0] = 0x%08x\n", debug, bit0, i1, i2, state);
					debug++;
				} 
			}
		}
	}
} 

void guessInaive(uint32_t output[], int k, int i) //WIP
{
	uint32_t i0, i1, i2, state, word;
	uint32_t bound = 0x1 << k;
	int debug = 1;

	for (i0 = 0; i0 <= 1; i0++) {
		uint32_t bit0 = (i0 << 31);
		/*printf("0x%x\n", bit0);*/

		for (i1 = 0; i1 < bound; i1++){
			/*printf("0x%x\n", i1 <<(32-k));*/
			uint32_t state1 = reverseWord(output[1+i] ^ (i1 << (32 - k)));
			for (i2 = 0; i2 < bound; i2++){
				/*printf("0x%x\n", i1 <<(32-k));*/
				uint32_t state397 = reverseWord(output[397+i] ^ (i2 << (32 - k)));

				/* RECONSTRUCTION */
				state = reconstructState(bit0, state1, state397);
				word = temper(state);
				/*printf("Word = 0x%x\n", word);*/
				if ((word & (0xffffffff >> k)) == output[624+i]){
					printf("GOT IT %4d : bit0 = 0x%x,  i1 = 0x%x, i2 = 0x%x, new state[0] = 0x%08x\n", debug, bit0, i1, i2, state);
					debug++;
				} 
			}
		}
	}
} 

void testuntwisttruncatedFile(char* filename, int n, int k)
/* n = length of output, k = amount of bits to remove */
{
	// extract values
	uint32_t output[n];
	readValuesFile(filename, n, output);
	truncatebits(output, n, k);
	guess0naive(output, k);

	int i;
	for (i=1; i < 30; i++){
		printf("i = %d\n", i);
		guessInaive(output, k, i);
	}
}

void testTempering()
{
	uint32_t a, b, c, d;
	a = 0x12385678;
	b = 0xabcdef12;

	c = reverseWord(a ^ b);
	d = reverseWord(a) ^ reverseWord(b);

	printf("c = 0x%08x d = 0x%08x\n", c, d);
}

/* main */
int main(/*int argc, char *argv[]*/)
{
	char* filename = "mtoutput.txt";
	int n = 624* 2;
	int k = 3;
	// Init 1Mersenne Twister
	/*mt_internal_state_t mt_internal_state;
	uint32_t seed = DEFAULT_SEED;
	mt_init(&mt_internal_state, seed);*/

	// Test
	//testUntwist(&mt_internal_state);
	//untwistFile("mtoutput.txt");
 	
	//testuntwisttruncatedFile(filename, n, k);
	testTempering();
	
	return 0;
}