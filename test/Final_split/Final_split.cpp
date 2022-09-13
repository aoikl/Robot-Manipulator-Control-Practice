#include <Arduino.h>
#include "Manipulator.h"
MANIPULATOR Manipulator;

void setup() {
    Serial.begin(9600);     //鮑率設定
    Manipulator.pinInit();  //引腳設定
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
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
    Serial.println("");
    Serial.println("");
    Serial.println("======================");
    Serial.println("請選擇以下項目：");
    Serial.println("[r]X、Y軸復歸");
    Serial.println("[i]中斷保護設定");
    Serial.println("[a]角度控制");
    Serial.println("======================");
    Serial.println("當前設定：");
    Serial.print("limit=");
    Serial.println(limit);
    Serial.print("index=");
    Serial.println(index);
    Serial.print("Theta1=");
    Serial.println(Theta1);
    Serial.print("Theta2=");
    Serial.println(Theta2);
    Serial.println("======================");
    char type_command;
    while (Serial.available() == 0) {
    }
    type_command = Serial.read();
    switch (type_command) {
        case 'r': {
            // Manipulator.Y_Mastering();
            Manipulator.X_Mastering();
        } break;
        case 'i': {
            Manipulator.Set_Interrupt();
        } break;
        case 'a': {
            Manipulator.Angle_control();
        } break;
        default: {
            Serial.println("");
            Serial.println("無此選項，請重新輸入!");
        } break;
    }
}
