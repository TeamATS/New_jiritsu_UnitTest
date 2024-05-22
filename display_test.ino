
#include "typedef.h"

#define SERIAL_BAUDRATE 9600

#define ENEMY_SENSOR_L1  PB12
#define ENEMY_SENSOR_L2  PB13
#define ENEMY_SENSOR_L3  PB14
#define ENEMY_SENSOR_C1  PB15
#define ENEMY_SENSOR_C2  PA8
#define ENEMY_SENSOR_R3  PA9
#define ENEMY_SENSOR_R2  PA10
#define ENEMY_SENSOR_R1  PA11

#define ON_BLADE_SENSOR_R   PA12
#define ON_BLADE_SENSOR_RC   PA15
#define ON_BLADE_SENSOR_C   PB3
#define ON_BLADE_SENSOR_LC   PB8
#define ON_BLADE_SENSOR_L   PB9

#define UNDER_BLADE_SENSOR_R  PB4
#define UNDER_BLADE_SENSOR_L  PB5

#define WHITE_SENSOR_LF  PB6
#define WHITE_SENSOR_LR  PB7
#define WHITE_SENSOR_RF  PB1
#define WHITE_SENSOR_RR  PB0

#define LED1  PC15
#define LED2  PC14
#define LED3  PC13

HardwareSerial Serial2(PA3, PA2);

void setup() {

  //ENEMY_SENSOR
  pinMode(ENEMY_SENSOR_L1, INPUT);
  pinMode(ENEMY_SENSOR_L2, INPUT);
  pinMode(ENEMY_SENSOR_L3, INPUT);
  pinMode(ENEMY_SENSOR_C1, INPUT);
  pinMode(ENEMY_SENSOR_C2, INPUT);
  pinMode(ENEMY_SENSOR_R3, INPUT);
  pinMode(ENEMY_SENSOR_R2, INPUT);
  pinMode(ENEMY_SENSOR_R1, INPUT);

  //ON_BLADE_SENSOR
  pinMode(ON_BLADE_SENSOR_R, INPUT);
  pinMode(ON_BLADE_SENSOR_RC, INPUT);
  pinMode(ON_BLADE_SENSOR_C, INPUT);
  pinMode(ON_BLADE_SENSOR_LC, INPUT);
  pinMode(ON_BLADE_SENSOR_L, INPUT);

  //UNDER_BLADE_SENSOR
  pinMode(UNDER_BLADE_SENSOR_R, INPUT);
  pinMode(UNDER_BLADE_SENSOR_L, INPUT);

  //WHITE_SENSOR
  pinMode(WHITE_SENSOR_LF, INPUT);
  pinMode(WHITE_SENSOR_LR, INPUT);
  pinMode(WHITE_SENSOR_RF, INPUT);
  pinMode(WHITE_SENSOR_RR, INPUT);

  //LED
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // serial setting
  Serial.begin(SERIAL_BAUDRATE);   // usb
  Serial2.begin(9600);

}

void loop() {


  static u1 mode = 0;
  static u1 num = 1;

  static u1 sen_ene = 0;
  static u1 sen_bld = 0;
  static u1 sen_whl = 0;

  String rx_str1;
  String rx_str2;
  String rx_str3;
  String rx_str4;

  int num_state2;
  int num_state3;
  int num_state4;

  while(1){
    //センサ値取得
    sen_ene = (digitalRead(ENEMY_SENSOR_L1) << 7) | (digitalRead(ENEMY_SENSOR_L2) << 6)
          |(digitalRead(ENEMY_SENSOR_L3) << 5) | (digitalRead(ENEMY_SENSOR_C1) << 4)
          |(digitalRead(ENEMY_SENSOR_C2) << 3) | (digitalRead(ENEMY_SENSOR_R3) << 2)
          |(digitalRead(ENEMY_SENSOR_R2) << 1) | (digitalRead(ENEMY_SENSOR_R1) << 0);

    sen_bld = (digitalRead(UNDER_BLADE_SENSOR_L) << 6)
          |(digitalRead(UNDER_BLADE_SENSOR_R) << 5) | (digitalRead(ON_BLADE_SENSOR_L) << 4)
          |(digitalRead(ON_BLADE_SENSOR_LC) << 3) | (digitalRead(ON_BLADE_SENSOR_C) << 2)
          |(digitalRead(ON_BLADE_SENSOR_RC) << 1) | (digitalRead(ON_BLADE_SENSOR_R) << 0);
  
    sen_whl = (digitalRead(WHITE_SENSOR_RR) << 3) | (digitalRead(WHITE_SENSOR_RF) << 2)
          |(digitalRead(WHITE_SENSOR_LR) << 1) | (digitalRead(WHITE_SENSOR_LF) << 0);

    //センサ値反転(対物はLowアクティブなので)
    sen_ene ^= B11111111;

    //センサ値送信
    Serial2.println("M");
  //  Serial2.println("甲：");
     Serial2.println(String(sen_ene));
  //  Serial2.println("乙：");
    Serial2.println(String(sen_bld));
  //  Serial2.println("丙：");
     Serial2.println(String(sen_whl));
  //  Serial2.println("終端");

  if (Serial2.available()) {            // 受信データ(Serial2)があれば
      digitalWrite(PC13, HIGH); //D5点灯
      rx_str1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)

      if (rx_str1.equals("S\r") == 1) {
      digitalWrite(PC14, HIGH); //D4点灯

        rx_str2 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)
        rx_str3 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)
        rx_str4 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)

        num_state2 = rx_str2.toInt();
        num_state3 = rx_str3.toInt();
        num_state4 = rx_str3.toInt();

      }
  }

    delay(200);

  }

}


