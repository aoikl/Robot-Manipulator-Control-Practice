/*============================

        Include

==============================*/
#include <Arduino.h>
#include <MegaEncoderCounter.h>
MegaEncoderCounter megaEncoderCounter(4);

#define a1c1 15     // 第一軸 encoder_count
#define a2c2 23.17  // 第二軸 encoder_count  減速比298:1 encoder ppr:7

#define enA 6  // 第一軸Enable腳位
#define enB 7  // 第二軸Enable腳位

#define in1 8   // 第一軸(+)
#define in2 9   // 第一軸(-)
#define in3 10  // 第二軸(+),輸出時第二軸正轉
#define in4 11  // 第二軸(-),輸出時第二軸逆轉

#define infrared_1 2  // 第一軸紅外線腳位
#define infrared_2 3  // 第二軸紅外線腳位

// Constant speed
#define Motor1_speed 70  //預設70
#define Motor2_speed 50  //預設50

// 極限保護記憶體
// limit = 1 觸發保護 , 0 在工作範圍內
bool limit = 0;

float Theta1 = 0, Theta2 = 0;  // 當前角度位置

void M1_forward();
void M1_backward();
void M2_forward();
void M2_backward();
void Motor_stop();
void Limit_protect();
void Read_encoder();
void Return_1();
void Return_2();

/*============================

        Setup

==============================*/
void setup() {
    Serial.begin(9600);  // 鮑率設定

    //宣告各腳位型式
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(enA, OUTPUT);

    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(enB, OUTPUT);

    pinMode(infrared_1, INPUT);
    pinMode(infrared_2, INPUT);

    // 編碼器歸零
    megaEncoderCounter.XAxisReset();
    megaEncoderCounter.YAxisReset();

    // 開啟極限保護
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_protect, CHANGE);
}

void loop() {
    Serial.println("");
    Serial.println("Please choose a function:");
    Serial.println("a = Motor angle mode control");
    Serial.println("p = End effector position mode control");
    Serial.println("r = Return to origin");
    Serial.println("i = Interrupt Config.");
    Serial.println("");
    char incomingByte;
    while (Serial.available() == 0) {
    }
    incomingByte = Serial.read();
    switch (incomingByte) {
        case 'r':  // 原點復歸
            // Return();
            Return_1();
            Return_2();
            break;

        case 'i':  // 極限保護設定
            Serial.println("Do you want to open the interrupt function?");
            Serial.println("entet [y] : open interrupt function.");
            Serial.println("entet [n] : close interrupt function.");

            while (Serial.available() == 0) {
            }
            incomingByte = Serial.read();

            if (incomingByte == 'y') {
                attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_protect, CHANGE);
                attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_protect, CHANGE);
            }
            if (incomingByte == 'n') {
                detachInterrupt(digitalPinToInterrupt(infrared_1));
                detachInterrupt(digitalPinToInterrupt(infrared_2));
            }
            break;
    }
}

/*============================

        Function

==============================*/

void M1_forward() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}
void M1_backward() {
    digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW);
}

void M2_forward() {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}
void M2_backward() {
    digitalWrite(in4, HIGH);
    digitalWrite(in3, LOW);
}

void Motor_stop() {
    analogWrite(enA, 0);
    analogWrite(enB, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// 觸發極限保護時將limit改成1,以免手臂繼續運作
void Limit_protect() {
    limit = 1;
    analogWrite(enA, 0);
    analogWrite(enB, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void Read_encoder() {
    signed long encoder_1 = megaEncoderCounter.XAxisGetCount();  // 當前X編碼器讀值
    signed long encoder_2 = megaEncoderCounter.YAxisGetCount();  // 當前Y編碼器讀值

    Theta1 = (float)encoder_1 / a1c1;  // 當前馬達1角度值
    Theta2 = (float)encoder_2 / a2c2;  // 當前馬達2角度值

    Serial.print("Theta1 = ");
    Serial.print(Theta1);
    Serial.print(",");
    Serial.print("Theta2 = ");
    Serial.println(Theta2);
}

void Return_1() {
    Serial.println("第一軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));

    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_1) == 0) {
        analogWrite(enA, Motor1_speed);
        M1_backward();
    }
    Motor_stop();

    // Reset Encoder讀值
    megaEncoderCounter.XAxisReset();

    analogWrite(enA, Motor1_speed);
    while (Theta1 < 90) {
        M1_forward();
        Read_encoder();
    }

    while (Theta1 > 90) {
        M1_backward();
        Read_encoder();
    }

    megaEncoderCounter.XAxisReset();

    // 開啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_protect, CHANGE);
    Serial.println("軸1復歸完成!");
}

void Return_2() {
    Serial.println("第二軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));

    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_2) == 0) {
        analogWrite(enB, Motor2_speed);
        M2_backward();
    }
    Motor_stop();

    // Reset Encoder讀值
    megaEncoderCounter.YAxisReset();

    analogWrite(enB, Motor2_speed);
    while (Theta2 < 180) {
        M2_forward();
        Read_encoder();
    }

    while (Theta2 > 180) {
        M2_backward();
        Read_encoder();
    }
    megaEncoderCounter.YAxisReset();

    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_protect, CHANGE);
    Serial.println("軸2復歸完成!");
}