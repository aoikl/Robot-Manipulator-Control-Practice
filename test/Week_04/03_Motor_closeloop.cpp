// Main code
#include <Arduino.h>
#include <MegaEncoderCounter.h>
#define a2c2 23.17  // encoder_count(每度脈衝數)= 分辨率/360
                    //分辨率 = 減速機減速比 * 編碼器PPR * 解碼器解析度  //減速比1:298 encoder ppr:7

#define enB 7   // 第二軸Enable腳位
#define in3 10  // 第二軸(+),輸出時第二軸正轉
#define in4 11  // 第二軸(-),輸出時第二軸逆轉

#define Motor2_speed 45  // 45~50

signed long encoder_2 = 0;                 // 宣告一個變數encoder_2用來儲存計數器讀值
MegaEncoderCounter megaEncoderCounter(4);  //設定編碼器的計數模式為4x

void M2_forward();
void M2_backward();
void Motor_stop();

void setup() {
    Serial.begin(9600);
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    megaEncoderCounter.YAxisReset();  //將Y軸之計數器歸零
}

void loop() {
    encoder_2 = megaEncoderCounter.YAxisGetCount();
    // Read the desired theta
    Serial.print("Please enter Desired Theta2: ");
    while (Serial.available() == 0) {
    }
    int Desired_theta2 = Serial.parseFloat();
    int theta_2_count = Desired_theta2 * a2c2;  // a2c2=23.17,轉到需要脈衝數
    Serial.println(Desired_theta2);

    while (encoder_2 < theta_2_count) {
        encoder_2 = megaEncoderCounter.YAxisGetCount();
        float Theta2 = (float)encoder_2 / a2c2;  // 當前馬達2角度值
        Serial.print("Theta2 = ");
        Serial.println(Theta2);
        M2_forward();
    }
    while (encoder_2 > theta_2_count) {
        encoder_2 = megaEncoderCounter.YAxisGetCount();
        float Theta2 = (float)encoder_2 / a2c2;  // 當前馬達2角度值
        Serial.print("Theta2 = ");
        Serial.println(Theta2);
        M2_backward();
    }
    Motor_stop();  //完成角度控制，馬達停止
}

void M2_forward() {
    analogWrite(enB, Motor2_speed);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

void M2_backward() {
    analogWrite(enB, Motor2_speed);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

void Motor_stop() {
    analogWrite(enB, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}