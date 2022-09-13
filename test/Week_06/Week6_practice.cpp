#include <Arduino.h>
/*此週進度須讓二軸手臂達成以下三點目標:
 * 1.雙軸原點復歸(先復歸第2軸,在復歸第1軸)(可以先分開復歸,若可達成同時復歸會加分)
 * 2.雙軸角度定速追跡,必須設計人機溝通介面,讓user輸入2軸的分別角度後,手臂雙軸轉到相對應角度(若可同動會加分)
 * 3.雙軸必須具有極限保護功能,若在運轉過程中有東西觸發紅外線感測器,手臂必須立即停止
 */

// Main code
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
// limit = 1:觸發保護 , 0:在工作範圍內
bool limit = 0;

// 馬達運動記憶體
// index = 1:馬達尚未達到指定位置 , 0:馬達已達成目的
bool index = 1;

int oin = 0;                                   //計數器
float Theta1 = 0, Theta2 = 0;                  // 當前角度位置
float Desired_Theta1 = 0, Desired_Theta2 = 0;  // 目標角度位置

void Angle_mode() {
    // Read the desired theta
    Serial.print("Please enter Desired Theta1: ");
    while (Serial.available() == 0) {
    }
    Desired_Theta1 = Serial.parseFloat();
    Serial.println(Desired_Theta1);

    Serial.print("Please enter Desired Theta2: ");
    while (Serial.available() == 0) {
    }
    Desired_Theta2 = Serial.parseFloat();
    Serial.println(Desired_Theta2);
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
    Motor1_stop();

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
    Motor1_stop();

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
    Motor2_stop();

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
    Motor2_stop();
    megaEncoderCounter.YAxisReset();

    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_protect, CHANGE);
    Serial.println("軸2復歸完成!");
}

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

void Motor1_stop() {
    analogWrite(enA, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}

void Motor2_stop() {
    analogWrite(enB, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// 觸發極限保護時將limit改成1,以免手臂繼續運作
void Limit_protect() {
    Serial.println("Alarm!!!");
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

void Move_simultanously() {
    if (limit == 0)  // 極限保護
    {
        analogWrite(enA, Motor1_speed);
        analogWrite(enB, Motor2_speed);

        while (index == 1 && limit == 0) {
            Read_encoder();
            if (Theta1 < Desired_Theta1) {
                M1_forward();
            }
            if (Theta1 > Desired_Theta1) {
                M1_backward();
            }
            if (Theta2 < Desired_Theta2) {
                M2_forward();
            }
            if (Theta2 > Desired_Theta2) {
                M2_backward();
            }

            if (abs(Desired_Theta1 - Theta1) < 5 && abs(Desired_Theta2 - Theta2) < 5) {
                oin++;
                if (oin > 10) {
                    index = 0;
                    analogWrite(enA, 0);
                    analogWrite(enB, 0);
                }
            }
        }
        Motor_stop();
        Serial.println("Complete !");
    }

    // 參數初始化
    index = 1;
    limit = 0;
    oin = 0;
}

void Move_simultanously_seperate() {
    if (limit == 0)  // 極限保護
    {
        while (index == 1 && limit == 0) {
            //第1軸轉到user輸入之Desire角度
            analogWrite(enA, Motor1_speed);
            while (Theta1 < Desired_Theta1) {
                M1_forward();
                Read_encoder();
            }
            while (Theta1 > Desired_Theta1) {
                M1_backward();
                Read_encoder();
            }
            Motor1_stop();

            //第2軸轉到user輸入之Desire角度
            analogWrite(enB, Motor2_speed);
            while (Theta2 < Desired_Theta2) {
                M2_forward();
                Read_encoder();
            }
            while (Theta2 > Desired_Theta2) {
                M2_backward();
                Read_encoder();
            }
            Motor2_stop();
            index = 0;
        }
        Motor_stop();
        Serial.println("Complete !");
    }

    // 參數初始化
    index = 1;
    limit = 0;
}

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
    char incomingByte, incomingByte1;
    while (Serial.available() == 0) {
    }
    incomingByte = Serial.read();
    switch (incomingByte) {
        case 'a':
            Angle_mode();

            Serial.println("");
            Serial.println("Please choose move type :");
            Serial.println("c = Constant speed mode");

            while (Serial.available() == 0) {
            }
            incomingByte1 = Serial.read();
            if (incomingByte1 == 'c') {
                // Move_simultanously();
                Move_simultanously_seperate();
            }
            break;

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