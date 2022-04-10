#include "address_map_arm.h"
#include "Potentiometer.c"
#include "WifiModule.c"

//pointers
volatile int* timerPTR = (int*)TIMER_BASE;
volatile int* ledPTR = (int*)LED_BASE;

//threshold to set fill level
int fillThreshold = 3;
int fillMinimum = 1;
int clientId = 0;
int unitId = 0;


int main(void) { 
	*(timerPTR + 1) = 0b1000;
	*(timerPTR + 2) = 0xCA00;
	*(timerPTR + 3) = 0x3B9A;
    //3B9ACA00 = 1000000000ns eqaul to 1s
	
	while(1) {		
        //get data from potent function call
        int currentRead = getData();

        //run loop 30 times for 30 seconds
        int i;
        for(i = 0; i < 30; i++){
            *(timerPTR + 1) = 0b0100;
            while(*(timerPTR) != 0b01) { }
            i++;
        }

        //create wifi msg
        WifiMessage msg;

        //get current time
        time_t currentTime;
        currentTime = time(NULL);

        //assign values
        msg.clientId = clientId;
        msg.unitId = unitId;
        msg.reading = currentRead;
        msg.timestamp = currentTime;    

        //get wifi msg and simulate send (display)
        char* message = buildCharArray(msg);
        displayScrollingMsg(*message);

        //output LEDs
        //above threshold - green or 3rd LED
        if(currentRead > fillThreshold){
            //trigger LED
            *ledPTR = 0b001;
        }

        //if under threshold and not zero
        if (currentRead <= fillThreshold && currentRead >= fillMinimum){
            //trigger LED
            *ledPTR = 0b010;
        }

        //if below minimum value (emopty)
        if(currentRead <= fillMinimum){
            //trigger LED
            *ledPTR = 0b100;
        
        }
	}
}