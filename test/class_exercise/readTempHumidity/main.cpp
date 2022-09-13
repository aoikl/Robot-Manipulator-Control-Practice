#include <Arduino.h>
#include "readTempHumidity.h"
//#include "tempNotification.h"

void setup() {
    Serial.begin(115200);
}

void loop() {
    readTempHumidity();
    // tempNotification();
    delay(1000);
}