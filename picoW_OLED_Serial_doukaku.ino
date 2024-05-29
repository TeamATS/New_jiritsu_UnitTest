#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // 別途「Adafruit BusIO」ライブラリ必要
#include "stdlib.h"
//#include <stdio.h>
//#include <string.h>
/*環境：Arduino ide
   [board設定]
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   上を設定にコピペし、ボードマネージャでpicoで検索。

   接続：かっこ内はピン番号,GNDは省略
   GP20(26):  I2C0 SDA (OLEDと接続)
   GP21(27):  I2C0 SCL (OLEDと接続)
   GP16(21):  UART0 TX (メインマイコンのRXと接続)
   GP17(22):  UART0 RX (メインマイコンのTXと接続)
   GP6(9):  デジタルイン(プルアップ設定)　緊急時入力１
   GP7(10): デジタルイン(プルアップ設定)　緊急時入力２
   GP10(14): デジタルイン(プルアップ設定) 　緊急時入力３
   GP11(15): デジタルイン（プルアップ設定）　緊急時入力４
   GP26(31): アナログイン　BAT電圧モニタ
   3.3V(36): I2C用電源供給  (OLEDと接続)
*/

// OLED設定
#define SCREEN_WIDTH 128  // OLED 幅指定
#define SCREEN_HEIGHT 64  // OLED 高さ指定（高さ32のものを使用する場合は32）
#define OLED_RESET -1     // リセット端子（未使用-1）

//PIN設定
#define PIN_SDA  20
#define PIN_SCL  21
#define PIN_TX0  16
#define PIN_RX0  17
#define BTN1  6
#define BTN2  7
#define BTN3  10
#define BTN4  11
#define BAT_V 26

//音程を定義する
#define C5 523 //ド
#define Cs5 554 //ド＃
#define D5 587 //レ
#define Ds5 622 //レ＃
#define E5 659 //ミ
#define F5 698 //ファ
#define Fs5 740 //ファ＃
#define G5 784 //ソ
#define Gs5 831 //ソ
#define A5 880  //La
#define As5 932 //La＃
#define B5 988 //Si＃

#define C4 262 //ド
#define Cs4 277 //ド＃
#define D4 294 //レ
#define Ds4 311 //レ＃
#define E4 330 //ミ
#define F4 349 //ファ
#define Fs4 370 //ファ＃
#define G4 392 //ソ
#define Gs4 415 //ソ＃
#define A4 440 //ラ
#define As4 466 //ラ
#define B4 494 //シ

//音符に対してのDelay Time(msec)を設定
#define Dt2 1100
#define Dt4 500
#define Dt8f 350
#define Dt8 200
#define Dt16 50

//音の長さ(msec)を定義する(BPM=100)
#define Bt2 1200 //2分音符
#define Bt4 500 //4分音符、スタッカート気味
#define Bt8f 450 //付点8分音符
#define Bt8 300 //8分音符
#define Bt16 150 //16分音符


#define VERSION = 240510
// I2Cに接続されたSSD1306用「display」の宣言
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char buffer_text[33];

// 変数宣言
int cnt = 0;        // ボタンを押した回数カウント用
bool state = false; // ボタン状態保持用
String c; // 受信データ（文字）格納用
String rx_str1;
String rx_str2;
String rx_str3;

typedef struct {
  int pitch; //音の高さ
  int delta_t;
} merody_data;

merody_data Doremi[] {
  {C4, Bt16},
  { -1, Dt16},
  {C4, Bt16},
  { -1, Dt16},
  {C4, Bt16},
  { -1, Dt16},
  {D4, Bt16},
  { -1, Dt16},
  {E4, Bt16},
  { -1, Dt16},
  {F4, Bt16},
  { -1, Dt16},
  {G4, Bt16},
  { -1, Dt16},
  {A4, Bt16},
  { -1, Dt16},
  {B4, Bt16},
  { -1, Dt16},
  {C5, Bt16},
  { -1, Dt16}
};

