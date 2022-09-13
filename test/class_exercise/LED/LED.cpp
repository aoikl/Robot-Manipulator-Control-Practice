/*****************
LED.cpp

******************/

#include "LED.h"
#include "Arduino.h"

LED::LED(byte p, bool state)
    : pin(p) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
}

LED::~LED() {
    disattach();
}

void LED::on() {
    digitalWrite(pin, HIGH);
}

void LED::off() {
    digitalWrite(pin, LOW);
}

bool LED::getState() {
    return digitalRead(pin);
}

void LED::disattach()  //引腳回收，恢復到上電狀態
{
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
}