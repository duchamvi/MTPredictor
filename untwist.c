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