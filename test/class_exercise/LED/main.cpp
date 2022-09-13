/**********************
實例化1個LED對象，用7號叫控制，讓他閃爍10次，並在串口打印出它的狀態。
10次完畢后釋放回收引腳
**********************/

#include "LED.h"

LED led(7);
byte count = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (count < 10) {
        led.on();
        delay(300);
        Serial.print("LED state:");
        Serial.println(led.getState(), DEC);

        led.off();
        delay(300);
        Serial.print("LED state:");
        Serial.println(led.getState(), DEC);

        ++count;
        if (count == 10)
            led.disattach();
    }
}