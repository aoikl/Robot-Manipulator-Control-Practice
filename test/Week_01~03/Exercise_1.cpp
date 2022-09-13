#include <Arduino.h>
void _99(int x, int y);
void caculate(int x, int y);
void pyramid_word(int n);
void pyramid(int n);

void setup() {
    Serial.begin(9600);  // BAUDRATE 鮑率
    caculate(-200, 300);
    _99(2, 3);
    pyramid(6);
    pyramid_word(15);
}

void loop() {
}
/*==============

Function

===============*/
void _99(int x, int y) {
    for (int i = x; i < 10; i++) {
        for (int j = 1; j < y; j++) {
            int a = i * j;
            Serial.print(i);
            Serial.print(" * ");
            Serial.print(j);
            Serial.print(" = ");
            Serial.print(a);
            Serial.print(" ; ");
        }
        Serial.println(" ");
    }
}

void caculate(int x, int y) {
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

void pyramid_word(int n) {
    int numbers = 1;
    int space = n - 1;
    char text[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    int index = 0;                              //印字母的引數
    for (int level = 1; level <= n; level++) {  //位在第幾層
        for (int i = space; i > 0; i--) {       //空白print幾個
            Serial.print(" ");
        }

        for (int j = numbers; j > 0; j--) {  //數字print幾個
            Serial.print(text[index]);
            Serial.print(" ");
            index++;
            if (index > 25) {
                index = 0;
            }  //若引數超過text的大小，將引數歸零從A開始重新打印
        }
        Serial.println("");
        space -= 1;    //每往下一層，空白少1個
        numbers += 1;  //每往下一層，數字多print 2 個
    }
}

void pyramid(int n) {
    int numbers = 1;
    int space = n - 1;
    for (int level = 1; level <= n; level++) {  //位在第幾層
        for (int i = space; i > 0; i--) {       //空白print幾個
            Serial.print(" ");
        }

        for (int j = numbers; j > 0; j--) {  //數字print幾個
            Serial.print(level);
        }
        Serial.println("");
        space--;       //每往下一層，空白少一個
        numbers += 2;  //每往下一層，數字多print 2 個
    }
}
