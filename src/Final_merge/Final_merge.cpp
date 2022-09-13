/*
B10803130_Manipulator
|--Include & Variables
|
|--Functions
|  |--***********↓基礎函式↓*************
|  |--void PinInit()    // 引腳初始化
|  |--void Read_Theta()   // 讀取X、Y軸當前角度
|  |--void X_forward(float X_speed) // X軸馬達逆時針轉(+)
|  |--void X_backward(float X_speed)  // X軸馬達順時針轉(-)
|  |--void Y_forward(float Y_speed) // Y軸小馬達逆時針轉(+)
|  |--void Y_backward(float Y_speed)    // Y軸小馬達順時針轉(-)
|  |--void X_stop() // X軸馬達停止
|  |--void Y_stop() // Y軸小馬達停止
|  |--void Limit_Protect()  // 極限保護，觸發紅外線時=>limit=1，手臂停止
|  |--***********↑基礎函式↑*************
|  |--***********↓選擇項目中的函式↓*************
|  |--void X_Mastering()    // @[r]X、Y軸復歸
|  |--void Y_Mastering()
|  |--void XY_Mastering()   // @[R]X、Y軸(同動)
|  |
|  |--void Interrupt_Setting()  // @[i]中斷保護設定
|  |
|  |--void XY_Seperate()    // @@[s]X, Y軸先後運轉
|  |
|  |--void XY_Simultanously()   //@@[m]X, Y軸同時運轉
|  |
|  |--float X_PID_ANGLE_output()    // X軸PID位置控制
|  |--float Y_PID_ANGLE_output()    // Y軸PID位置控制
|  |--void Scan()
|  |--void PID_XY_Simultanously()   //@@[d](PID) X, Y軸同時運轉
|  |
|  |--void Shifting_Type_Select()   // @@選擇移動到終點的模式
|  |
|  |--void Angle_mode()// @[a]角度模式
|  |
|  |--void Position_mode()// @[p]座標模式(反向運動學)
|  |
|  |--float Y_PID_RPM_output()  // Y軸PID速度控制
|  |--void Encoder2_getrpm()    //讀取當前Y軸轉速
|  |--void PID_Y_Velocity() // @[s]Y軸PID速度控制
|  |--***********↑選擇項目中的函式↑*************
|
|--Setup & Loop
|  |--void setup()
|  |--void loop()
*/

/******************************************

            Include & Variables

******************************************/
#include <Arduino.h>
#include <MegaEncoderCounter.h>
#include <MsTimer2.h>

// 設定編碼器的計數模式為4x
MegaEncoderCounter megaEncoderCounter(4);

// Mega2560引腳
const int in1 = 8;  // 第一軸(+),輸出時第一軸逆時針正轉
const int in2 = 9;  // 第一軸(-),輸出時第一軸順時針逆轉
const int enA = 6;
const int in3 = 10;  // 第二軸(+),輸出時第二軸逆時針正轉
const int in4 = 11;  // 第二軸(-),輸出時第二軸順時針逆轉
const int enB = 7;

//預設速度
float X_speed = 70;
float Y_speed = 45;  // 45~50

// encoder_count(每度脈衝數)= 分辨率/360
const float a1c1 = 15.0;
const float a2c2 = 23.17;

// 紅外線,有外物遮擋回傳 1；無則 0
const int infrared_1 = 2;  //第一軸紅外線腳位:2
const int infrared_2 = 3;  //第二軸紅外線腳位:3

// 極限保護記憶體
volatile bool limit = 0;  // limit = 1:觸發保護 , 0:在工作範圍內

// 馬達運動記憶體
bool index = 1;  // index = 1:馬達尚未達到指定位置 , 0:馬達已達成目的

// 計數器
int oin = 0;

//手臂角度
float Theta1 = 0;  //當前值
float Theta2 = 0;
float Desired_Theta1, Desired_Theta2;  //目標值

// 座標(反向運動學)
const float a1 = 150.0, a2 = 50.0;
float X, Y;
float Theta11, Theta21, Theta12, Theta22;
float displacement1, displacement2;

// PID Y軸轉速參數初始化
float interval = 10;  // 10 ms
long currentMillis = 0;
long previousMillis = 0;
float new_reading = 0;
float old_reading = 0;
float Desired_rpm2 = 0;
float rpm2 = 0;

