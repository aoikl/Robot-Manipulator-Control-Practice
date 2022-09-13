#include <Arduino.h>
#include <MegaEncoderCounter.h>
#include "Myfunctions.h"

void setup() {
    Serial.begin(9600);  //鮑率設定
    pinInit();           //引腳設定
    Serial.println(Theta2);
    YReset();  //將Y軸之計數器歸零
    attachInterrupt(digitalPinToInterrupt(infrared_1), IR_Readings, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), IR_Readings, CHANGE);
}
/*
角度控制 10
手臂原點復歸 10
過行程保護 10
End dffector座標控制 20
PID座標控制 40
PID+T curve座標控制 10
*/
void loop() {
    Serial.println("請選擇以下項目：");
    Serial.println("[a]控制角度");
    Serial.println("[y]打開緊急停止");
    Serial.println("[n]關閉緊急停止");
    while (Serial.available() == 0) {
    }
    if (Serial.available() > 0) {
        char command = Serial.read();
        switch (command) {
            case 'a':
                Serial.println("輸入想要控制的軸：");
                Serial.println("[X] or [Y]");
                while (Serial.available() == 0) {
                }
                if (Serial.available() > 0) {
                    char axis = Serial.read();
                    switch (axis) {
                        case 'X':
                            Serial.print("輸入想要角度值：");
                            Serial.print("[0~360]");
                            while (Serial.available() == 0) {
                            }
                            int Desired_theta_1 = Serial.parseInt();
                            Serial.println(Desired_theta_1);
                            Angle_control('X', Desired_theta_1);
                            break;
                    
                    }
                }
                break;
            case 'n':
                detachInterrupt(digitalPinToInterrupt(infrared_1));
                detachInterrupt(digitalPinToInterrupt(infrared_2));
                Serial.println("關閉緊急停止功能");
                delay(1000);
                break;
            case 'y':
                // change代表infra引腳電位有變化就觸發中斷函式
                attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
                attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
                Serial.println("開啟緊急停止功能");
                delay(1000);
                break;
            case 'r':
                Y_Mastering();
                break;
            default:
                Serial.println("無此指令，請重新輸入");
                break;
        }
    }
}
