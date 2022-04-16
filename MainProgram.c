/**
 * @file MainProgram.c
 * @authors Samuel Weller, Andreas Mavroudis, Jayson Dale
 * @brief The main program logic for our IoT-enabled sanitizer dispenser embedded system
 * @version 0.1
 * @date 2022-04-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "address_map_arm.h"
#include "Potentiometer.c"
#include "WifiModule.c"

// Pointers for the interval timer and built-in LEDs
volatile int* timerPTR = (int*)TIMER_BASE;
volatile int* ledPTR = (int*)LED_BASE;

// Pre-programmed thresholds to determine which LEDs to illuminate
int fillThreshold = 5;
int fillMinimum = 1;
int clientId = 0;
int unitId = 0;

// Timer interval for 30s on a 100MHz interval timer
int timeInterval = 3000000000;

// Counter to keep track of the number of readings taken since initialization
volatile int unitCount = 0;

/**
 * @brief Programs the interval timer to run for a specified interval and
 * 
 * @param timeInt 
 */
void initTimer(int timeInt){
    *(timerPTR + 1) = 0b1000;       // Enable the stop bit
    *(timerPTR + 2) = timeInt;      // Program the lower 16 bits of the interval timer period register
    *(timerPTR + 3) = timeInt << 16;// Program the upper 16 bits of the interval timer period register
}

/**
 * @brief Starts the timer and waits for timeout to occur
 * 
 */
void runTimer(){
    *(timerPTR + 1) = 0b0100;
    while(*(timerPTR) != 0b01) { }
}

/**
 * @brief Constructs a WiFi message reference for a specified reading
 * 
 * @param read Raw sensor reading value
 * @return WifiMessage 
 */
WifiMessage buildWifiMessage(int read){
    // Intiialize reference
    WifiMessage msg;
    
    // assign values
    msg.clientId = clientId;
    msg.unitId = unitId;
    msg.reading = read;
    msg.timestamp = unitCount++;

    return msg;
}

int main(void) {
    // Initialize the timer for 30s interval
    initTimer(timeInterval);
    
    while(1) {
        // Activate the timer's start control bit
        runTimer();

        // Get the potentiometer reading (simulating the sensor reading)
        int currentRead = getData();

        // Build WiFi message reference
        WifiMessage msg = buildWifiMessage(currentRead);

        // Get WiFi message and simulate transmission (via 7-segment display)
        char* message = buildCharArray(msg);
        int messageLength = computeMessageLength(msg);
        displayScrollingMsg(message, messageLength);

        // Trigger output LEDs
        // If above threshold, illuminate 3rd LED
        if(currentRead > fillThreshold){
            *ledPTR = 0b100;
        }

        // If under threshold but not below minimum value, illuminate second LED
        if (currentRead <= fillThreshold && currentRead >= fillMinimum){
            *ledPTR = 0b010;
        }

        // If below minimum value, illuminate first LED
        if(currentRead < fillMinimum){
            *ledPTR = 0b001;
        }
    }
}
