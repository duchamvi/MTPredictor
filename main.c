#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_SEED 5489


/* Functions used for technical tests*/
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


void testUntwist(mt_internal_state_t* mt_internal_state) //Test basic untwist 

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



void readValuesFile(char* filename, int n, uint32_t output[]) // Read 32-bits words file
{
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(output[i]))!= EOF) && i < n) {
		/*printf("%u\n", output[i]);*/
		i++;
	}
	printf("Closing file %s, read %d values\n", filename, i);
	fclose(mtfile);
}


void untwistFile(char* filename)	// Test untwist file
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

void truncatebits(uint32_t output[], int n, int k)	// truncate bits of the output
/* n = length of output, k = amount of bits to remove */
{
	uint32_t mask = 0xffffffff >> k;
	int i;
	for (i=0; i<n; i++){
		output[i] &= mask;
		/*printf("0x%8x\n", output[i]);*/
	}
}


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

void guessInaive(uint32_t output[], int k, int i) 
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

void testuntwisttruncatedFile(char* filename, int n, int k)	// Test untwist truncated file
/* n = length of output, k = amount of bits to remove */
{
	
	uint32_t mask = 0xffffffff >> k;

	// extract values
	uint32_t output[n];
	readValuesFile(filename, n, output);
	truncatebits(output, n, k);

	mt_internal_state_t creation;
	guessTruncated(output, creation.MT, k);
	creation.index = 624;

	printf("Check :\n");
	int i;
	for (i=0; i<MT_SIZE; i++){
		printf("%08x = %08x ?\n", mask & mt_generate_number(&creation), output[i+ MT_SIZE]);
	}

	printf("Predictions :\n");
	for (i=0; i<50; i++){
		printf("%08x = %08x ?\n", mask & mt_generate_number(&creation), output[i+ 2 * MT_SIZE]);
	}
}

void testTempering()	// test the distributivity of the untempering function
{
	uint32_t a, b, c, d;
	a = 0x12385678;
	b = 0xabcdef12;

	c = reverseWord(a ^ b);
	d = reverseWord(a) ^ reverseWord(b);

	printf("c = 0x%08x d = 0x%08x\n", c, d);
}

/* functions used for demos */
void readValuesDemoFile(char* filename, int n, uint32_t output[]) // Read file with output divided in 8*624 unsigned integers
{
	uint32_t buffer;
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(buffer))!= EOF) && i < n*8) {
		/*printf("%u\n", output[i]);*/
		if (i%8 == 0){
			output[i/8] = buffer;
		}
		else {
			output[i/8] += buffer << (4*(i%8));  
		}
		i++;
	}
	printf("Closing file %s, read %d values\n", filename, i);
	fclose(mtfile);
}

void demoUntwistFile(char* filename)	// Test untwist file
{
	// extract values
	uint32_t output[MT_SIZE];
	readValuesDemoFile(filename, MT_SIZE, output);

	printf("\nPredictions :\n");
	mt_internal_state_t creation;
	reverseState(output, creation.MT);
	creation.index = 624;
	
	char c;
	uint32_t sortie;
	do
	{
		sortie = mt_generate_number(&creation);
		printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
		sortie = sortie >> 16;
		printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
		c = getchar();
	} while (c != 'q');
}

void readValuesTruncatedDemoFile(char* filename, int n, uint32_t output[], int nbSortiesJetees) // Read file with output truncated
{
	int nbGardees = 8 - nbSortiesJetees;
	uint32_t buffer;
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(buffer))!= EOF) && i < n*nbGardees) {
		/*printf("%u\n", output[i]);*/
		if (i%nbGardees == 0){
			output[i/nbGardees] = buffer;
		}
		else {
			output[i/nbGardees] += buffer << (4*(i%nbGardees));  
		}
		i++;
	}
	printf("Closing file %s, read %d values\n", filename, i);
	fclose(mtfile);
}


void demoUntwistTruncatedFile(char* filename, int nbSortiesJetees)	// Test untwist truncated file
{
	// extract values
	uint32_t output[MT_SIZE];
	readValuesTruncatedDemoFile(filename, 2*MT_SIZE, output, nbSortiesJetees);

	mt_internal_state_t creation;
	guessTruncated(output, creation.MT, 4*nbSortiesJetees);
	creation.index = 624;

	printf("Check :\n");
	int i;
	uint32_t dummy;
	for (i=0; i<MT_SIZE; i++){
		dummy = mt_generate_number(&creation);
	}
	
	printf("\nPredictions :\n");
	char c;
	uint32_t sortie;
	do
	{
		sortie = mt_generate_number(&creation);
		for (i=0; i < 8-nbSortiesJetees; i++){
			printf("%02u	", (sortie>> (4*i) &0xf)); 
		}		
		printf("\n");
		c = getchar();
	} while (c != 'q');
}


/* main */
int main(/*int argc, char *argv[]*/)
{
	//untwistFile("mtoutput.txt"); //untwist file with 624 values
	
	/*
	//untwist File with truncated values
	char* filename = "mtoutput.txt";
	int n = 1999;
	int k = 4;
	testuntwisttruncatedFile(filename, n, k);*/

	/* Demo 1 : Untwist file with 624 first values => 8*624 first outputs of the lfsr*/
	demoUntwistFile("demo_mt.txt");	// Demo untwist file
	/* Demo 2 : Untwist file with truncated values */
	demoUntwistTruncatedFile("demo_mt_truncated.txt", 624);	// Demo untwist truncated file

	return 0;
}