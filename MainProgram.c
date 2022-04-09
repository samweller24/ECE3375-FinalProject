#include "address_map_arm.h"
#include "Potentiometer.c"
#include "WifiModule.c"

//pointers
volatile int* timerPTR = (int*)TIMER_BASE;
volatile int* ledPTR = (int*)LED_BASE;

//threshold to set fill level
int fillThreshold = 3;


int main(void) { 
	*(timerPTR + 1) = 0b1000;
	*(timerPTR + 2) = 0xCA00;
	*(timerPTR + 3) = 0x3B9A;
    //3B9ACA00 = 1000000000ns eqaul to 1s
	
	while(1) {		
        //set to start										
		*(timerPTR + 1) = 0b0100;
        //get data from potent function call
        int currentRead = getData();

        //timer countdown
		while(*(timerPTR) != 0b01) { }

        //main program code
        //above threshold - green or 3rd LED
        if(currentRead > fillThreshold){
            //trigger LED
            *ledPTR = 0b001;
        }

        //if under threshold and not zero
        if (currentRead <= fillThreshold && currentRead != 0){
            //trigger LED
            *ledPTR = 0b010;
            //trigger data for refill
        }

        //if empty
        if(currentRead == 0){
            //trigger LED
            *ledPTR = 0b100;
            //trigger empty data send
        }
	}
}