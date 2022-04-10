/*
The Strategy:
Create a standardized message structure and display on the LCD (scrolling message perhaps)
*/

#include "address_map_arm.h"
#include "string.h"
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>

// Pointers to seven-segment display LEDs
volatile int* lowerSeg = (int*) HEX3_HEX0_BASE;
volatile int* upperSeg = (int*) HEX5_HEX4_BASE;

unsigned int num_hex[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67 };

// Lettters to display on 7-seg display
int c_hex = 0x39;
int r_hex = 0x50;
int u_hex = 0x1C;
int t_hex = 0x0F;

/**
 * @brief Wifi Message Structure
 * 
 */
typedef struct s_data {
    int clientId;
    int unitId;
    int reading;
    int timestamp;
} WifiMessage;

/**
 * @brief Displays array of 6 characters (hex-encoded) on the LCD displays
 * 
 * @param msg Array of 6 integer values
 */
void display(int msg[]) {
    unsigned int lowerSegs = (msg[3] << 24) | (msg[2] << 16) | (msg[1] << 8) | msg[0];
	unsigned int upperSegs = (msg[5] << 8) | msg[4];
	*(lowerSeg) = lowerSegs;
	*(upperSeg) = upperSegs;
}

/**
 * @brief Delay progrma execution for a specified number of seconds
 * 
 * @param secs Length of delay in seconds
 */
void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

// Helper function to display message
void displayScrollingMsg(char msg[]) {
    int displayQueue[] = {0, 0, 0, 0, 0, 0}; // To hold all current values displayed on LCD
    int msgComplete = 0;
    int blankDigits = 0;
    int msgPtr = 0;
    while(1) {
        // If message has been written and all digits are blank, break the loop
        if (blankDigits == 6) {
            break;
        }

        if (!msgComplete) {
            msgComplete = msgPtr == sizeof(msg);
        }

        // Shift all queue elements over by 1
        displayQueue[5] = displayQueue[4];
        displayQueue[4] = displayQueue[3];
        displayQueue[3] = displayQueue[2];
        displayQueue[2] = displayQueue[1];
        displayQueue[1] = displayQueue[0];

        if (msgComplete) {
            displayQueue[0] = 0; // Display nothing
            blankDigits++;
        } else {
            // Add next character from message
            char next = msg[msgPtr++];
            if (isdigit(next)) {
                int nextInt = (int) next;
                displayQueue[0] = num_hex[nextInt];
            } else {
                if (next == 'c') {
                    displayQueue[0] = c_hex;
                } else if (next == 'r') {
                    displayQueue[0] = r_hex;
                } else if (next == 'u') {
                    displayQueue[0] = u_hex;
                } else if (next == 't') {
                    displayQueue[0] = t_hex;
                }
            }
        }

        display(displayQueue);

        // Delay 1 second
        waitFor(1);
    }
}

/**
 * @brief Helper function to determine the number of digits needed to represent an integer
 * 
 * @param n The integer to analyze
 * @return int The number of digits
 */
int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

void pushDigitsToArray(char arr[], char num[], int offset) {
    int numDigits = sizeof(num);
    int placeIndex;
    for (placeIndex = offset; placeIndex < offset + numDigits; placeIndex++) {
        arr[placeIndex] = num[placeIndex - offset];
    }
}

const char* buildCharArray(WifiMessage msg) {
    int clientIdLength = numPlaces(msg.clientId);
    int unitIdLength = numPlaces(msg.unitId);
    int readingLength = numPlaces(msg.reading);
    int timestampLength = numPlaces(msg.timestamp);

    char clientId[clientIdLength];
    char unitId[unitIdLength];
    char reading[readingLength];
    char timestamp[timestampLength];

    sprintf(clientId, "%d", msg.clientId);
    sprintf(unitId, "%d", msg.unitId);
    sprintf(reading, "%d", msg.reading);
    sprintf(timestamp, "%d", msg.timestamp);

    // Determine message size
    int numDigs = 3; // Start with 3 (one for each flag character)
    numDigs = numDigs + clientIdLength + unitId + readingLength + timestampLength;

    // Build char array containing final message
    char message[numDigs];
    int insertPtr = 0;
    message[insertPtr] = "c";
    insertPtr += 1;
    pushDigitsToArray(message, clientId, insertPtr);
    insertPtr += clientIdLength;
    message[insertPtr] = "u";
    insertPtr += 1;
    pushDigitsToArray(message, unitId, insertPtr);
    insertPtr += unitIdLength;
    message[insertPtr] = "r";
    insertPtr += 1;
    pushDigitsToArray(message, reading, insertPtr);
    insertPtr += readingLength;
    message[insertPtr] = "t";
    insertPtr += 1;
    pushDigitsToArray(message, timestamp, insertPtr);
    
    return message;
}