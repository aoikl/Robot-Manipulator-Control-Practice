#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#include <Arduino.h>
#include <MegaEncoderCounter.h>

void pinInit();

void X_forward();
void X_backward();
void Y_forward();
void Y_backward();
void X_stop();
void Y_stop();

void XReset();
void YReset();
void XRead_Theta1();
void YRead_Theta2();

void IR_Readings();
void Limit_Protect();

void X_Mastering();
void Y_Mastering();

void Angle_control(char _axis, float _angle);
// values
extern int in8;  // .h內聲明
extern int in9;  // .cpp內定義
extern int enA;
extern int in3;
extern int in4;
extern int enB;

extern int Motor1_speed;
extern int Motor2_speed;

extern float a1c1;
extern float a2c2;
extern float encorder_1;
extern float encoder_2;

extern int infrared_1;
extern int infrared_2;
extern float Theta1;
extern float Theta2;

extern bool limit;

#endif