#include "untwist.h"

uint32_t reverseRightShiftXor(uint32_t number, int shift)
{
    uint32_t stateWord = 0; // word in the mersenne twister
    uint32_t shiftmask = (0xffffffff << (32 - shift)) & 0xffffffff; // position of bits to copy
    stateWord ^= shiftmask & number;
    
    uint32_t stepmask;
    int step = 0;
    while (step * shift < 32) {
        step++;
        stepmask = stateWord & shiftmask;    // copy of the bits
        stepmask = stepmask >> shift;
        shiftmask = shiftmask >> shift;

        stateWord ^= (shiftmask & number)^stepmask;  // XOR of copied bits
    }
    return stateWord;
}

uint32_t reverseLeftShiftXor(uint32_t number, int shift, uint32_t mask)
{

    uint32_t stateWord = 0;   // word in the mersenne twister

    uint32_t shiftmask = (0xffffffff << (shift)) ^ 0xffffffff;   // position of bits to copy
    stateWord ^= shiftmask & number;
    
    uint32_t stepmask;
    int step = 0;
    while (step * shift < 32) {
        step++;
        stepmask = stateWord & shiftmask;    // copy of the bits
        stepmask = (stepmask << shift) & mask;
        shiftmask = (shiftmask << shift) & 0xffffffff;

        stateWord ^= (shiftmask & number)^stepmask; //XOR of copied bits
    }
    return stateWord;
}

uint32_t reverseWord(uint32_t number)
{
    number = reverseRightShiftXor(number, 18);
    number = reverseLeftShiftXor(number, 15, 0xEFC60000);
    number = reverseLeftShiftXor(number, 7, 0x9D2C5680);
    number = reverseRightShiftXor(number, 11);
    return number;
}

void reverseState(uint32_t output[], uint32_t state_destination[])
{
    int i;
    for (i=0; i<MT_SIZE; i++){
        state_destination[i] = reverseWord(output[i]);
    }
}

uint32_t reconstructPartialState(uint32_t statei0, uint32_t statei1)
{
	uint32_t state, temp;
	temp = (statei0 & 0x80000000) + (statei1 & 0x7fffffff);
	state =  temp >> 1; 
	if (temp%2 != 0){
		state ^= 0x9908b0df;
	}
	return state;
}


void guess0(uint32_t output[], uint32_t stateInit[], int k)
{
	uint32_t i0, i1, state, word;
	uint32_t bound = 0x1 << k;

	for (i0 = 0; i0 <= 1; i0++) {
		uint32_t bit0 = (i0 << 31);
		/*printf("0x%x\n", bit0);*/

		for (i1 = 0; i1 < bound; i1++){
			/*printf("0x%x\n", i1 <<(32-k));*/
			uint32_t state1 = reverseWord(output[1] ^ (i1 << (32 - k)));
            /* RECONSTRUCTION */
            state = reconstructPartialState(bit0, state1);
            word = temper(state) ^ output[397];
            /*printf("Word = 0x%x\n", word);*/
            if ((word & (0xffffffff >> k)) == output[624]){
                stateInit[0] = bit0;
                stateInit[1] = state1;
            } 
		}
	}
} 

void guessi(uint32_t output[], uint32_t stateInit[], int k, int i)
{
	uint32_t i1, state, word;
	uint32_t bound = 0x1 << k;

    for (i1 = 0; i1 < bound; i1++){
        /*printf("0x%x\n", i1 <<(32-k));*/
        uint32_t state1 = reverseWord(output[i + 1] ^ (i1 << (32 - k)));
        /* RECONSTRUCTION */
        state = reconstructPartialState(stateInit[i], state1);
        word = temper(state) ^ output[i + 397];
        /*printf("Word = 0x%x\n", word);*/
        if ((word & (0xffffffff >> k)) == output[i + 624]){
            stateInit[i + 1] = state1;
        } 
    }
} 

void guessTruncated(uint32_t output[], uint32_t stateInit[], int k)
{
    guess0(output, stateInit, k);
    int i;
    for (i=1; i< MT_SIZE-1; i++){
        guessi(output, stateInit, k, i);
    } 
}