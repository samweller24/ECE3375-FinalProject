#include "address_map_arm.h"

volatile unsigned int * const weightInputADC = (unsigned int *)ADC_BASE;// adc port 0xFF204000

int getData(){
    weightInputADC[0] = 0x1; // a value written to channel 0 causes a refresh

    volatile int readOut = weightInputADC[0]; //take the reading and store into readout for return to main
    readOut &= 0xFFF; //bitwise and to isolae just the 12 lsb coming from poten in demo

    return(readOut * 10 / 4096); // will divide the 12 bits into 10 subdivisions, the low value indication begeins at 3/10
}