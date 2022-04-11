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
int timeInterval = 3000000000;

void initTimer(int timeInt){
    *(timerPTR + 1) = 0b1000;
	*(timerPTR + 2) = timeInt;
	*(timerPTR + 3) = timeInt << 16;
}

void runTimer(){
    *(timerPTR + 1) = 0b0100;
    while(*(timerPTR) != 0b01) { }
}

WifiMessage buildWifiMessage(int read, time_t time){
    //create wifi msg
    WifiMessage msg;
    //assign values
    msg.clientId = clientId;
    msg.unitId = unitId;
    msg.reading = read;
    msg.timestamp = time;

    return msg;
}

int main(void) { 
	initTimer(timeInterval);
	
	while(1) {		
        //run timer
        runTimer();

        //get data from potent function call
        int currentRead = getData();    
        //get current time
        time_t currentTime;
        currentTime = time(NULL);

        //create msg
        WifiMessage msg = buildWifiMessage(currentRead, currentTime);

        //get wifi msg and simulate send (display)
        char* message = buildCharArray(msg);
        int messageLength = computeMessageLength(msg);
        displayScrollingMsg(message, messageLength);

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