merody_data DQ_LVUP[] {
  {G4, Bt16},
  { -1, Dt16},
  {G4, Bt16},
  { -1, Dt16},
  {G4, Bt4},
  { -1, Dt4},
  {G4, Bt4},
  { -1, Dt4},
  {F4, Bt4},
  { -1, Dt4},
  {A4, Bt4},
  { -1, Dt4},
  {G4, Bt4},
  { -1, Dt4},
  { -2, 0}
};

merody_data starWars_DV[] {
  //https://numbersdrown.com/arduino-pitch/
  {G4, Bt4},
  { -1, Dt4},
  {G4, Bt4},
  { -1, Dt4},
  {G4, Bt4},
  { -1, Dt4},
  {Ds4, Bt8f},
  { -1, Dt8f},
  {As4, Bt8},
  { -1, Dt8f},
  {G4, Bt4},
  { -1, Dt4},
  {Ds4, Bt8f},
  { -1, Dt8f},
  {As4, Bt8},
  { -1, Dt8},
  {G4, Bt2},
  { -1, Dt2},
  { -2, 0}
};


int counter = 0;
int counter_ad = 0;
int sens_object[8] = { 0, 0, 0, 0,  0, 0, 0, 0};//対物センサ　遠LL 遠L 遠R 遠RR 　近LL 近L 近R 近RR
int sens_white[4] = { 0, 0, 0, 0};//白線センサ　LF RF LR RR
int sens_blade_shoudo[5] = { 0, 0, 0, 0, 0}; //ブレード上照度センサ　LL  L 中央　R 　RR
int sens_blade_sw[2] = { 0, 0};
int state_sw[4] = {0, 0, 0, 0};
int state_SOC = 0;
float batt_volt = 0;
float batt_volt_filt = 0;
int ad_in = 0;

int counter_1 = 0;
int counter_2 = 0;
int counter_3 = 0;
int counter_4 = 0;
int counter_5 = 0;
int counter_6 = 0;
int counter_6_1=0;
int oled_init_disp=1;
int oled_init_count=0;
int merody_id = 0;
int  merody_sum = 0;
int  merody_end = 0;

int sound_idx_old = -1;

struct repeating_timer st_timer; //割り込み用
bool timer_flag = false;

const int BUZZER_PIN = 1;
// 関数 ----------------------------------------------------------------------
void led() {  // LED点灯
  digitalWrite(LED_BUILTIN, HIGH); // LEDを点灯
  delay(200);             // 遅延時間
  digitalWrite(LED_BUILTIN, LOW);  // LEDを消灯
  delay(200);             // 遅延時間
}

/* タイマー割り込み処理 */
bool Timer(struct repeating_timer *t) {
  timer_flag = true;

  counter++;
  counter_1++;
  counter_2++;
  counter_3++;
  counter_4++;
  counter_6++;
  return true;

}


void sequence_sound_ver2(merody_data m_d[] , int counter_num) {
  // 毎回積分の計算をするのは、勿体無い。
  // N番目とそこまでの積算値をglobal変数で記憶しておき、
  // その値を超過したかどうかで判定する。
  // 超過時にN番目をN+1に上書き、積算値も上書き、音処理も実施する。
  // global変数は、
  // merody_id
  // merody_sum
  int koushin_flag = 0;
  if (merody_end == 0) {
    while (counter_num > merody_sum) {
      merody_id++;
      merody_sum = merody_sum + m_d[merody_id].delta_t;
      koushin_flag = 1;
    }

    if (koushin_flag == 1) {
      //音処理の実施
      if (m_d[merody_id].pitch == -1) {
        noTone(BUZZER_PIN);
      } else if (m_d[merody_id].pitch == -2) {
        merody_end = 1;
        noTone(BUZZER_PIN);
        //        digitalWrite(BUZZER_PIN,0);
      } else {
        tone(BUZZER_PIN, m_d[merody_id].pitch, m_d[merody_id].delta_t); /*ド(C5)*/
      }
    } else {
      //何もしない
    }
  }
}


void sequence_sound(merody_data m_d[] , int counter_num) {
  int flag_num;
  int cumsum = 0;
  for (int x = 0; x < 20; x++) {
    cumsum = cumsum + m_d[x].delta_t;
    if (counter_num > cumsum) {
      flag_num = x;
    }
  }

  if (sound_idx_old != flag_num) {
    sound_idx_old = flag_num;
    if (m_d[flag_num].pitch == -1) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, m_d[flag_num].pitch, m_d[flag_num].delta_t); /*ド(C5)*/
    }
  }
}

