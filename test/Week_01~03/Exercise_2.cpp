#include <Arduino.h>

#define led1 3  // pwm腳位
#define led2 5
#define led3 6
#define led4 9
#define potPin A0  // select the input pin for the potentiometer
char incomingbyte;
char com_stop;

void LED_Blink();
void LED_VR();
void LED_Breath();
void LED_VR_Speed();
void Caculation();
void Pyramid();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:

    Serial.println("    ");
    Serial.println("Please Enter Command: ");
    Serial.println("    A    =>    LED Blink");
    Serial.println("    B    =>    LED & Variable Resistor");
    Serial.println("    C    =>    LED Breath");
    Serial.println("    D    =>    LED & Variable Resistor Speed");
    Serial.println("    E    =>    Math Caculation");
    Serial.println("    F    =>    Pyramid");

    while (Serial.available() == 0) {
    }
    incomingbyte = Serial.read();

    Serial.println("    ");

    switch (incomingbyte) {
        case 'A':
            LED_Blink();
            break;
        case 'B':
            LED_VR();
            break;
        case 'C':
            LED_Breath();
            break;
        case 'D':
            LED_VR_Speed();
            break;
        case 'E':
            Caculation();
            break;
        case 'F':
            Pyramid();
            break;
        default:
            Serial.println("Invalid Command!!");
            break;
    }
}

/*==============

Function

===============*/

void LED_Blink() {
    Serial.println("LED Blink => START!!!");
    Serial.println("Enter 'S' to stop");
    while (1) {
        if (Serial.available() > 0) {  //跑LED燈指令前，判斷序列監視器(serial port)是否有輸入停止指令
            com_stop = Serial.read();
            if (com_stop == 'S') {  //當輸入S時，程式停止並跳出迴圈
                Serial.println("LED Blink => STOP!!!");
                break;
            }
        }
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        digitalWrite(led4, HIGH);
        delay(500);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        digitalWrite(led4, LOW);
        delay(500);
    }
}

void LED_VR() {
    Serial.println("LED VR => START!!!");
    Serial.println("Enter 'S' to stop");
    while (1) {
        if (Serial.available() > 0) {  //跑LED燈指令前，判斷序列監視器(serial port)是否有輸入停止指令
            com_stop = Serial.read();
            if (com_stop == 'S') {  //當輸入S時，程式停止並跳出迴圈
                Serial.println("LED VR => STOP!!!");
                break;
            }
        }

        int sensorValue = analogRead(potPin);
        Serial.println(sensorValue, DEC);

        // sensorValue = sensorValue / 4; // convert from 0-1023 to 0-255
        sensorValue = map(sensorValue, 0, 1023, 0, 255);
        analogWrite(led1, sensorValue);
        analogWrite(led2, sensorValue);
        analogWrite(led3, sensorValue);
        analogWrite(led4, sensorValue);
        delay(150);
    }
}

void LED_Breath() {
    Serial.println("LED Breath => START!!!");
    Serial.println("Enter 'S' to stop");
    while (1) {
        if (Serial.available() > 0) {  //跑LED燈指令前，判斷序列監視器(serial port)是否有輸入停止指令
            com_stop = Serial.read();
            if (com_stop == 'S') {  //當輸入S時，程式停止並跳出迴圈
                Serial.println("LED Breth => STOP!!!");
                break;
            }
        }

        for (int i = 0; i <= 255; i++) {
            analogWrite(led1, i);
            analogWrite(led2, i);
            analogWrite(led3, i);
            analogWrite(led4, i);
            delay(5);
        }

        for (int i = 255; i >= 0; i--) {
            analogWrite(led1, i);
            analogWrite(led2, i);
            analogWrite(led3, i);
            analogWrite(led4, i);
            delay(5);
        }
    }
}

void Caculation() {
    // int x, y;
    Serial.println("Enter Two Integral Number");
    while (Serial.available() == 0) {
    }
    int x = Serial.parseInt();

    while (Serial.available() == 0) {
    }
    int y = Serial.parseInt();

    int a = x + y;
    int b = x - y;
    long c = (long)x * y;
    float d = (float)x / y;
    long e = pow(x, 2);
    float f = (float)sqrt(abs(x));

    Serial.print(x);
    Serial.print(" + ");
    Serial.print(y);
    Serial.print(" = ");
    Serial.print(a);
    Serial.println(" ; ");
    Serial.print(x);
    Serial.print(" - ");
    Serial.print(y);
    Serial.print(" = ");
    Serial.print(b);
    Serial.println(" ; ");
    Serial.print(x);
    Serial.print(" * ");
    Serial.print(y);
    Serial.print(" = ");
    Serial.print(c);
    Serial.println(" ; ");
    Serial.print(x);
    Serial.print(" / ");
    Serial.print(y);
    Serial.print(" = ");
    Serial.print(d);
    Serial.println(" ; ");
    Serial.print(x);
    Serial.print(" pow ");
    Serial.print(" = ");
    Serial.print(e);
    Serial.println(" ; ");
    Serial.print(x);
    Serial.print(" sqrt ");
    Serial.print(" = ");
    Serial.print(f);
    Serial.println(" ; ");
}

void Pyramid() {
    ///////////////等待序列監視器輸入值///////////////

    Serial.println("輸入層數：");
    while (Serial.available() == 0) {
    }
    int n = Serial.parseInt();
    Serial.println(n);

    ///////////////上週的數字金字塔程式///////////////

    int numbers = 1;
    int space = n - 1;
    for (int level = 1; level <= n; level++) {  //位在第幾層
        for (int i = space; i > 0; i--) {       //空白print幾個
            Serial.print(" ");
        }
        for (int j = numbers; j > 0; j--) {  //數字print幾個
            Serial.print(level);
        }
        Serial.println(" ");
        space--;       //每往下一層，空白少一個
        numbers += 2;  //每往下一層，數字多print 2 個
    }
}

void LED_VR_Speed() {
    Serial.println("LED VR Speed => START!!!");
    Serial.println("Enter 'S' to stop");
    while (1) {
        if (Serial.available() > 0) {  //跑LED燈指令前，判斷序列監視器(serial port)是否有輸入停止指令
            com_stop = Serial.read();
            if (com_stop == 'S') {  //當輸入S時，程式停止並跳出迴圈
                Serial.println("LED VR Speed => STOP!!!");
                break;
            }
        }
        int sensorValue = analogRead(potPin);
        Serial.println(sensorValue, DEC);

        for (int i = 1; i <= 4; i++) {
            if (i == 1) {
                digitalWrite(led1, 1);
                delay(sensorValue);
                digitalWrite(led1, 0);
                delay(sensorValue);
            }
            if (i == 2) {
                digitalWrite(led2, 1);
                delay(sensorValue);
                digitalWrite(led2, 0);
                delay(sensorValue);
            }
            if (i == 3) {
                digitalWrite(led3, 1);
                delay(sensorValue);
                digitalWrite(led3, 0);
                delay(sensorValue);
            }
            if (i == 4) {
                digitalWrite(led4, 1);
                delay(sensorValue);
                digitalWrite(led4, 0);
                delay(sensorValue);
            }
        }
    }
}