// PID 位置控制參數
float M1_preError = 0.0;
float M2_preError = 0.0;
float integral_1 = 0.0;
float integral_2 = 0.0;

// PID 位置控制
float X_MAX_speed = 100;
float Y_MAX_speed = 100;
float X_MIN_speed = 55;
float Y_MIN_speed = 20;
float X_u = 0;
float Y_u = 0;

/******************************************

            Functions

******************************************/
//***********↓基礎函式↓*************

// 引腳初始化
void PinInit() {
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

// 讀取X、Y軸當前角度
void Read_Theta() {
    signed long encoder_1 = megaEncoderCounter.XAxisGetCount();
    Theta1 = (float)encoder_1 / a1c1;  // 當前X角度值
    signed long encoder_2 = megaEncoderCounter.YAxisGetCount();
    Theta2 = (float)encoder_2 / a2c2;  // 當前Y角度值
    Serial.print("Theta1 = ");
    Serial.print(Theta1);
    Serial.print(",");
    Serial.print("Theta2 = ");
    Serial.println(Theta2);
}

// X軸馬達逆時針轉(+)
void X_forward(float X_speed) {
    analogWrite(enA, X_speed);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}

// X軸馬達順時針轉(-)
void X_backward(float X_speed) {
    analogWrite(enA, X_speed);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}

// Y軸小馬達逆時針轉(+)
void Y_forward(float Y_speed) {
    analogWrite(enB, Y_speed);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

// Y軸小馬達順時針轉(-)
void Y_backward(float Y_speed) {
    analogWrite(enB, Y_speed);
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

// 極限保護，觸發紅外線時=>limit=1，手臂停止
void Limit_Protect() {
    Serial.println("偵測到異物, 緊急停止!");
    limit = 1;
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enB, 0);
}
//***********↑基礎函式↑*************

//***********↓選擇項目中的函式↓*************
// @[r]X、Y軸復歸
void X_Mastering() {
    Serial.println("X軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));

    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_1) == 0) {
        X_backward(X_speed);
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值歸零
    megaEncoderCounter.XAxisReset();
    Serial.print("X軸歸零, Theta1=");
    Serial.println(Theta1);
    //  開始原點復歸
    while (Theta1 < 90) {
        X_forward(X_speed);
        Read_Theta();
    }
    while (Theta1 > 90) {
        X_backward(X_speed);
        Read_Theta();
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值
    megaEncoderCounter.XAxisReset();
    //重啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
    Serial.println("=>X軸復歸完成");
    // 記憶體初始化
    limit = 0;
    index = 1;
    oin = 0;
}
void Y_Mastering() {
    Serial.println("Y軸復歸中");
    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));
    // 馬達轉到光遮斷後停止
    while (digitalRead(infrared_2) == 0) {
        Y_backward(Y_speed);
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值歸零
    megaEncoderCounter.YAxisReset();

    Serial.print("Y軸歸零, Theta2=");
    Serial.println(Theta2);
    //  開始原點復歸
    while (Theta2 < 180) {
        Y_forward(Y_speed);
        Read_Theta();
    }
    while (Theta2 > 180) {
        Y_backward(Y_speed);
        Read_Theta();
    }
    X_stop();
    Y_stop();
    // Reset Encoder讀值
    megaEncoderCounter.YAxisReset();
    //重啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
    Serial.println("=>Y軸復歸完成");
    // 記憶體初始化
    limit = 0;
    index = 1;
    oin = 0;
}
// @[R]X、Y軸(同動)
void XY_Mastering() {
    Serial.println("手臂復歸中");

    // 紀錄紅外線觸發狀態記憶體
    bool limit_1 = 0, limit_2 = 0;  // limit = 1 已經觸發紅外線, 0 尚未觸發紅外線

    // 編碼器歸零
    megaEncoderCounter.XAxisReset();
    megaEncoderCounter.YAxisReset();

    // 中斷關閉
    detachInterrupt(digitalPinToInterrupt(infrared_1));
    detachInterrupt(digitalPinToInterrupt(infrared_2));

    // 馬達反轉直到觸發紅外線
    X_backward(X_speed);
    Y_backward(Y_speed);

    while (index) {
        Read_Theta();

        // 觸發紅外線訊號後將編碼器讀值歸零,接著馬達正轉至原點後停止旋轉
        if (digitalRead(infrared_1)) {
            megaEncoderCounter.XAxisReset();
            X_forward(X_speed);
            limit_1 = 1;
        }
        if (digitalRead(infrared_2)) {
            megaEncoderCounter.YAxisReset();
            Y_forward(Y_speed);
            limit_2 = 1;
        }

        if (Theta1 < 92 && limit_1 == 1) {
            X_forward(X_speed);
        }
        if (Theta1 > 88 && limit_1 == 1) {
            X_backward(X_speed);
        }

        if (Theta2 < 178 && limit_2 == 1) {
            Y_forward(Y_speed);
        }
        if (Theta2 > 182 && limit_2 == 1) {
            Y_backward(Y_speed);
        }

        if (abs(Theta1 - 90) < 10 && abs(Theta2 - 180) < 10) {
            oin++;
            if (oin > 5) {
                index = 0;
                analogWrite(enA, 0);
                analogWrite(enB, 0);
            }
        } else {
            oin = 0;
        }
    }
    X_stop();
    Y_stop();

    // Reset Encoder讀值
    megaEncoderCounter.XAxisReset();
    megaEncoderCounter.YAxisReset();

    // 重啟中斷
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
    Serial.println("=>XY軸復歸完成");
    Read_Theta();
    // 記憶體初始化
    index = 1;
    limit = 0;
    oin = 0;
}

// @[i]中斷保護設定
void Interrupt_Setting() {
    Serial.println("");
    Serial.println("");
    Serial.println("@@=====================");
    Serial.println("    --中斷保護設定--    ");
    Serial.println("[y]打開中斷保護");
    Serial.println("[n]關閉中斷保護");
    Serial.println("@@=====================");
    while (Serial.available() == 0) {
    }
    char _Enter = Serial.read();
    if (_Enter == 'y') {
        // change代表infra引腳電位有變化就觸發中斷函式
        attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
        attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
        Serial.println("=>開啟中斷保護成功");
        delay(1000);
    }
    if (_Enter == 'n') {
        detachInterrupt(digitalPinToInterrupt(infrared_1));
        detachInterrupt(digitalPinToInterrupt(infrared_2));
        Serial.println("=>關閉中斷保護成功");
        delay(1000);
    }
}

// @@[s]X, Y軸先後運轉
void XY_Seperate() {
    if (limit == 0) {  // 中斷保護
        while ((limit == 0) && (index == 1)) {
            //第1軸轉到user輸入之Desire角度
            while ((Theta1 < Desired_Theta1) && (limit == 0) && (index == 1)) {
                X_forward(X_speed);
                Read_Theta();
            }
            while ((Theta1 > Desired_Theta1) && (limit == 0) && (index == 1)) {
                X_backward(X_speed);
                Read_Theta();
            }
            X_stop();
            Serial.println("=>X軸完成!");
            //第2軸轉到user輸入之Desire角度
            while ((Theta2 < Desired_Theta2) && (limit == 0) && (index == 1)) {
                Y_forward(Y_speed);
                Read_Theta();
            }
            while ((Theta2 > Desired_Theta2) && (limit == 0) && (index == 1)) {
                Y_backward(Y_speed);
                Read_Theta();
            }
            Y_stop();
            Serial.println("=>Y軸完成!");
            index = 0;
        }
        X_stop();
        Y_stop();
        // 記憶體初始化
        limit = 0;
        index = 1;
        Serial.println("=>X, Y軸先後運轉完成!");
    }
}

//@@[m]X, Y軸同時運轉
void XY_Simultanously() {
    if (limit == 0) {  // 中斷保護
        while ((limit == 0) && (index == 1)) {
            Read_Theta();
            if (Theta1 < Desired_Theta1) {
                X_forward(X_speed);
            }
            if (Theta1 > Desired_Theta1) {
                X_backward(X_speed);
            }
            if (Theta2 < Desired_Theta2) {
                Y_forward(Y_speed);
            }
            if (Theta2 > Desired_Theta2) {
                Y_backward(Y_speed);
            }
            if (abs(Desired_Theta1 - Theta1) < 10 && abs(Desired_Theta2 - Theta2) < 10) {
                oin++;
                if (oin > 10) {
                    index = 0;
                    analogWrite(enA, 0);
                    analogWrite(enB, 0);
                }
            }
        }
        X_stop();
        Y_stop();
        // 記憶體初始化
        limit = 0;
        index = 1;
        oin = 0;
        Serial.println("=>X, Y軸同時運轉完成!");
    }
}

// X軸PID位置控制
float X_PID_ANGLE_output() {
    float kp = 8;
    float ki = 1;
    float kd = 0;
    float dt = 0.01;

    float error = Desired_Theta1 - Theta1;
    integral_1 = integral_1 + (error * dt);
    float derivative = (error - M2_preError) / dt;

    float Pout = kp * error;
    float Iout = ki * integral_2;
    float Dout = kd * derivative;
    float output = Pout + Iout + Dout;

    M1_preError = error;
    return output;
}
// Y軸PID位置控制
float Y_PID_ANGLE_output() {
    float kp = 4;
    float ki = 1;
    float kd = 0;
    float dt = 0.01;

    float error = Desired_Theta2 - Theta2;
    integral_2 = integral_2 + (error * dt);
    float derivative = (error - M2_preError) / dt;

    float Pout = kp * error;
    float Iout = ki * integral_2;
    float Dout = kd * derivative;
    float output = Pout + Iout + Dout;

    M2_preError = error;
    return output;
}
void Scan() {
    if (limit == 0) {
        float X_u = X_PID_ANGLE_output();
        float Y_u = Y_PID_ANGLE_output();
        if (X_u > X_MAX_speed) {
            X_u = X_MAX_speed;
        } else if (X_u < X_MIN_speed && X_u > 0) {
            X_u = X_MIN_speed;
        } else if (X_u < -X_MAX_speed) {
            X_u = -X_MAX_speed;
        } else if (X_u > -X_MIN_speed && X_u < 0) {
            X_u = -X_MIN_speed;
        }
        if (Y_u > Y_MAX_speed) {
            Y_u = Y_MAX_speed;
        } else if (Y_u < Y_MIN_speed && Y_u > 0) {
            Y_u = Y_MIN_speed;
        } else if (Y_u < -Y_MAX_speed) {
            Y_u = -Y_MAX_speed;
        } else if (Y_u > -Y_MIN_speed && Y_u < 0) {
            Y_u = -Y_MIN_speed;
        }
        // PID控制量方向設定
        if (X_u > 0) {
            X_forward(X_u);
        } else if (X_u <= 0) {
            X_u = abs(X_u);
            X_backward(X_u);
        }
        if (Y_u > 0) {
            Y_forward(Y_u);
        } else if (Y_u <= 0) {
            Y_u = abs(Y_u);
            Y_backward(Y_u);
        }
    }
}
//@@[d](PID) X, Y軸同時運轉
void PID_XY_Simultanously() {
    MsTimer2::set(10, Scan);
    MsTimer2::start();               //時間中斷開啟
    while (oin < 5 && limit == 0) {  // if error < 5 degree,5 times break
        signed long encoder_1 = megaEncoderCounter.XAxisGetCount();
        Theta1 = (float)encoder_1 / a1c1;  // 當前X角度值
        signed long encoder_2 = megaEncoderCounter.YAxisGetCount();
        Theta2 = (float)encoder_2 / a2c2;  // 當前Y角度值
        Serial.print(Desired_Theta1);
        Serial.print("\t");
        Serial.print(Theta1);
        Serial.print("\t");
        Serial.print(Desired_Theta2);
        Serial.print("\t");
        Serial.println(Theta2);
        if ((abs(Desired_Theta1 - Theta1) <= 5) && (abs(Desired_Theta2 - Theta2) <= 5)) {
            oin++;
        } else {
            oin = 0;
        }
    }
    MsTimer2::stop();
    X_stop();
    Y_stop();
    Serial.print("完成!");
    oin = 0;
}

// @@選擇移動到終點的模式
void Shifting_Type_Select() {
    Serial.println("@@====================");
    Serial.println("    --移動到終點的模式--    ");
    Serial.println("[s]X, Y軸先後運轉");
    Serial.println("[m]X, Y軸同時運轉");
    Serial.println("[d](PID) X, Y軸同時運轉");
    Serial.println("@@====================");
    while (Serial.available() == 0) {
    }
    char _setMode = Serial.read();
    if (_setMode == 's') {
        XY_Seperate();
    } else if (_setMode == 'm') {
        XY_Simultanously();
    } else if (_setMode == 'd') {
        PID_XY_Simultanously();
    } else {
        Serial.println("");
        Serial.println("無此選項, 請重新輸入!");
    }
}

// @[a]角度模式
void Angle_mode() {
    Serial.println("");
    Serial.println("輸入欲旋轉的X軸角度:[-90 ~ 90]");
    while (Serial.available() == 0) {
    }
    Desired_Theta1 = Serial.parseFloat();
    Serial.println("");
    Serial.print("Desired_Theta1=");
    Serial.println(Desired_Theta1);
    Serial.println("");
    Serial.println("輸入欲旋轉的Y軸角度:[-180 ~ 180]");
    while (Serial.available() == 0) {
    }
    Desired_Theta2 = Serial.parseFloat();
    Serial.println("");
    Serial.print("Desired_Theta2=");
    Serial.println(Desired_Theta2);
    Serial.println("");
    Serial.println("");
    Shifting_Type_Select();  // @@移動到終點的模式
}

// @[p]座標模式(反向運動學)
void Position_mode() {
    Serial.println("");
    Serial.println("輸入欲旋轉的X座標:[0 ~ 200]");
    while (Serial.available() == 0) {
    }
    X = Serial.parseFloat();
    Serial.println("");
    Serial.print("X = ");
    Serial.println(X);
    Serial.println("");
    Serial.println("輸入欲旋轉的Y座標:[-200 ~ 200]");
    while (Serial.available() == 0) {
    }
    Y = Serial.parseFloat();
    Serial.println("");
    Serial.print("Y = ");
    Serial.println(Y);
    Serial.println("");

    // Calculate the inverse kinematics solution
    Theta21 = acos((X * X + Y * Y - a1 * a1 - a2 * a2) / (2 * a1 * a2));
    Theta11 = atan2(Y, X) - atan2(a2 * sin(Theta21), (a1 + a2 * cos(Theta21)));
    Theta21 = Theta21 * RAD_TO_DEG;
    Theta11 = Theta11 * RAD_TO_DEG;

    Theta22 = -acos((X * X + Y * Y - a1 * a1 - a2 * a2) / (2 * a1 * a2));
    Theta12 = atan2(Y, X) - atan2(a2 * sin(Theta22), (a1 + a2 * cos(Theta22)));
    Theta22 = Theta22 * RAD_TO_DEG;
    Theta12 = Theta12 * RAD_TO_DEG;
    Serial.println("[Current]");
    Serial.print("Theta1=");
    Serial.println(Theta1);
    Serial.print("Theta2=");
    Serial.println(Theta2);
    Serial.println("[SOL]");
    Serial.print("Theta11:");
    Serial.print(Theta11);
    Serial.print(", ");
    Serial.print("Theta21:");
    Serial.println(Theta21);
    Serial.print("Theta12:");
    Serial.print(Theta12);
    Serial.print(", ");
    Serial.print("Theta22:");
    Serial.println(Theta22);
    if (abs(Theta11) <= 90 && abs(Theta21) <= 180 && abs(Theta12) <= 90 && abs(Theta22) <= 180) {
        Serial.println("=>兩組解皆在工作範圍");
        displacement1 = abs(Theta11 - Theta1) + abs(Theta21 - Theta2);
        displacement2 = abs(Theta12 - Theta1) + abs(Theta22 - Theta2);
        Serial.print("displacement1:");
        Serial.print(displacement1);
        Serial.print(", ");
        Serial.print("displacement2:");
        Serial.println(displacement2);
        if (displacement1 < displacement2) {  //使用解1的角度值
            Desired_Theta1 = Theta11;
            Desired_Theta2 = Theta21;
        } else {  //使用解2的角度值
            Desired_Theta1 = Theta12;
            Desired_Theta2 = Theta22;
        }
    } else {
        if (abs(Theta11) <= 90 && abs(Theta21) <= 180) {
            Serial.println("=>只有解1在工作範圍");
            Desired_Theta1 = Theta11;  //使用解1的角度值
            Desired_Theta2 = Theta21;
        } else if (abs(Theta12) <= 90 && abs(Theta22) <= 180) {
            Serial.println("=>只有解2在工作範圍");
            Desired_Theta1 = Theta12;  //使用解2的角度值
            Desired_Theta2 = Theta22;
        } else {
            Serial.println("=>兩組解皆不在工作範圍,維持不動");
            Serial.println("");
            Desired_Theta1 = Theta1;
            Desired_Theta2 = Theta2;  // 若兩組解皆不符合則保持原位
        }
    }
    Serial.print("Desired_Theta1 = ");
    Serial.print(Desired_Theta1);
    Serial.print(", Desired_Theta2 = ");
    Serial.println(Desired_Theta2);
    Shifting_Type_Select();  // @@移動到終點的模式
}

float Y_PID_RPM_output() {
    float kp = 3;
    float ki = 0.8;
    float kd = 0;
    float dt = 0.01;

    float error = Desired_rpm2 - rpm2;
    integral_2 = integral_2 + (error * dt);
    float derivative = (error - M2_preError) / dt;

    float Pout = kp * error;
    float Iout = ki * integral_2;
    float Dout = kd * derivative;
    float output = Pout + Iout + Dout;

    M2_preError = error;
    return output;
}
//讀取當前Y軸轉速
void Encoder2_getrpm() {
    signed long encoder_2 = megaEncoderCounter.YAxisGetCount();  // 當前Y編碼器讀值
    currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        new_reading = encoder_2;
        rpm2 = (float)(((new_reading - old_reading) / a2c2) * 0.1666667 / (interval / 1000));
        old_reading = new_reading;
        Serial.print(Desired_rpm2);
        Serial.print("\t");
        Serial.println(rpm2);
    }
}
// @[s]Y軸PID速度控制
void PID_Y_Velocity() {
    if ((limit == 0) && (index == 1)) {  // 極限保護
        Serial.println("");
        Serial.println("輸入欲控制的Y軸速度:");
        Serial.println("*運行時按[s]停止");
        while (Serial.available() == 0) {
        }
        Desired_rpm2 = Serial.parseFloat();
        while ((limit == 0) && (index == 1)) {
            if (Serial.available() > 0) {
                char _Enter = Serial.read();
                if (_Enter == 's') {  //當輸入S時，程式停止並跳出迴圈
                    break;
                }
            }
            float Y_u = Y_PID_RPM_output();
            Y_forward(Y_u);
            Encoder2_getrpm();
        }
        Y_stop();
        integral_2 = 0;
        Serial.println("");
        Serial.println("PID_Y_Velocity Stop!");
    }
}
//***********↑選擇項目中的函式↑*************

/******************************************

            Setup & Loop

******************************************/
void setup() {
    Serial.begin(9600);  //鮑率設定
    PinInit();           //引腳設定
    //編碼器歸零
    megaEncoderCounter.XAxisReset();
    megaEncoderCounter.YAxisReset();
    //開啟中斷保護
    attachInterrupt(digitalPinToInterrupt(infrared_1), Limit_Protect, CHANGE);
    attachInterrupt(digitalPinToInterrupt(infrared_2), Limit_Protect, CHANGE);
}

void loop() {
    Serial.println("");
    Serial.println("");
    Serial.println("@=====================");
    Serial.println("請選擇以下項目:");
    Serial.println("[r]X、Y軸復歸");
    Serial.println("[R]X、Y軸(同動)");
    Serial.println("[i]中斷保護設定");
    Serial.println("[a]角度模式");
    Serial.println("[p]座標模式(反向運動學)");
    Serial.println("[s]Y軸PID速度控制");
    Serial.println("");
    Serial.println("當前資訊:");
    Serial.print("limit=");
    Serial.print(limit);
    Serial.print(", ");
    Serial.print("index=");
    Serial.println(index);
    Serial.print("Theta1=");
    Serial.print(Theta1);
    Serial.print(", ");
    Serial.print("Theta2=");
    Serial.println(Theta2);
    Serial.print("X=");
    Serial.print(X);
    Serial.print(", ");
    Serial.print("Y=");
    Serial.println(Y);
    Serial.println("@=====================");
    char type_command;
    while (Serial.available() == 0) {
    }
    type_command = Serial.read();
    switch (type_command) {
        case 'r': {
            Y_Mastering();  //@原點復歸
            X_Mastering();
        } break;
        case 'R': {
            XY_Mastering();  //@原點復歸(較準)
        } break;
        case 'i': {
            Interrupt_Setting();  //@中斷保護設定
        } break;
        case 'a': {
            Angle_mode();  //@角度模式
        } break;
        case 'p': {
            Position_mode();  //@座標模式(反向運動學)
        } break;
        case 's': {
            PID_Y_Velocity();  //@Y軸速度PID
        } break;
        default: {
            Serial.println("");
            Serial.println("無此選項, 請重新輸入!");
        } break;
    }
}