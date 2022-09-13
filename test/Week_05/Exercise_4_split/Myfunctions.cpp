#include "Myfunctions.h"
#include <Arduino.h>
#include <MegaEncoderCounter.h>

//設定編碼器的計數模式為4x
MegaEncoderCounter megaEncoderCounter(4);

/*============================

        Values setting

==============================*/
int in1 = 8;  // 第一軸(+),輸出時第一軸正轉
int in2 = 9;  // 第一軸(-),輸出時第一軸逆轉
int enA = 6;
int in3 = 10;  // 第二軸(+),輸出時第二軸正轉
int in4 = 11;  // 第二軸(-),輸出時第二軸逆轉
int enB = 7;

int Motor1_speed = 60;
int Motor2_speed = 45;  // 45~50

float a1c1 = 15;
float a2c2 = 23.17;  // encoder_count(每度脈衝數)= 分辨率/360
float encoder_1 = 0;
float encoder_2 = 0;

int infrared_1 = 2;  //第一軸紅外線腳位:2
int infrared_2 = 3;  //第二軸紅外線腳位:3
float Theta1 = 0;
float Theta2 = 0;

bool limit = 0;

/*============================

        Functions

==============================*/
void pinInit() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(enA, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(enB, OUTPUT);
    // Arduino會依據紅外線感測器訊號做判斷
    //因此腳位宣告為INPUT
    pinMode(infrared_1, INPUT);
    pinMode(infrared_2, INPUT);
}

// X軸馬達逆時針轉(+)
void X_forward() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}
// X軸馬達順時針轉(-)
void X_backward() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}
// Y軸小馬達逆時針轉(+)
void Y_forward() {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}
// Y軸小馬達順時針轉(-)
void Y_backward() {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}
// X軸馬達停止
void X_stop() {
    analogWrite(enA, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}
// Y軸小馬達停止
void Y_stop() {
    analogWrite(enB, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// X、Y軸角度讀值歸零
void XReset() {
    megaEncoderCounter.XAxisReset();  //將Y軸之計數器歸零
}
void YReset() {
    megaEncoderCounter.YAxisReset();  //將Y軸之計數器歸零
}

// X、Y軸當前角度讀值
void XRead_Theta1() {
    encoder_1 = megaEncoderCounter.XAxisGetCount();
    Theta1 = (float)encoder_1 / a1c1;  // 當前馬達2角度值
}
void YRead_Theta2() {
    encoder_2 = megaEncoderCounter.YAxisGetCount();
    Theta2 = (float)encoder_2 / a2c2;  // 當前馬達2角度值
}

// 紅外線感測器讀值
void IR_Readings() {
    Serial.print(digitalRead(infrared_1));
    Serial.println(digitalRead(infrared_2));
    delay(50);
}

// 極限保護
void Limit_Protect() {
}

// X、Y軸復歸
void X_Mastering() {
}
void Y_Mastering() {
    YRead_Theta2();
    Serial.println("第二軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));
    // 馬達轉到光遮斷後停止
    while (!digitalRead(infrared_2)) {
        analogWrite(enB, Motor2_speed);
        Y_backward();
    }
    Y_stop();
    // Reset Encoder讀值
    megaEncoderCounter.YAxisReset();
    // 開始原點復歸
    analogWrite(enB, Motor2_speed);
    while (Theta2 < 180) {
        Y_forward();
        YRead_Theta2();
    }
    delay(500);
    while (Theta2 > 180) {
        Y_backward();
        YRead_Theta2();
        // Reset Encoder讀值
        megaEncoderCounter.YAxisReset();
        //重啟中斷
        attachInterrupt(digitalPinToInterrupt(infrared_1), IR_Readings, CHANGE);
        attachInterrupt(digitalPinToInterrupt(infrared_2), IR_Readings, CHANGE);
        Serial.println("Y軸復歸完成!");
    }
}

// 絕對角度控制
void Angle_control(char _axis, float _angle) {
    if (_axis == 'X') {
        while (encoder_1 < _angle) {
            encoder_1 = megaEncoderCounter.XAxisGetCount();
            Theta1 = (float)encoder_1 / a1c1;
            Serial.print("Theta1=");
            Serial.println(Theta1);
            analogWrite(enA, Motor1_speed);
            X_forward();
        }
        while (encoder_1 > _angle) {
            encoder_1 = megaEncoderCounter.XAxisGetCount();
            Theta1 = (float)encoder_1 / a1c1;
            Serial.print("Theta1=");
            Serial.println(Theta1);
            X_backward();
        }
        X_stop();
    }
}