#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_SEED 5489

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

void readValuesFile(char* filename, int n, uint32_t output[]){
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(output[i]))!= EOF) && i < n) {
		printf("%u\n", output[i]);
		i++;
	}
	printf("Closing file %s\n", filename);
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

int main(/*int argc, char *argv[]*/)
{
	
	// Init Mersenne Twister
	mt_internal_state_t mt_internal_state;
	uint32_t seed = DEFAULT_SEED;
	mt_init(&mt_internal_state, seed);

	// Test
	//testUntwist(&mt_internal_state);
	untwistFile("mtoutput.txt");

	return 0;
}