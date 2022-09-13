#include "Manipulator.h"
#include "MegaEncoderCounter.h"
MegaEncoderCounter megaEncoderCounter(4);

/*============================

        Values setting

==============================*/

const int in1 = 8;  // 第一軸(+),輸出時第一軸逆時針正轉
const int in2 = 9;  // 第一軸(-),輸出時第一軸順時針逆轉
const int enA = 6;
const int in3 = 10;  // 第二軸(+),輸出時第二軸逆時針正轉
const int in4 = 11;  // 第二軸(-),輸出時第二軸順時針逆轉
const int enB = 7;

float Motor1_speed = 70;
float Motor2_speed = 40;  // 45~50

const float a1c1 = 15.0;
const float a2c2 = 23.17;  // encoder_count(每度脈衝數)= 分辨率/360

//紅外線：有外物遮擋回傳 1；無則 0
const int infrared_1 = 2;  //第一軸紅外線腳位:2
const int infrared_2 = 3;  //第二軸紅外線腳位:3
float Theta1 = 0;
float Theta2 = 0;

// 極限保護記憶體
// limit = 1:觸發保護 , 0:在工作範圍內
bool limit = 0;

// 馬達運動記憶體
// index = 1:馬達尚未達到指定位置 , 0:馬達已達成目的
bool index = 1;

/*============================

        Functions

==============================*/
MANIPULATOR::MANIPULATOR() {
}
MANIPULATOR::~MANIPULATOR() {
}
void MANIPULATOR::pinInit() {
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
    // 編碼器歸零
    megaEncoderCounter.XAxisReset();
    megaEncoderCounter.YAxisReset();
}

// 讀取X、Y軸當前角度
void MANIPULATOR::XRead_Theta1() {
    signed long encoder_1 = megaEncoderCounter.XAxisGetCount();
    Theta1 = (float)encoder_1 / a1c1;  // 當前馬達1角度值
    Serial.print("Theta1=");
    Serial.println(Theta1);
}
void MANIPULATOR::YRead_Theta2() {
    signed long encoder_2 = megaEncoderCounter.YAxisGetCount();
    Theta2 = (float)encoder_2 / a2c2;  // 當前馬達2角度值
    Serial.print("Theta2=");
    Serial.println(Theta2);
}