void oled_disp_initial(){
  char WORD[]="team ATS    ";

 oled_init_count++;
  /*DIPSW の表示*/
  display.clearDisplay();     // 表示クリア
  display.setTextSize(2);     // 文字サイズ（1）

  for(int i=0 ; i<oled_init_count;i++){
  display.setCursor(i*12, 30);       // 表示開始位置左上角（X,Y）
  display.printf("%c", WORD[i]); // カウント数
  }
  display.display();  // 表示実行

  if(oled_init_count>11){
     oled_init_disp=0;
  }
  
}

void oled_disp_robo_sens_ver3() {
  display.clearDisplay();     // 表示クリア
  int SW1 = digitalRead(BTN1);    // スイッチの状態を判定
  int SW2 = digitalRead(BTN2);    // スイッチの状態を判定
  int SW3 = digitalRead(BTN3);    // スイッチの状態を判定

  int L2x = 6 + 15;
  int L1x = 18 + 15;
  int C1x = 30 + 15;
  int R1x = 42 + 15;
  int R2x = 54 + 15;
  int S1y = 6 + 5;
  int S2y = 14 + 7; //S1y+8

  //対物の位置（縦位置）
  int Tcfy = 26 + 10;
  int Tcry = 34 + 12;
  int T1y = 30 + 12;
  int T2y = 38 + 14;

  int HLx = 6;
  int HRx = 54 + 30;
  int H1y = 6;
  int H2y = 57;

  display.drawRect(13, 5, 65, 23, WHITE);   // 四角
  display.drawRect(13, 30,  65, 30, WHITE);  // 四角
  //display.drawRect(1, 44,  60, 20, WHITE);  // 四角


  /*------------照度センサ----------------------*/
  display.drawCircle(L2x, S1y, 4, WHITE);      // 円
  display.drawCircle(L1x, S1y, 4, WHITE);      // 円
  display.drawCircle(C1x, S1y, 4, WHITE);      // 円
  display.drawCircle(R1x, S1y, 4, WHITE);      // 円
  display.drawCircle(R2x, S1y, 4, WHITE);      // 円

  display.drawCircle(L1x, S2y, 4, WHITE);      // 円
  display.drawCircle(R1x, S2y, 4, WHITE);      // 円

  /*------------対物センサ----------------------*/
  display.drawCircle(L2x, T1y, 4, WHITE);      // 円
  display.drawCircle(L1x, T1y, 4, WHITE);      // 円
  display.drawCircle(C1x, Tcfy, 4, WHITE);      // 円
  display.drawCircle(R1x, T1y, 4, WHITE);      // 円
  display.drawCircle(R2x, T1y, 4, WHITE);      // 円

  display.drawCircle(L2x, T2y, 4, WHITE);      // 円
  display.drawCircle(C1x, Tcry, 4, WHITE);      // 円
  display.drawCircle(R2x, T2y, 4, WHITE);      // 円

  /*-----------白線センサ-----------------------*/
  display.drawCircle(HLx, H1y, 4, WHITE);      // 円
  display.drawCircle(HRx, H1y, 4, WHITE);      // 円
  display.drawCircle(HLx, H2y, 4, WHITE);      // 円
  display.drawCircle(HRx, H2y, 4, WHITE);      // 円


  /*----------------------------------*/
  /*対物センサ*/
  /*対物センサの表示　遠距離*/
  if (sens_object[0] == 1) {
    display.fillCircle(L2x, T2y, 2, WHITE); // L1
  }
  if (sens_object[1] == 1) {
    display.fillCircle(L2x, T1y, 2, WHITE); //L2
  }
  if (sens_object[2] == 1) {
    display.fillCircle(L1x, T1y,  2, WHITE);  // L3
  }
  if (sens_object[3] == 1) {
    display.fillCircle(C1x, Tcfy, 2, WHITE);   //C1
  }
  if (sens_object[4] == 1) {
    display.fillCircle(C1x, Tcry, 2, WHITE);   // C2
  }
  if (sens_object[5] == 1) {
    display.fillCircle(R1x, T1y,  2, WHITE);  // R3
  }
  if (sens_object[6] == 1) {
    display.fillCircle(R2x, T1y, 2, WHITE);   // R2
  }
  if (sens_object[7] == 1) {
    display.fillCircle(R2x, T2y,  2, WHITE);  // R1
  }
  /*----------------------------------*/
  /*照度センサ*/
  if (sens_blade_shoudo[0] == 1) {
    display.fillCircle(L2x, S1y, 2, WHITE);      // 円（塗り潰し）
  }
  if (sens_blade_shoudo[1] == 1) {
    display.fillCircle(L1x, S1y, 2, WHITE);      // 円（塗り潰し）
  }
  if (sens_blade_shoudo[2] == 1) {
    display.fillCircle(C1x, S1y, 2, WHITE);      // 円（塗り潰し）
  }
  if (sens_blade_shoudo[3] == 1) {
    display.fillCircle(R1x, S1y, 2, WHITE);      // 円（塗り潰し）
  }
  if (sens_blade_shoudo[4] == 1) {
    display.fillCircle(R2x, S1y, 2, WHITE);      // 円（塗り潰し）
  }
  /*----------------------------------*/
  /*ブレード上スイッチ*/
  if (sens_blade_sw[0] == 1) {
    display.fillCircle(L1x, S2y, 2, WHITE);      // 円（塗り潰し）
  }
  if (sens_blade_sw[1] == 1) {
    display.fillCircle(R1x, S2y, 2, WHITE);      // 円（塗り潰し）
  }

  /*----------------------------------*/
  /*白線センサ*/
  if (sens_white[0] == 1) {
    display.fillCircle(HLx, H1y, 2, WHITE);      // FL
  }
  if (sens_white[2] == 1) {
    display.fillCircle(HRx, H1y,  2, WHITE);  // FR
  }
  if (sens_white[1] == 1) {
    display.fillCircle(HLx, H2y, 2, WHITE);      // RL
  }
  if (sens_white[3] == 1) {
    display.fillCircle(HRx, H2y,  2, WHITE);  // RR
  }
  /*----------------------------------*/


  /*DIPSW の表示*/
  display.setTextSize(1);     // 文字サイズ（1）
  display.setCursor(90, 1);       // 表示開始位置左上角（X,Y）
  display.printf("SW%1d%1d%1d%1d", state_sw[0], state_sw[1], state_sw[2], state_sw[3]); // カウント数


  //SOC表示
  display.setTextSize(2);     // 文字サイズ（2）
  display.setCursor(90, 16);       // 表示開始位置左上角（X,Y）
  display.printf("%02d%%\n", state_SOC);  // SOC表示

  //  display.setCursor(90, 32);       // 表示開始位置左上角（X,Y）
  //  display.printf("%02d\n", ad_in);  // カウント数
  //  display.setCursor(90, 42);       // 表示開始位置左上角（X,Y）
  //  display.printf("%.1fV\n", batt_volt);  // カウント数

  display.display();  // 表示実行


}




