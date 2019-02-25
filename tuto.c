#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>


void readOutputFile(char* filename, int n, uint32_t output[]) 
{   
    /* 
	read a Mersenne Twister output file and writes it into output[]
	inputs : name of the file with the values, array of destination of the values	
	*/
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
	/*
	initialize Mersenne Twister empty state
	ouput : empty mt_internal_state
	*/
    mt_internal_state_t state;
	state.index = 624;
	// Remark : state.MT is an array with the 624 internal state 32-bit words
    return state;
}


uint32_t mt_temper(uint32_t state_word)
{
	/*
	temper function of the Mersenne Twister 
	input : 32-bit word from the state
	output : 32-bit output from the Mersenne Twister
	*/
    return temper(state_word);
} 


uint32_t mt_reverse_temper(uint32_t ouput_word)
{
	/*
	reverse temper function
	input : 32-bit output from the Mersenne Twister
	output : 32-bit word from the state
	*/
    return reverseTemper(ouput_word);
} 


void mt_twist(mt_internal_state_t * mt_state)
{
	/*
	twist function of the Mersenne Twister 
	input : mt_internal_state*
	*/
    twist(mt_state);
} 


void mt_generate_next_ouput(mt_internal_state_t state)
{
	/*
	generate the next outputs for the wheel
	input : mt_internal_state*
	*/
    uint32_t sortie = mt_generate_number(&state);
    printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
    sortie = sortie >> 16;
    printf("%02u	%02u	%02u	%02u\n", sortie&0xf, (sortie>>4)&0xf, (sortie>>8)&0xf, (sortie>>12)&0xf);
}


/* main */
int main()
{
	/* Compléter */ 
	return 0;
}
