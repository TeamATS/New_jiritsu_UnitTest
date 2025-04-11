#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secret_wifi.h"


//------ 定数設定 ------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_SCL 5
#define OLED_SDA 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//PIN設定
#define PIN_SDA 4
#define PIN_SCL 5
#define PIN_TX0 9
#define PIN_RX0 8
#define BTN1  2 //動作確認　L
#define BTN2  3 //真ん中
#define BTN3  8
#define AD_MACHINE_IDENTIFY A3
#define AD_BATTERY_VOLT  A0

#define PIN_MACHINE_AD A3
#define VERSION = 250326


WiFiClient espClient;
PubSubClient client(espClient);

//------ グローバル変数設定 ------
unsigned int answerback_deta = 0;
unsigned int answerback_deta_prev = 0;
unsigned int machine_identify = 0; //自分のマシン認識用
unsigned long num = 0;
int serial_trans1 = 0;
int serial_trans2 = 0;
int serial_trans3 = 0;

String c;            // 受信データ（文字）格納用
String rx_str1;

String rx_sak1;
String rx_sak2;
String rx_sak3;

String rx_sen1;
String rx_sen2;
String rx_sen3;

String rx_dbg1;
String rx_dbg2;
String rx_dbg3;
String rx_dbg4;
String rx_dbg5;

