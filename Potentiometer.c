/**
 * @file Potentiometer.c
 * @author Samuel Weller, Andreas Mavroudis, Jayson Dale
 * @brief Utility to read value of potentiometer
 * @version 0.1
 * @date 2022-04-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "address_map_arm.h"

volatile unsigned int * const weightInputADC = (unsigned int *)ADC_BASE;// Pointer to ADC port located at 0xFF204000

/**
 * @brief Get reading from potentiometer
 * 
 * @return int Integer reading scaled to be between 0-10
 */
int getData(){
    weightInputADC[0] = 0x1; // A value written to channel 0 causes a refresh

    volatile int readOut = weightInputADC[0]; // Take the reading and store into readout for return to main
    readOut &= 0xFFF; // Bitwise AND to isolae just the 12 least significant bits coming from potentiometer

    return(readOut * 10 / 4096); // Divide the 12 bits into 10 subdivisions, the low value indication begeins at 3/10
}