void serial_comu_test_ver2() {

  String rx_str4;
  String rx_str5;
  String rx_str6;


  if (Serial1.available()) {            // 受信データ(Serial2)があれば
    Serial.println("Serial通信受信(ver2)!!");
    rx_str4 = Serial1.readStringUntil('\n');
    //teststr.trim();
    Serial.print(rx_str4);
    Serial.println(rx_str4.compareTo("M\r"));
    Serial.println(rx_str4.length());
    if (rx_str4.compareTo("M") == 13) {
      Serial.println("");
      Serial.println(rx_str4.length());
      Serial.println("Bingo");
      Serial.println("");

    }

  }
  /*
    0  (28)

    L 1 (76)
    M 13
    N -1 (78)
    O -2 (79)
  */
}

void serial_comu_soushin_test(){
int result;
  counter_6_1++;

 if(counter_6_1>9){
  counter_6_1=0;
 }
  //Txに出力
  Serial1.println("S");
  Serial1.println("0");
  Serial1.println("0");
  Serial1.println(String(counter_6_1));
  //シリアルコンソールに出力
  Serial.print("送信");
  Serial.println(counter_6_1);
}

void serial_comu_test() {
  int result;
  const char arg1[] = "MES";
  String mes_check = "MES";
  int len ;
  int num_state;
  String rx_str4;
  String rx_str5;
  String rx_str6;
  bool state;
  bool state1; //debug用
  bool state2; //debug用
  bool state3; //debug用
  bool state4; //debug用
  bool state5; //debug用
  bool state6; //debug用
  bool state7; //debug用
  bool state8; //debug用

  long watch_num;
  // 受信データ処理

  if (Serial.available()) {             // 受信データ(USBシリアル)がある間繰り返し
    //Serial.println("kokomadekita0");
    c = Serial.readStringUntil('\n');   // 受信データを\nの手前まで取得(Serial)
    //led();
    Serial.println(c);                  // 文字列出力(USBシリアル)
    Serial2.println(c);                 // 文字列出力(Serial2)
  }

  if (Serial1.available()) {            // 受信データ(Serial2)があれば
    Serial.print("Serial受信!!");
    Serial.println(Serial1.available());
    rx_str1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)
        Serial.print(rx_str1);

    if (rx_str1.equals("M\r") == 1) {
      Serial.println("decode!");
      //文字列比較
      //https://greenhornprofessional.hatenablog.com/entry/2020/09/13/223155

      rx_str2 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)
      rx_str3 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)
      rx_str4 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial2)

      if (0) {
        Serial.println(rx_str1);
        Serial.println(rx_str2);
        Serial.println(rx_str3);
        Serial.println(rx_str4);

      }

      while (Serial1.available()) {
        Serial1.read();
      }
      // 文字列を整数に変換する

      /*対物センサへの割り付け*/
      num_state = rx_str2.toInt();
      for (int i = 0; i < 8; i++) {
        sens_object[7 - i] = (num_state >> i) & 1;
      }

      /*ブレード上照度センサへの割り付け*/
      num_state = rx_str3.toInt();
      for (int i = 0; i < 5; i++) {
        sens_blade_shoudo[4 - i] = (num_state >> (i )) & 1;
      }
      /*ブレードスイッチセンサへの割り付け*/
      for (int i = 0; i < 2; i++) {
        sens_blade_sw[1 - i] = (num_state >> i + 5) & 1;
      }

      /*白線センサへの割り付け*/
      num_state = rx_str4.toInt();
      for (int i = 0; i < 4; i++) {
        sens_white[ i] = (~num_state >> i) & 1;
      }
      
      

    }else{
        while (Serial1.available()) {
          Serial1.read();
        }
    }
  }
}



