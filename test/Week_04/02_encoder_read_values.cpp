
#include <Arduino.h>
#include <MegaEncoderCounter.h>            //導入函式庫
#define a1c1 1.77                          // encoder_count(每度脈衝數)= 分辨率/360
                                        //分辨率 = 減速機減速比 * 編碼器PPR * 解碼器解析度  //減速比1:50 encoder ppr:3.2
signed long encoder_1 = 0;                 // 宣告一個變數encoder_1用來儲存計數器讀值
MegaEncoderCounter megaEncoderCounter(4);  //設定編碼器的計數模式為4x

void setup() {
    Serial.begin(9600);
    megaEncoderCounter.YAxisReset();  //將Y軸之計數器歸零
}

void loop() {
    encoder_1 = megaEncoderCounter.YAxisGetCount();  //讀取Y軸當前計數器讀值，並儲存於變數encoder_1
    float Theta1 = (float)encoder_1 / a1c1;          // 當前馬達1角度值
    Serial.print("Theta1 = ");
    Serial.println(Theta1);
}