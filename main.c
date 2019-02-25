#include "MT19937.h"
#include "untwist.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>


void enterpause()
{
	printf("\nPress Enter to continue\n\n");
	getchar();
}

void readValuesDemoFile(char* filename, int n, uint32_t output[]) 
/* read a Mersenne Twister output file */
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


void readValuesTruncatedDemoFile(char* filename, int n, uint32_t output[], int nbSortiesJetees) // Read file with output truncated
/* read a Truncated Mersenne Twister output file */
{
	int nbGardees = 8 - nbSortiesJetees;
	uint32_t buffer;
	printf("Opening file %s\n", filename);
	FILE * mtfile = fopen(filename, "r");
	int i=0;
	printf("Reading file %s\n", filename);
	while ((fscanf(mtfile, "%u\n", &(buffer))!= EOF) && i < n*nbGardees) {
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


void demoUntwistFile(char* filename)	// Test untwist file
{
	// extract values
	uint32_t output[MT_SIZE];
	readValuesDemoFile(filename, MT_SIZE, output);

	enterpause();

	printf("Reversing output\n");
	mt_internal_state_t creation;
	reverseState(output, creation.MT);
	creation.index = 624;
	printf("Internal state recreated\n");

	enterpause();

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


void demoUntwistTruncatedFile(char* filename, int nbSortiesJetees)	// Test untwist truncated file
{
	// extract values
	uint32_t output[2*MT_SIZE];
	readValuesTruncatedDemoFile(filename, 2*MT_SIZE, output, nbSortiesJetees);

	enterpause();

	printf("Reversing output\n");
	mt_internal_state_t creation;
	guessTruncated(output, creation.MT, 4*nbSortiesJetees);
	twist(&creation);
	creation.index = 624;

	printf("Internal state recreated\n");

	enterpause();

	printf("Predictions\n");

	int i;
	char c;
	uint32_t sortie;
	do
	{
		sortie = mt_generate_number(&creation);
		for (i=0; i < (8-nbSortiesJetees); i++){
			printf("%02u	", (sortie>> (4*i)) &0xf); 
		}		
		printf("\n");
		c = getchar();
	} while (c != 'q');
}


/* main */
int main(/*int argc, char *argv[]*/)
{

	int mode = 0; // 0 : mt, 1: mt_truncated

	if (mode == 0){
		/* Demo 1 : Untwist file with 624 first values => 8*624 first outputs of the lfsr*/
		demoUntwistFile("demo_mt.txt");	// Demo untwist file
	}
	else
	{
		/* Demo 2 : Untwist file with truncated values */
		demoUntwistTruncatedFile("demo_mt_truncated.txt", 3);	// Demo untwist truncated file
	}

	return 0;
}