int sens_object[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //対物センサ　遠LL 遠L 遠R 遠RR 　近LL 近L 近R 近RR
int sens_white[4] = { 0, 0, 0, 0 };               //白線センサ　LF RF LR RR
int sens_blade_shoudo[5] = { 0, 0, 0, 0, 0 };     //ブレード上照度センサ　LL  L 中央　R 　RR
int sens_blade_sw[2] = { 0, 0 };
boolean state_sw[4] = { 0, 0, 0, 0 };
int state_SOC = 0;
float batt_volt = 0;
float batt_volt_filt = 0;
int ad_in = 0;

//display設定
int oled_init_disp = 1;
int oled_init_count = 0;

//以下のユーザモードは、初期のmode値で決める。４bitのスイッチ
int user_mode = 0;  //0---debug . 1---闘魂M .  2---影武者R 　3---闘魂K 　4---下田
int ui_screen = 0;
boolean ui_userselect = 0;  //ユーザの設定など条件で値を決める。 0---ユーザ未決定  1--ユーザ決定

//int WL_IDLE_STATUS=0;//250326
//int status = WL_IDLE_STATUS;     // the Wifi radio's status
int wifi_Mqtt_state = 0;  //0-- nowifi 1-- wifiok MQTT ng  2--MQTTok
int receivedValue = 0;
int receivedValue_to_sousin = 0;
int mqtt_incoming_Flag  = 0;

int answerback_incomig_Flag = 0;
int debug_signal_incomig_Flag = 0;
int sensor_signal_incomig_Flag = 0;
int Com_Err_Flag = 0;
int dekamoji_Flag = 0;
int Com_Err_Flag_naiyou = 0;

unsigned long dekamoji_cnt = 0;

boolean button_1_old = 1;
boolean button_2_old = 1;
boolean button_3_old = 1;
boolean button_1_downedge = 0;
boolean button_2_downedge = 0;
boolean button_3_downedge = 0;

//------ 関数定義 ------
void callback(char *topic, byte *payload, unsigned int length);

//--display---
void testdrawchar(uint8_t offset = 0) {
  display.clearDisplay();
  display.display();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font
  for (uint8_t i = 0; i < 170; i++)
    display.write((uint8_t)((i + offset) % 26 + 65));// 65:A to 90:Z
  display.display();
}
void oled_disp_control() {

  //画面の遷移control
  if (ui_screen == 0 ) {
    if (button_1_downedge) {
      ui_screen = 6;
    }
    else if (button_3_downedge) {
      ui_screen = 1;
    }
  } else   if (ui_screen == 1 ) {
    if (button_1_downedge) {
      ui_screen = 6;
    }
    else if (button_3_downedge) {
      ui_screen = 2;
    }
  } else   if (ui_screen == 2 ) {
    if (button_1_downedge) {
      ui_screen = 1;
    }
    else if (button_3_downedge) {
      ui_screen = 3;
    }
  } else   if (ui_screen == 3 ) {
    if (button_1_downedge) {
      ui_screen = 2;
    }
    else if (button_3_downedge) {
      ui_screen = 4;
    }
  } else   if (ui_screen == 4 ) {
    if (button_1_downedge) {
      ui_screen = 3;
    }
    else if (button_3_downedge) {
      ui_screen = 5;
    }
  } else   if (ui_screen == 5 ) {
    if (button_1_downedge) {
      ui_screen = 4;
    }
    else if (button_3_downedge) {
      ui_screen = 6;
    }
  } else   if (ui_screen == 6 ) {
    if (button_1_downedge) {
      ui_screen = 5;
    }
    else if (button_3_downedge) {
      ui_screen = 1;
    }
  }

  switch (ui_screen) {
    case 0://電源投入後の画面
      oled_stop_state();
      break;
    case 1://待機状態の画面
      oled_wait_state();
      break;
    case 2://センサ表示画面
      oled_disp_robo_sens_only();
      break;
    case 3://作戦選択の画面
      oled_sakusen_input();
      break;
    case 5://GO指示の画面
      oled_go_manual_sw();
      break;
    case 6://ユーザ選択の画面
      oled_machine_name();
      break;
    case 80://GO状態の画面
      oled_go_state();
      break;
    case 90://STOP状態の画面
      oled_stop_state();
      break;
  }

  ;
}
//-----------------------
void oled_sakusen_input() {
  display.clearDisplay();  // 表示クリア
  display.setTextSize(1);  // 文字サイズ（1）
  display.setCursor(0, 0);              // 表示開始位置左上角（X,Y）
  display.printf("manual sakusen setting");  // SOC表示
  display.setTextSize(2);  // 文字サイズ（1）
  display.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
  display.setCursor(4, 10);              // 表示開始位置左上角（X,Y）
  display.printf("000000\n");  // SOC表示
  display.display();  // 表示実行 
}
//-----------------------
void oled_go_manual_sw() {
  display.clearDisplay();  // 表示クリア
  display.setTextSize(1);  // 文字サイズ（1）
  display.setCursor(0, 0);              // 表示開始位置左上角（X,Y）
  display.printf("manual GO setting");  // SOC表示
  display.setTextSize(3);  // 文字サイズ（1）
  display.setCursor(0, 10);              // 表示開始位置左上角（X,Y）
  display.printf("  wait\n GO");  // SOC表示

  display.display();  // 表示実行 
}

void oled_wait_state(){
  display.clearDisplay();  // 表示クリア
  display.setTextSize(4);  // 文字サイズ（1）
  display.setCursor(2 , 30);              // 表示開始位置左上角（X,Y）
  display.printf("%02d%%\n", state_SOC);  // SOC表示

    //MQTT 通信情報表示
    if (wifi_Mqtt_state == 0) {
      display.setTextSize(2);     // 文字サイズ（2）
      display.printf("NG");       // SOC表示
      display.setTextSize(1);     // 文字サイズ（2）
      display.setCursor(90, 52);  // 表示開始位置左上角（X,Y）
      display.printf("WIFI");     // SOC表示
    } else if (wifi_Mqtt_state == 1) {
      display.setTextSize(2);  // 文字サイズ（2）
      display.printf("NG");
      display.setTextSize(1);     // 文字サイズ（2）
      display.setCursor(90, 52);  // 表示開始位置左上角（X,Y）
      display.printf("MQTT");     // SOC表示
    } else if (wifi_Mqtt_state == 3) {
      display.setTextSize(1);                 // 文字サイズ（2）
      display.setCursor(90, 42);              // 表示開始位置左上角（X,Y）
      display.printf("%02d", answerback_deta);  // SOC表示
    }

  display.display();  // 表示実行 
}
void oled_go_state() {
  display.clearDisplay();  // 表示クリア
  display.setTextSize(4);  // 文字サイズ（1）
  display.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
  display.printf("GO");       // SOC表示
  display.display();  // 表示実行
}
void oled_stop_state() {
  display.clearDisplay();  // 表示クリア
  display.setTextSize(4);  // 文字サイズ（1）
  display.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
  display.printf("STOP");       // SOC表示
  display.display();  // 表示実行
}
//-----------------------
void oled_machine_name() {
  int sensorValue ;
  int haba;
  func_machine_identify();
  display.clearDisplay();  // 表示クリア
  display.setTextSize(2);  // 文字サイズ（1）
  display.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
  display.printf(machineName);       // SOC表示

  sensorValue = analogRead(AD_MACHINE_IDENTIFY); //0~4096
  haba = (sensorValue * 60) / 4096;
  display.drawRect(1, 10, 65, 10, WHITE);
  display.fillRect(3, 12, haba, 8, WHITE);

  display.display();  // 表示実行
}
//-----------------------
void oled_disp_initial() {
  char WORD[] = "team ATS    ";

  oled_init_count++;
  /*DIPSW の表示*/
  display.clearDisplay();  // 表示クリア
  display.setTextSize(2);  // 文字サイズ（1）

  for (int i = 0; i < oled_init_count; i++) {
    display.setCursor(i * 12, 30);  // 表示開始位置左上角（X,Y）
    display.printf("%c", WORD[i]);  // カウント数
  }
  display.setTextSize(1);  // 文字サイズ（1）

  for (int i = 0; i < 11; i++) {
    //ユーザモードの表示
    display.setCursor(i * 6, 50);  // 表示開始位置左上角（X,Y）
    if (user_mode == 0) {
      //display.printf("%c", machineName0[i]);  // カウント数

    } else if (user_mode == 1) {
      // display.printf("%c", machineName1[i]);  // カウント数
    } else if (user_mode == 2) {
      // display.printf("%c", machineName2[i]);  // カウント数
    }
  }
  // user_mode=2;
  display.display();  // 表示実行

  if (oled_init_count > 11) {
    oled_init_disp = 0;
  }
}




void oled_disp_robo_sens_only() {
  display.clearDisplay();       // 表示クリア

  int L2x = 6 + 15;
  int L1x = 18 + 15;
  int C1x = 30 + 15;
  int R1x = 42 + 15;
  int R2x = 54 + 15;
  int S1y = 6 + 5;
  int S2y = 14 + 7;  //S1y+8

  //対物の位置（縦位置）
  int Tcfy = 26 + 10;
  int Tcry = 34 + 12;
  int T1y = 30 + 12;
  int T2y = 38 + 14;

  int HLx = 6;
  int HRx = 54 + 30;
  int H1y = 6;
  int H2y = 57;

  unsigned int hyoji_date;

    display.drawRect(13, 5, 65, 23, WHITE);   // 四角
    display.drawRect(13, 30, 65, 30, WHITE);  // 四角

    /*------------照度センサ----------------------*/
    display.drawCircle(L2x, S1y, 4, WHITE);  // 円
    display.drawCircle(L1x, S1y, 4, WHITE);  // 円
    display.drawCircle(C1x, S1y, 4, WHITE);  // 円
    display.drawCircle(R1x, S1y, 4, WHITE);  // 円
    display.drawCircle(R2x, S1y, 4, WHITE);  // 円

    display.drawCircle(L1x, S2y, 4, WHITE);  // 円
    display.drawCircle(R1x, S2y, 4, WHITE);  // 円

    /*------------対物センサ----------------------*/
    display.drawCircle(L2x, T1y, 4, WHITE);   // 円
    display.drawCircle(L1x, T1y, 4, WHITE);   // 円
    display.drawCircle(C1x, Tcfy, 4, WHITE);  // 円
    display.drawCircle(R1x, T1y, 4, WHITE);   // 円
    display.drawCircle(R2x, T1y, 4, WHITE);   // 円

    display.drawCircle(L2x, T2y, 4, WHITE);   // 円
    display.drawCircle(C1x, Tcry, 4, WHITE);  // 円
    display.drawCircle(R2x, T2y, 4, WHITE);   // 円

    /*-----------白線センサ-----------------------*/
    display.drawCircle(HLx, H1y, 4, WHITE);  // 円
    display.drawCircle(HRx, H1y, 4, WHITE);  // 円
    display.drawCircle(HLx, H2y, 4, WHITE);  // 円
    display.drawCircle(HRx, H2y, 4, WHITE);  // 円


    /*----------------------------------*/
    /*対物センサ*/
    /*対物センサの表示　遠距離*/
    if (sens_object[0] == 1) {
      display.fillCircle(L2x, T2y, 2, WHITE);  // L1
    }
    if (sens_object[1] == 1) {
      display.fillCircle(L2x, T1y, 2, WHITE);  //L2
    }
    if (sens_object[2] == 1) {
      display.fillCircle(L1x, T1y, 2, WHITE);  // L3
    }
    if (sens_object[3] == 1) {
      display.fillCircle(C1x, Tcfy, 2, WHITE);  //C1
    }
    if (sens_object[4] == 1) {
      display.fillCircle(C1x, Tcry, 2, WHITE);  // C2
    }
    if (sens_object[5] == 1) {
      display.fillCircle(R1x, T1y, 2, WHITE);  // R3
    }
    if (sens_object[6] == 1) {
      display.fillCircle(R2x, T1y, 2, WHITE);  // R2
    }
    if (sens_object[7] == 1) {
      display.fillCircle(R2x, T2y, 2, WHITE);  // R1
    }
    /*----------------------------------*/
    /*照度センサ*/
    if (sens_blade_shoudo[0] == 1) {
      display.fillCircle(L2x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[1] == 1) {
      display.fillCircle(L1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[2] == 1) {
      display.fillCircle(C1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[3] == 1) {
      display.fillCircle(R1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[4] == 1) {
      display.fillCircle(R2x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    /*----------------------------------*/
    /*ブレード上スイッチ*/
    if (sens_blade_sw[0] == 1) {
      display.fillCircle(L1x, S2y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_sw[1] == 1) {
      display.fillCircle(R1x, S2y, 2, WHITE);  // 円（塗り潰し）
    }

    /*----------------------------------*/
    /*白線センサ*/
    if (sens_white[0] == 1) {
      display.fillCircle(HLx, H1y, 2, WHITE);  // FL
    }
    if (sens_white[2] == 1) {
      display.fillCircle(HRx, H1y, 2, WHITE);  // FR
    }
    if (sens_white[1] == 1) {
      display.fillCircle(HLx, H2y, 2, WHITE);  // RL
    }
    if (sens_white[3] == 1) {
      display.fillCircle(HRx, H2y, 2, WHITE);  // RR
    }
    /*----------------------------------*/



    //SOC表示
    display.setTextSize(2);                 // 文字サイズ（2）
    display.setCursor(90, 16);              // 表示開始位置左上角（X,Y）
    display.printf("%02d%%\n", state_SOC);  // SOC表示

    display.setCursor(90, 35);  // 表示開始位置左上角（X,Y）


  display.display();  // 表示実行
}


void oled_disp_robo_sens_ver3() {
  display.clearDisplay();       // 表示クリア
  int SW1 = digitalRead(BTN1);  // スイッチの状態を判定
  int SW2 = digitalRead(BTN2);  // スイッチの状態を判定
  int SW3 = digitalRead(BTN3);  // スイッチの状態を判定

  int L2x = 6 + 15;
  int L1x = 18 + 15;
  int C1x = 30 + 15;
  int R1x = 42 + 15;
  int R2x = 54 + 15;
  int S1y = 6 + 5;
  int S2y = 14 + 7;  //S1y+8

  //対物の位置（縦位置）
  int Tcfy = 26 + 10;
  int Tcry = 34 + 12;
  int T1y = 30 + 12;
  int T2y = 38 + 14;

  int HLx = 6;
  int HRx = 54 + 30;
  int H1y = 6;
  int H2y = 57;

  unsigned int hyoji_date;

  if (dekamoji_Flag == 1) {
    display.setTextSize(3);                 // 文字サイズ（3）
    display.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
    display.printf("%02d", answerback_deta);  // SOC表示
  } else if (dekamoji_Flag == 0) {
    display.drawRect(13, 5, 65, 23, WHITE);   // 四角
    display.drawRect(13, 30, 65, 30, WHITE);  // 四角
    //display.drawRect(1, 44,  60, 20, WHITE);  // 四角


    /*------------照度センサ----------------------*/
    display.drawCircle(L2x, S1y, 4, WHITE);  // 円
    display.drawCircle(L1x, S1y, 4, WHITE);  // 円
    display.drawCircle(C1x, S1y, 4, WHITE);  // 円
    display.drawCircle(R1x, S1y, 4, WHITE);  // 円
    display.drawCircle(R2x, S1y, 4, WHITE);  // 円

    display.drawCircle(L1x, S2y, 4, WHITE);  // 円
    display.drawCircle(R1x, S2y, 4, WHITE);  // 円

    /*------------対物センサ----------------------*/
    display.drawCircle(L2x, T1y, 4, WHITE);   // 円
    display.drawCircle(L1x, T1y, 4, WHITE);   // 円
    display.drawCircle(C1x, Tcfy, 4, WHITE);  // 円
    display.drawCircle(R1x, T1y, 4, WHITE);   // 円
    display.drawCircle(R2x, T1y, 4, WHITE);   // 円

    display.drawCircle(L2x, T2y, 4, WHITE);   // 円
    display.drawCircle(C1x, Tcry, 4, WHITE);  // 円
    display.drawCircle(R2x, T2y, 4, WHITE);   // 円

    /*-----------白線センサ-----------------------*/
    display.drawCircle(HLx, H1y, 4, WHITE);  // 円
    display.drawCircle(HRx, H1y, 4, WHITE);  // 円
    display.drawCircle(HLx, H2y, 4, WHITE);  // 円
    display.drawCircle(HRx, H2y, 4, WHITE);  // 円


    /*----------------------------------*/
    /*対物センサ*/
    /*対物センサの表示　遠距離*/
    if (sens_object[0] == 1) {
      display.fillCircle(L2x, T2y, 2, WHITE);  // L1
    }
    if (sens_object[1] == 1) {
      display.fillCircle(L2x, T1y, 2, WHITE);  //L2
    }
    if (sens_object[2] == 1) {
      display.fillCircle(L1x, T1y, 2, WHITE);  // L3
    }
    if (sens_object[3] == 1) {
      display.fillCircle(C1x, Tcfy, 2, WHITE);  //C1
    }
    if (sens_object[4] == 1) {
      display.fillCircle(C1x, Tcry, 2, WHITE);  // C2
    }
    if (sens_object[5] == 1) {
      display.fillCircle(R1x, T1y, 2, WHITE);  // R3
    }
    if (sens_object[6] == 1) {
      display.fillCircle(R2x, T1y, 2, WHITE);  // R2
    }
    if (sens_object[7] == 1) {
      display.fillCircle(R2x, T2y, 2, WHITE);  // R1
    }
    /*----------------------------------*/
    /*照度センサ*/
    if (sens_blade_shoudo[0] == 1) {
      display.fillCircle(L2x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[1] == 1) {
      display.fillCircle(L1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[2] == 1) {
      display.fillCircle(C1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[3] == 1) {
      display.fillCircle(R1x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_shoudo[4] == 1) {
      display.fillCircle(R2x, S1y, 2, WHITE);  // 円（塗り潰し）
    }
    /*----------------------------------*/
    /*ブレード上スイッチ*/
    if (sens_blade_sw[0] == 1) {
      display.fillCircle(L1x, S2y, 2, WHITE);  // 円（塗り潰し）
    }
    if (sens_blade_sw[1] == 1) {
      display.fillCircle(R1x, S2y, 2, WHITE);  // 円（塗り潰し）
    }

    /*----------------------------------*/
    /*白線センサ*/
    if (sens_white[0] == 1) {
      display.fillCircle(HLx, H1y, 2, WHITE);  // FL
    }
    if (sens_white[2] == 1) {
      display.fillCircle(HRx, H1y, 2, WHITE);  // FR
    }
    if (sens_white[1] == 1) {
      display.fillCircle(HLx, H2y, 2, WHITE);  // RL
    }
    if (sens_white[3] == 1) {
      display.fillCircle(HRx, H2y, 2, WHITE);  // RR
    }
    /*----------------------------------*/


    /*DIPSW の表示*/
 //   display.setTextSize(1);                                                                // 文字サイズ（1）
 //   display.setCursor(90, 1);                                                              // 表示開始位置左上角（X,Y）
 //   display.printf("SW%1d%1d%1d%1d", state_sw[0], state_sw[1], state_sw[2], state_sw[3]);  // カウント数


    //SOC表示
    display.setTextSize(2);                 // 文字サイズ（2）
    display.setCursor(90, 16);              // 表示開始位置左上角（X,Y）
    display.printf("%02d%%\n", state_SOC);  // SOC表示

    display.setCursor(90, 35);  // 表示開始位置左上角（X,Y）

    //MQTT 通信情報表示
    if (wifi_Mqtt_state == 0) {
      display.setTextSize(2);     // 文字サイズ（2）
      display.printf("NG");       // SOC表示
      display.setTextSize(1);     // 文字サイズ（2）
      display.setCursor(90, 52);  // 表示開始位置左上角（X,Y）
      display.printf("WIFI");     // SOC表示
    } else if (wifi_Mqtt_state == 1) {
      display.setTextSize(2);  // 文字サイズ（2）
      display.printf("NG");
      display.setTextSize(1);     // 文字サイズ（2）
      display.setCursor(90, 52);  // 表示開始位置左上角（X,Y）
      display.printf("MQTT");     // SOC表示
    } else if (wifi_Mqtt_state == 3) {
      display.setTextSize(1);                 // 文字サイズ（2）
      display.setCursor(90, 42);              // 表示開始位置左上角（X,Y）
      display.printf("%02d", answerback_deta);  // SOC表示
    }
    else if (wifi_Mqtt_state == 99) {
      display.setTextSize(1);                 // 文字サイズ（2）
      display.printf("FIX");       // SOC表示
    }
  }

  display.display();  // 表示実行
}

//--シリアル通信---
void serial_comu_micom() {
  int result;
  const char arg1[] = "MES";
  String mes_check = "MES";
  int len;
  int num_state;

  bool state;
  bool state1;  //debug用
  bool state2;  //debug用
  bool state3;  //debug用
  bool state4;  //debug用
  bool state5;  //debug用
  bool state6;  //debug用
  bool state7;  //debug用
  bool state8;  //debug用

  long watch_num;
  // 受信データ処理

  if (Serial.available()) {  // 受信データ(USBシリアル)がある間繰り返し
    c = Serial.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial)
    Serial.println(c);   // 文字列出力(USBシリアル)
  }

  if (Serial0.available()) {  // 受信データ(Serial1)があれば
    rx_str1 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)

    if (rx_str1.equals("M\r") == 1) {
      //文字列比較
      //https://greenhornprofessional.hatenablog.com/entry/2020/09/13/223155

      rx_sen1 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sen2 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sen3 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)

      Serial.print("rx_sen1:");
      Serial.print(rx_sen1);
      Serial.print(",  ");
      Serial.print("rx_sen2:");
      Serial.print(rx_sen2);
      Serial.print(",  ");
      Serial.print("rx_sen3:");
      Serial.println(rx_sen3);
      Serial.println();

      while (Serial0.available()) {
        Serial0.read();
      }
      // 文字列を整数に変換する

      /*対物センサへの割り付け*/
      num_state = rx_sen1.toInt();
      for (int i = 0; i < 8; i++) {
        sens_object[7 - i] = (num_state >> i) & 1;
      }

      /*ブレード上照度センサへの割り付け*/
      num_state = rx_sen2.toInt();
      for (int i = 0; i < 5; i++) {
        sens_blade_shoudo[4 - i] = (num_state >> (i)) & 1;
      }
      /*ブレードスイッチセンサへの割り付け*/
      for (int i = 0; i < 2; i++) {
        sens_blade_sw[1 - i] = (num_state >> i + 5) & 1;
      }

      /*白線センサへの割り付け*/
      num_state = rx_sen3.toInt();
      for (int i = 0; i < 4; i++) {
        sens_white[i] = (~num_state >> i) & 1;
      }
      Serial.println(sens_white[1]);
      //sensor_signal_incomig_Flag = 1;

    }
    else if (rx_str1.equals("D\r") == 1) {
      rx_dbg1 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg2 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg3 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg4 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg5 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      while (Serial0.available()) {
        Serial0.read();
      }
      //debug_signal_incomig_Flag = 1;
      Serial.print(rx_dbg1);
      Serial.print(",");
      Serial.print(rx_dbg2);
      Serial.print(",");
      Serial.print(rx_dbg3);
      Serial.print(",");
      Serial.print(rx_dbg4);
      Serial.print(",");
      Serial.print(rx_dbg5);
      Serial.println("");
    }
    else if (rx_str1.equals("A\r") == 1) {
      rx_sak1 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sak2 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sak3 = Serial0.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      while (Serial0.available()) {
        Serial0.read();
      }


      Serial.println("BPからのアンサーバック値を表示");
      Serial.println(rx_sak1);
      Serial.println(rx_sak2);
      Serial.println(rx_sak3);
      Serial.println("\n");
      num_state = rx_sak1.toInt();
      answerback_deta = 0;
      if (num_state == serial_trans1) {
        answerback_deta = answerback_deta + num_state * 10000;
        num_state = rx_sak2.toInt();
        if (num_state == serial_trans2) {
          answerback_deta = answerback_deta + num_state * 100;
          num_state = rx_sak3.toInt();
          if (num_state == serial_trans3) {
            answerback_deta = answerback_deta + num_state;
            //mqtt_incoming_Flag = 0;
            //answerback_incomig_Flag = 1;
            //アンサーバック値がGo/Stopコマンドの場合、捨てる(dispに表示したくない)
            if (answerback_deta == 909003 || answerback_deta == 909005) {
              answerback_deta = answerback_deta_prev;
            }
            else {
              answerback_deta_prev = answerback_deta;
              //dekamoji_Flag = 1;
            }
          }
        }
      }
      //Serial.print(mqtt_incoming_Flag);
    } else {
      while (Serial0.available()) {
        Serial0.read();
      }
    }
  }
}


void setup() {
  // 本体初期化
  delay(100);

  //Serial通信用
  Serial.begin(115200);     // PC通信 通信速度
  Serial0.begin(38400);     // マイコン間通信 通信速度

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT_PULLUP);

  //wifi
  while (!Serial && millis() < 5000)
    ;

  func_machine_identify();


  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  // check for the WiFi module:
  setupWifi();

  display.clearDisplay();
  testdrawchar();
}

void loop() {
  //    Serial.println(analogRead(PIN_MACHINE_AD));
  // put your main code here, to run repeatedly:
  if (num >= 500) {
    Serial0.println("AAAA");
    num = 0;
  } else {
    num++;
  }

  delay(1);
  func_button_input();
  //  serial_comu_micom();
  // oled_disp_robo_sens_ver3();
  oled_disp_control();
}
//-----------------------------------------------
void func_button_input() {
  boolean button_1;
  boolean button_2;
  boolean button_3;

  button_1 = digitalRead(BTN1);
  button_2 = digitalRead(BTN2);
  button_3 = digitalRead(BTN3);


  if (button_1 == 1 && button_1_old == 0) {
    button_1_downedge = 1;
    Serial.printf("1down");
  } else {
    button_1_downedge = 0;
  }
  if (button_2 == 1 && button_2_old == 0) {
    button_2_downedge = 1;
    Serial.printf("2down");
  } else {
    button_2_downedge = 0;
  }

  if (button_3 == 1 && button_3_old == 0) {
    button_3_downedge = 1;
    Serial.printf("3down");
  } else {
    button_3_downedge = 0;
  }

  button_1_old = button_1;
  button_2_old = button_2;
  button_3_old = button_3;
}
//------------------------------------------------
void func_machine_identify()
{
  int sensorValue ;

  //0---debug . 1---闘魂M .  2---影武者R 　3---闘魂K 　4---下田
  //マシンの識別(常時しない。)
  Serial.printf("\n\n<マシン認識>\n");
  sensorValue = analogRead(AD_MACHINE_IDENTIFY); //0~4096
  Serial.printf("(AD値:%d)", sensorValue);
  if (sensorValue < 1024) {
    user_mode = 0;
    Serial.printf("デバッグ用機体\n");
  } else  if (sensorValue < 2048) {
    user_mode = 1; //闘魂M
    Serial.printf("闘魂 M\n");
  } else if (sensorValue < 3072) {
    user_mode  = 2; //影武者R
    Serial.printf("影武者 R\n");
  } else {
    user_mode  = 0; //debug用
    Serial.printf("デバッグ用機体\n");
  }

  switch (user_mode) {
    case 0:
      ssid = ssid0;
      password = password0;
      mqtt_server = mqtt_server1;
      PubTopic = PubTopic_1;

      machineName = machineName_1;
      break;
    case 1:
      ssid = ssid_matsu;
      password = password_matsu;
      mqtt_server = mqtt_server_matsu;
      PubTopic = PubTopic_matsu;
      machineName = machineName_matsu;
      break;
    case 2:
      ssid = ssid_koza;
      password = password_koza;
      mqtt_server = mqtt_server_koza;
      PubTopic = PubTopic_koza;
      machineName = machineName_koza;

      break;
    case 3:
      ssid = ssid_kana;
      password = password_kana;
      mqtt_server = mqtt_server_kana;
      machineName = machineName_kana;
      break;
    case 4:
      ssid = ssid_shimo;
      password = password_shimo;
      mqtt_server = mqtt_server_shimo;
      machineName = machineName_shimo;
      break;
  }

  Serial.printf("user_mode");
  Serial.printf("%d", user_mode);
  Serial.printf(ssid);
}

//------------------------------------------------
void setupWifi()
{
  int wifi_NG_counter = 0;

  delay(10);

  Serial.printf("\nStart WiFiMQTTSecure on \n");

  WiFi.mode(WIFI_STA);        // Set the mode to WiFi station mode.
  WiFi.begin(ssid, password); // Start Wifi connection.
  Serial.print("4 ");
  WiFi.setTxPower(WIFI_POWER_18_5dBm); // 送信出力値を設定

  while (WiFi.status() != WL_CONNECTED && wifi_NG_counter < 10)
  {
    delay(500);
    // M5.Lcd.print(".");
    Serial.print(". ");
    wifi_NG_counter++;
  }

  if (wifi_NG_counter >= 10) {
    Serial.println("wifi_ng");
    wifi_Mqtt_state = 0;
  } else {
    wifi_Mqtt_state = 1;
    Serial.println("wifi_ok");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback); // Sets the message callback function.  设置消息回调函数
    Serial.printf("mq_setup is ok.\n");
    wifi_Mqtt_state = 2;
  }
}

//------------------------------------------------

void callback(char *topic, byte *payload, unsigned int length)
{
  //canvasMQTT.fillSprite(TFT_GREEN);
  //canvasMQTT.setCursor(20, 20);
  //canvasMQTT.print("Message arrived [");
  //canvasMQTT.print(topic);
  //canvasMQTT.print("] ");
  for (int i = 0; i < length; i++)
  {
    //canvasMQTT.print((char)payload[i]);
  }
  //canvasMQTT.println();
  //canvasMQTT.pushSprite(&canvas1, 0, 0);
  //canvas1.pushSprite(&display, 100, 100);
}
