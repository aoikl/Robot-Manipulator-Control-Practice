#ifndef MANIPULATOR_H
#define MANIPULATOR_H
//導入Arduino核心頭文件
#include "Arduino.h"
class MANIPULATOR {
   public:
    //建構子、解構子
    MANIPULATOR();
    ~MANIPULATOR();

    //引腳設定
    void pinInit();

    // Encoder讀值，且轉換成當前Theta角度
    void XRead_Theta1();
    void YRead_Theta2();

    // X, Y順逆轉、停止函式
    void X_forward(float X_speed);
    void X_backward(float X_speed);
    void Y_forward(float Y_speed);
    void Y_backward(float Y_speed);
    void X_stop();
    void Y_stop();

    // X, Y軸復歸
    void X_Mastering();
    void Y_Mastering();

    //中斷設定
    void Set_Interrupt();

    //角度控制
    void Angle_control();
    void Angle_Move_Seperate(float _desireTheta1, float _desireTheta2);

   private:
};

//中斷後，欲跳轉的函式
void Limit_Protect();

// 可外部存取的數值
extern const int in8;  // .h內聲明
extern const int in9;  // .cpp內定義
extern const int enA;
extern const int in3;
extern const int in4;
extern const int enB;

extern float Motor1_speed;
extern float Motor2_speed;

extern const float a1c1;
extern const float a2c2;

extern const int infrared_1;
extern const int infrared_2;
extern float Theta1;
extern float Theta2;

extern bool limit;
extern bool index;
#endif