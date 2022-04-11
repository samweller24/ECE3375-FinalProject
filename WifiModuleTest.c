#include "WifiModule.c"

int main() {
    WifiMessage m;
    m.clientId = 0;
    m.unitId = 1;
    m.reading = 255500;
    m.timestamp = 32225;
    char* arr = buildCharArray(m);
    int msgLength = computeMessageLength(m);
    int i;
    for (i = 0; i <= msgLength; i++) {
        printf("%c", arr[i]);
    }
}