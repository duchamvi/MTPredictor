#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>


void readOutputFile(char* filename, int n, uint32_t output[]) 
{   
    /* read a Mersenne Twister output file and writes it into output[]*/
	uint32_t buffer;
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(buffer))!= EOF) && i < n*8) {
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


mt_internal_state_t mt_empty_state()
{
    mt_internal_state_t state;
	state.index = 624;
    return state;
}


uint32_t mt_temper(uint32_t state_word)
{
    return temper(state_word);
} 


uint32_t mt_reverse_temper(uint32_t ouput_word)
{
    return reverseTemper(ouput_word);
} 


void mt_twist(mt_internal_state_t * mt_state)
{
    twist(mt_state);
} 


void mt_generate_next_ouput(mt_internal_state_t state)
{
    uint32_t sortie = mt_generate_number(&state);
    printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
    sortie = sortie >> 16;
    printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
}


void demoUntwistFile(char* filename)	// Test untwist file
{
	// extract values
	uint32_t output[MT_SIZE];
	readValuesDemoFile(filename, MT_SIZE, output);

	printf("Reversing output\n");
	mt_internal_state_t creation;
	reverseState(output, creation.MT);
	creation.index = 624;
	printf("Internal state recreated\n");


	printf("Predictions\n");

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


/* main */
int main()
{
	demoUntwistFile("demo_mt.txt");
	return 0;
}