void button_read_demo() {
  int SW = digitalRead(BTN3);    // スイッチの状態を判定
  if (SW == HIGH) {                // "HIGH"なら”1”を送信
    //    digitalWrite(LED_BUILTIN, LOW);
    //    Serial.write('1');
    //Serial2.write('1');
    Serial2.println("1!2!50");
  } else if (SW == LOW) {          // "LOW"なら"0"を送信
    //    digitalWrite(LED_BUILTIN, HIGH);
    //    Serial.write('0');
    Serial.println(cnt);
    //Serial2.write('0');
  }
}

void button_BAT_read() {
  /*
    int SW1 = digitalRead(BTN1);    // スイッチの状態を判定
    int SW2 = digitalRead(BTN2);    // スイッチの状態を判定
    int SW3 = digitalRead(BTN3);    // スイッチの状態を判定
    int SW4 = digitalRead(BTN4);    // スイッチの状態を判定
  */
  int ad_value;
  long temp ;

  state_sw[0] = digitalRead(BTN1);
  state_sw[1] = digitalRead(BTN2);
  state_sw[2] = digitalRead(BTN3);
  state_sw[3] = digitalRead(BTN4);

  //  state_SOC
  ad_value = analogRead(BAT_V);
  ad_in = ad_value;
  batt_volt = 4.13 * ad_value / (1023 / 3.3);
  temp = 116 * ad_value - 95200;

  batt_volt_filt = ((float)temp - batt_volt_filt) / 10 + batt_volt_filt;
  if (batt_volt_filt < 0) {
    batt_volt = 0;
  } else if (batt_volt_filt > 10000) {
    batt_volt_filt = 10000;
  }


  state_SOC = (int)(batt_volt_filt / 100);
  if (state_SOC > 100) {
    state_SOC = 100;
  } else if (state_SOC < 0) {
    state_SOC = 0;
  }



  // 5V 1023
  // 0V 0
  //11.7V 58%  -- 871     y=44/38 *IN -952
  //11.1V 14%  -- 833     y= 1.16 * IN - 952;
  //分圧　 15k/(15k+47k) = 1/4.13倍

}

