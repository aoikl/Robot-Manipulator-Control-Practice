/*******************
LED.h

*******************/

#ifndef _LED_H__
#define _LED_H__

//導入Arduino核心頭文件
#include "Arduino.h"

class LED {
   private:
    byte pin;  //控制led使用的引腳

   public:
    LED(byte p, bool state = LOW);  //構造函數

    ~LED();  //析構函數

    byte getPin();  //獲取控制的引腳

    void on();  //打開LED

    void off();  //關閉LED

    bool getState();   //獲取LED狀態
    void disattach();  //釋放引腳與LED的綁定，使得引腳可以控制其他的東西
};

#endif