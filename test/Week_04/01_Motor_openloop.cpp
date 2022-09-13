#include <Arduino.h>
#define enB 9
#define in3 7
#define in4 8
int motor_speed = 0;
int in3_value = 0;
int in4_value = 0;

void setup() {
    Serial.begin(9600);
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
}

void loop() {
    Serial.println("Please determine ccw or cw or stop: ccw[f],cw[b],stop[s] ");
    while (Serial.available() == 0) {
    }
    char incomingbyte = Serial.read();
    if (incomingbyte == 'f') {
        Serial.print("Please Enter desire motor speed: ");
        while (Serial.available() == 0) {
        }
        if (Serial.available() > 0) {
            motor_speed = Serial.parseInt();
            Serial.println("motor direction is ccw");
            Serial.print("motor speed:");
            Serial.println(motor_speed);
        }
        in3_value = 1;
        in4_value = 0;
    }

    if (incomingbyte == 'b') {
        Serial.print("Please Enter desire motor speed: ");
        while (Serial.available() == 0) {
        }
        if (Serial.available() > 0) {
            motor_speed = Serial.parseInt();
            Serial.println("motor direction is cw");
            Serial.print("motor speed:");
            Serial.println(motor_speed);
        }
        in3_value = 0;
        in4_value = 1;
    }
    if (incomingbyte == 's') {
        motor_speed = 0;
    }

    analogWrite(enB, motor_speed);
    digitalWrite(in3, in3_value);
    digitalWrite(in4, in4_value);
}