void tone_ongaku(int number) {
  //ファファファファ ミ♭ ソ ファー♪ レベルアップのＢＧＭです。

  switch (number) {
    case 0:
      tone(BUZZER_PIN, 262, 800); /*ド(C4)*/
      break;
    case 2:
      tone(BUZZER_PIN, 294, 800); /*レ(D4)*/
      break;
    case 3:
      tone(BUZZER_PIN, 330, 800); /*ミ(E4)*/
      break;
    case 4:
      tone(BUZZER_PIN, 349, 800); /*ファ(F4)*/
      break;
    case 5:
      tone(BUZZER_PIN, 392, 800); /*ソ(G4)*/
      break;
    case 6:
      tone(BUZZER_PIN, 440, 800); /*ラ(A4)*/
      break;
    case 7:
      tone(BUZZER_PIN, 494, 800); /*シ(B4)*/
      break;
    case 8:
      tone(BUZZER_PIN, 523, 800); /*ド(C5)*/
      break;
    case 255:
      noTone(BUZZER_PIN);
      break;

  }


}

// 初期設定 --------------------------------------------------
void setup() {
  // 本体初期化
  Wire.setSDA(PIN_SDA);  // I2C0 SDA 端子番号設定
  Wire.setSCL(PIN_SCL);  // I2C0 SCL 端子番号設定
  Wire.begin();     // I2C通信開始設定(SDA,SDL)　※(0, 1 / 4, 5 / 8, 9 / 12, 13 / 16, 17 / 20, 21)

  //Serial通信用
  Serial.begin(115200);
  Serial1.setRX( PIN_RX0); // 標準設定以外の端子を使用する場合（UART1）
  Serial1.setTX( PIN_TX0); //
  Serial1.begin(9600);      // UART0 通信速度

  // OLED初期設定
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306:0 allocation failed"));
    for (;;); // エラーなら無限ループ
  }
  // OLED表示設定
  display.setTextColor(SSD1306_WHITE);  // 文字色

  pinMode(BTN1, INPUT_PULLUP); // ボタン1の端子を入力端子（プルアップ）に設定
  pinMode(BTN2, INPUT_PULLUP); // ボタン2の端子を入力端子（プルアップ）に設定
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(BAT_V, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);

  /* タイマーの初期化(割込み間隔はusで指定) */
  add_repeating_timer_us(1000, Timer, NULL, &st_timer);//1ms

  pinMode(BUZZER_PIN, OUTPUT);
}

// メイン -------------------------------------------------
void loop() {
  int counter_num;

  if (counter_4 > 20 && merody_end == 0) {
    counter_4 = 0;
    counter_5++;
    counter_num = counter_5 * 20;

    //sequence_sound_ver2(starWars_DV ,  counter_num);
    sequence_sound_ver2(DQ_LVUP, counter_num);
    // sequence_sound_ver2(Doremi, counter_num);


  }

  //OLED液晶表示
  if (counter_2 > 100) {
    if(oled_init_disp==1){
    oled_disp_initial();
    }else{
    oled_disp_robo_sens_ver3();
    }
    counter_2 = 0;
  }

  //シリアル通信の確認
  if (counter_1 > 20) {
    serial_comu_test();


    counter_1 = 0;
    //    Serial.println("");
  }

  //button_read_demo();
  if (counter_3 > 1000) {
    button_BAT_read();
    counter_3 = 0;
  }

  //Serial Output()
  if(counter_6 >2500){
    counter_6 = 0;
    serial_comu_soushin_test();    
  }

  
  //  Serial.println("loop_end");
  //delay(100);
}
