#include <Arduino.h>
#include <MegaEncoderCounter.h>
// Encorder
#define a1c1 1.77;
#define a2c2 23.17;
signed long encorder_1 = 0;
signed long encorder_2 = 0;
MegaEncoderCounter megaEncoderCounter(4);
float Theta2 = 0;

// Motor
#define PWM_M2 7  // Enable_B
#define IN_3 10   //  Motor_2 pin
#define IN_4 11   //  Motor_2 pin

void M2_forward(int);
void M2_backward(int);
void Motor_stop();

void setup() {
    Serial.begin(9600);
    pinMode(PWM_M2, OUTPUT);
    pinMode(IN_3, OUTPUT);
    pinMode(IN_4, OUTPUT);
    // Encorder
    megaEncoderCounter.YAxisReset();
}

void loop() {
    // Encorder
    encorder_2 = megaEncoderCounter.YAxisGetCount();
    Serial.print("輸入想要角度值：");
    // float Theta1 = (float)encorder_1 / a1c1;
    while (Serial.available() == 0) {
    }
    int Desired_theta_2 = Serial.parseInt();
    int theta_2_count = (int)Desired_theta_2 * a2c2;  // a2c2=23.17
    Serial.println(Desired_theta_2);

    while (encorder_2 < theta_2_count) {
        encorder_2 = megaEncoderCounter.YAxisGetCount();
        Theta2 = (float)encorder_2 / a2c2;
        Serial.print("Theta2=");
        Serial.println(Theta2);
        M2_forward(50);
    }
    while (encorder_2 > theta_2_count) {
        encorder_2 = megaEncoderCounter.YAxisGetCount();
        Theta2 = (float)encorder_2 / a2c2;
        Serial.print("Theta2=");
        Serial.println(Theta2);
        M2_backward(50);
    }
    Motor_stop();
    // motor
    // while (Serial.available() == 0) {
    // }
    // char incomingbyte = Serial.read();
    // if (incomingbyte == 'c') {
    //     Serial.print("輸入想要的速度：");
    //     while (Serial.available() == 0) {
    //     }
    //     if (Serial.available() > 0) {
    //         int motor_speed = Serial.parseInt();
    //         moveLeft(motor_speed);
    //         Serial.print(motor_speed);
    //     }
    // }
    // if (incomingbyte == 'w') {
    //     Serial.print("輸入想要的速度：");
    //     while (Serial.available() == 0) {
    //     }
    //     if (Serial.available() > 0) {
    //         int motor_speed = Serial.parseInt();
    //         moveRight(motor_speed);
    //         Serial.print(motor_speed);
    //     }
    // }
    // if (incomingbyte == 's') {
    //     analogWrite(PWM_M2, 0);
    //     Serial.println("STOP");
    // }
}

/*==============

Function

===============*/
void M2_forward(int spe) {
    analogWrite(PWM_M2, spe);
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
}
void M2_backward(int spee) {
    analogWrite(PWM_M2, spee);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
}
void Motor_stop() {
    analogWrite(PWM_M2, 0);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
}