// X軸馬達逆時針轉(+)
void MANIPULATOR::X_forward(float X_speed) {
    analogWrite(enA, X_speed);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    XRead_Theta1();
}
// X軸馬達順時針轉(-)
void MANIPULATOR::X_backward(float X_speed) {
    analogWrite(enA, X_speed);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    XRead_Theta1();
}
// Y軸小馬達逆時針轉(+)
void MANIPULATOR::Y_forward(float Y_speed) {
    analogWrite(enB, Y_speed);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    YRead_Theta2();
}
// Y軸小馬達順時針轉(-)
void MANIPULATOR::Y_backward(float Y_speed) {
    analogWrite(enB, Y_speed);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    YRead_Theta2();
}
// X軸馬達停止
void MANIPULATOR::X_stop() {
    analogWrite(enA, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}
// Y軸小馬達停止
void MANIPULATOR::Y_stop() {
    analogWrite(enB, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// 極限保護
void Limit_Protect() {
    Serial.println("偵測到異物，緊急停止!");
    limit = 1;
    analogWrite(enA, 0);
    analogWrite(enB, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

// X、Y軸復歸
void MANIPULATOR::X_Mastering() {
    Serial.println("第一軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));

    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_1) == 0) {
        X_backward(Motor1_speed);
        Serial.println("X軸逆轉中...");
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值歸零
    megaEncoderCounter.XAxisReset();
    // XRead_Theta1();
    Serial.print("X軸歸零, Theta1=");
    Serial.println(Theta1);
    // delay(100);  //打開可動
    //  開始原點復歸
    while (Theta1 < 90) {
        X_forward(Motor1_speed);
    }
    // XRead_Theta1();  //旭霖
    // delay(500);      //旭霖
    while (Theta1 > 90) {
        X_backward(Motor1_speed);
    }
    X_stop();  //旭霖
    Y_stop();  //旭霖
    // Reset Encoder讀值
    megaEncoderCounter.XAxisReset();
    Serial.println("X軸歸零");
    XRead_Theta1();
    //重啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
    Serial.println("=>X軸復歸完成");
    XRead_Theta1();  //旭霖
    // 記憶體初始化
    limit = 0;
    index = 1;
}

void MANIPULATOR::Y_Mastering() {
    Serial.println("第二軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));
    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_2) == 0) {
        Y_backward(Motor2_speed);
        Serial.println("Y軸逆轉中...");
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值歸零
    megaEncoderCounter.YAxisReset();

    Serial.print("Y軸歸零, Theta2=");
    Serial.println(Theta2);  //  開始原點復歸
    // delay(100);
    while (Theta2 < 180) {
        Y_forward(Motor2_speed);
    }
    // YRead_Theta2();
    // delay(500);
    while (Theta2 > 180) {
        Y_backward(Motor2_speed);
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值
    megaEncoderCounter.YAxisReset();
    Serial.println("Y軸歸零");
    YRead_Theta2();
    //重啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
    Serial.println("=>Y軸復歸完成");
    YRead_Theta2();
    // 記憶體初始化
    limit = 0;
    index = 1;
}

void MANIPULATOR::Set_Interrupt() {
    Serial.println("");
    Serial.println("");
    Serial.println("@=====================");
    Serial.println("    --中斷保護設定--    ");
    Serial.println("[y]打開中斷保護");
    Serial.println("[n]關閉中斷保護");
    Serial.println("@=====================");
    while (Serial.available() == 0) {
    }
    char _setInter = Serial.read();
    if (_setInter == 'y') {
        // change代表infra引腳電位有變化就觸發中斷函式
        attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
        attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
        Serial.println("=>開啟中斷保護成功");
        delay(1000);
    }
    if (_setInter == 'n') {
        detachInterrupt(digitalPinToInterrupt(infrared_1));
        detachInterrupt(digitalPinToInterrupt(infrared_2));
        Serial.println("=>關閉中斷保護成功");
        delay(1000);
    }
}
// 絕對角度控制
void MANIPULATOR::Angle_control() {
    Serial.println("");
    Serial.println("輸入欲控制的X軸角度：");
    Serial.println("[0~360]");
    while (Serial.available() == 0) {
    }
    float _setTheta1 = Serial.parseFloat();
    Serial.println("");
    Serial.print("_setTheta1=");
    Serial.println(_setTheta1);
    Serial.println("");
    Serial.println("輸入欲控制的Y軸角度：");
    Serial.println("[0~360]");
    while (Serial.available() == 0) {
    }
    float _setTheta2 = Serial.parseFloat();
    Serial.println("");
    Serial.print("_setTheta2=");
    Serial.println(_setTheta2);
    Serial.println("");
    Serial.println("");
    Serial.println("@=====================");
    Serial.println("    --角度控制模式--    ");
    Serial.println("[s]X, Y軸先後運轉");
    Serial.println("[]...");
    Serial.println("@=====================");
    while (Serial.available() == 0) {
    }
    char _setMode = Serial.read();
    if (_setMode == 's') {
        Angle_Move_Seperate(_setTheta1, _setTheta2);
    } else {
        Serial.println("");
        Serial.println("無此選項，請重新輸入!");
    }
}

void MANIPULATOR::Angle_Move_Seperate(float _desireTheta1, float _desireTheta2) {
    if (limit == 0) {  // 極限保護
        while ((limit == 0) && (index == 1)) {
            Serial.println('a');
            //第1軸轉到user輸入之Desire角度
            while (Theta1 < _desireTheta1) {
                X_forward(Motor1_speed);
            }
            while (Theta1 > _desireTheta1) {
                X_backward(Motor1_speed);
            }
            X_stop();
            Serial.println("=>X軸完成!");
            //第2軸轉到user輸入之Desire角度
            while (Theta2 < _desireTheta2) {
                Y_forward(Motor2_speed);
            }
            while (Theta2 > _desireTheta2) {
                Y_backward(Motor2_speed);
            }
            Y_stop();
            Serial.println("=>Y軸完成!");
            index = 0;
        }
        X_stop();
        Y_stop();
    }
    // 記憶體初始化
    limit = 0;
    index = 1;
}