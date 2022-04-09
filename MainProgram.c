#include "address_map_arm.h"
volatile int* timerPTR = (int*)TIMER_BASE;


int main(void) { 
	*(timerPTR + 1) = 0b1000;
	*(timerPTR + 2) = 0x4240;
	*(timerPTR + 3) = 0x000F;
	
	while(1) {												
		*(timerPTR + 1) = 0b0100;
        //timer countdown
		while(*(timerPTR) != 0b01) { }

        //main program code
	}
}