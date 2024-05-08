
#include "typedef.h"

#include "encoder.h"
#include "sumo_serial.h"
#include "tire.h"

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
}

void loop() {

//  SumoSerial sumo_serial(SERIAL_BAUDRATE);
  HardwareSerial Serial2(PA3, PA2);
  Serial2.begin(9600);

  Encoder encoder;
  Tire tire;

  static u1 mode = 0;
  static u1 num = 1;

  static u1 sen_ene = 0;
  static u1 sen_bld = 0;
  static u1 sen_whl = 0;

  while(1){
//    periodic_process();
//    sumo_serial.ReadString(true);
//    Serial.println(String(encoder.GetCount()));
//    delay(100);

    // if (Serial2.available()) {
    //   digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // }

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

    //センサ値反転(対物/ブレード上下はLowアクティブなので)
    sen_ene ^= B11111111;
  //  sen_bld ^= B11111111;

    //センサ値送信(対物/ブレード上下はLowアクティブなので)
    Serial2.println("MES");
  //  Serial2.println("甲：");
    Serial2.println(String(sen_ene));
  //  Serial2.println("乙：");
    Serial2.println(String(sen_bld));
  //  Serial2.println("丙：");
    Serial2.println(String(sen_whl));
  //  Serial2.println("終端");

/*
    //敵センサのデバッグ処理
    if(sen_ene != 0xff) digitalWrite(LED3, HIGH);
    else                digitalWrite(LED3, LOW);

    //白線センサのデバッグ処理
    if(sen_whl != 0x00) digitalWrite(LED1, HIGH);
    else                digitalWrite(LED1, LOW);

    //照度センサ(ブレ上)のデバッグ処理
    if(sen_bld != 0x7f) digitalWrite(LED2, HIGH);
    else                digitalWrite(LED2, LOW);
*/
    delay(200);

    // switch(mode){
    //   case 0:
    //     if(serial_test()) mode = 1;
    //     break;

    //   case 1:
    //     periodic_process();
    //     break;
    // }
  }

}

// =====================================================
// 定期処理
// =====================================================
#define CYCLE_TIME 1
void periodic_process(void){

  static u4 prevTime = 0;
  u4 currentTime = millis();

  // 定期処理
  if((currentTime - prevTime) >= CYCLE_TIME){
    prevTime = currentTime;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  return;
}
