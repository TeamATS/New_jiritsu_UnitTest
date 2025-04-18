#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "secret_wifi.h"
#include <LovyanGFX.hpp>


//------ 定数設定 ------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_SCL 5
#define OLED_SDA 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//PIN設定
#define PIN_SDA 13
#define PIN_SCL 15
#define PIN_TX0 43
#define PIN_RX0 44
#define BTN1  3 //動作確認　L
#define BTN2  5 //真ん中
#define BTN3  7
#define AD_MACHINE_IDENTIFY 1
#define AD_BATTERY_VOLT  9

#define PIN_MACHINE_AD 1
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

long tgt_sakusen_num = 0;
long tgt_sakusen_num_kari = 0;
int digits[6];

//以下のユーザモードは、初期のmode値で決める。４bitのスイッチ
int user_mode = 0;  //0---debug . 1---闘魂M .  2---影武者R 　3---闘魂K 　4---下田
int ui_screen = 0;
int ui_screen_sentaku1 = 0;

boolean ui_mode_change = 0;
//boolean ui_userselect = 0;  //ユーザの設定など条件で値を決める。 0---ユーザ未決定  1--ユーザ決定 notuse

//int WL_IDLE_STATUS=0;//250326
//int status = WL_IDLE_STATUS;     // the Wifi radio's status
int wifi_Mqtt_state = 0;  //0-- nowifi 1-- wifiok MQTT ng  2--MQTTok
int receivedValue = 0;
int receivedValue_to_sousin = 0;
int mqtt_incoming_Flag  = 0;
long wifi_connect_time = 0;

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
//timer割り込み設定----------------------------------------------
hw_timer_t *timer = NULL;
bool timer_flag = false;

int counter;
int counter_1;
int counter_2;
int counter_3;
int counter_mqtt;
int counter_5;
int counter_6;
int counter_6_1 =0;
int counter_8;
int counter_wifi;
int counter_sf;

//display設定--------------------------------------------------
int oled_init_disp = 1;
int oled_init_count = 0;

// 独自の設定を行うクラスを、LGFX_Deviceから派生して作成します。
// クラス名はLGFXからLGFX_SSD1306に変更してます。（コンストラクタ名も）
class LGFX_SSD1306 : public lgfx::LGFX_Device {
    // 接続するOLED表示器にあったインスタンスを準備します。
    lgfx::Panel_SSD1306   _panel_instance;  // SSD1306を使用する場合
    lgfx::Bus_I2C   _bus_instance;          // I2Cバスのインスタンス (ESP32のみ)

    // コンストラクタを作成し、ここで各種設定を行います。
  public:
    LGFX_SSD1306() {  // コンストラクタ名はクラス名に合わせてLGFXからLGFX_SSD1306に変更してます。（クラス名と同じにする）
      { // バス制御の設定を行います。
        auto cfg = _bus_instance.config();  // I2Cバス設定用の構造体を取得します。
        cfg.i2c_port    = 1;          // 使用するI2Cポートを選択 (0 or 1)
        cfg.freq_write  = 400000;     // 送信時のクロック
        cfg.freq_read   = 400000;     // 受信時のクロック
        cfg.pin_sda     = 5;         // SDAを接続しているピン番号
        cfg.pin_scl     = 6;         // SCLを接続しているピン番号
        cfg.i2c_addr    = 0x3C;       // I2Cデバイスのアドレス

        _bus_instance.config(cfg);    // 設定値をバスに反映します。
        _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
      }
      { // 表示パネル制御の設定を行います。
        // 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。
        auto cfg = _panel_instance.config();  // 表示パネル設定用の構造体を取得します。
        cfg.memory_width  = 128;      // ドライバICがサポートしている最大の幅
        cfg.memory_height =  64;      // ドライバICがサポートしている最大の高さ

        _panel_instance.config(cfg);  // 設定をパネルに反映
      }
      setPanel(&_panel_instance);     // 使用するパネルをセットします。
    }
};
static LGFX_SSD1306 lcd; // LGFX_SSD1306のインスタンス（クラスLGFX_SSD1306を使ってlcdでいろいろできるようにする）を作成
static LGFX_Sprite canvas(&lcd);  // スプライトを使うためのLGFX_Spriteのインスタンスを作成




//------ 関数定義 --------------------------------------------------------
void callback(char *topic, byte *payload, unsigned int length);
void serial_comu_micom() ;
void testdrawchar();
void onTimer(void* arg);



//------ 関数 --------------------------------------------------------

void setup() {
  // 本体初期化
  delay(100);

  //Serial通信用-------------------------
  Serial.begin(115200);     // PC通信 通信速度
  Serial0.begin(38400);     // マイコン間通信 通信速度

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }
  WiFi.disconnect();

  //------------------------
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT_PULLUP);

  //wifi
  while (!Serial && millis() < 5000)
    ;

  func_machine_identify();

  display.display();
  delay(1000); // Pause for 2 seconds
  display.clearDisplay();

  //LCD初期設定-------------------------
  lcd.init();                                     // 表示器初期化
  canvas.setTextWrap(false);                      // 右端到達時のカーソル折り返しを禁止(true)で許可
  canvas.createSprite(lcd.width(), lcd.height()); // スプライト用の仮想画面を画面幅を取得して準備
  lcd.setRotation(2);

  // check for the WiFi module:
  //  setupWifi();
  setupWifi_2nd();

  // タイマー構成の定義-------------------------
  esp_timer_create_args_t timer_args = {
    .callback = &onTimer,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "1ms_timer"
  };

  esp_timer_handle_t timer;
  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, 2000);  // 1000us = 1msごとに実行


  //display.clearDisplay();
  //testdrawchar();
}

//------ 関数 loop--------------------------------------------------------
void loop() {

  if (0) {//シリアル通信動作テスト用　esp32 -> blackpill 500ms, AAA trans
    if (num >= 500) {
      Serial0.println("AAAA");
      num = 0;
    } else {
      num++;
    }
    delay(1);
  }

  //  serial_comu_micom();
  // oled_disp_robo_sens_ver3();
  // oled_disp_control();
  //oled_disp_control_lav();

  loop_count_up_by_timer();
  loop_schedule();

  if (!client.connected())
  {
    reConnect();
  }
  client.loop();

}
//------ 関数 --------------------------------------------------------
/* タイマー割り込み処理 */
void onTimer(void* arg)
{
  timer_flag = true;
}

//------ 関数 --------------------------------------------------------
void loop_count_up_by_timer() {
  if (timer_flag)
  {
    counter++;
    counter_1++;//シリアル通信の確認
    counter_2++;//液晶表示
    counter_3++;
    counter_mqtt++;
    counter_6++;
    counter_8++;
    counter_wifi++;
    timer_flag = false;

  }
}

void loop_schedule(void) {
  bool done = true;
  //OLED液晶表示
  if (counter_2 > 50) {
    //    oled_disp_robo_sens_ver3();
    func_button_input();
    oled_disp_control_lav();
    counter_2 = 0;
    setupWifi_connect_kakunin();
  }

  //シリアル通信の確認
  if (counter_1 >= 2) {
    //serial_comu_test();
    counter_1 = 0;
    //    Serial.println("");
  }

  //BAT電圧チェック
  if (counter_3 > 1000) {
    button_BAT_read();
    counter_3 = 0;
  }

  if (mqtt_incoming_Flag == 1 && counter_6 > 200) {
    serial_comu_soushin_remocon2();
    counter_6 = 0;
    //MQTTでメッセージが来た時には、50msecごとに送信する。
    //アンサーバック値が来たときに、mqtt_incoming_Flagを下げる。
  }

  if (counter_8 > 3000) {
    if (Com_Err_Flag) {
      if (Com_Err_Flag_naiyou == 0) {
        //serial_comu_soushin_without_remocon(01, 01, 01); //寸動
        Com_Err_Flag_naiyou = 1;
      } else {
        //serial_comu_soushin_without_remocon(90, 90, 05); //GO
        Com_Err_Flag_naiyou = 0;
      }
    }
    counter_8 = 0;
  }

  //wifi 再接続1sec
  if (    counter_wifi > 2000) {
    counter_wifi = 0;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WIFI RETRY");
      WiFi.disconnect();

      delay(100);
      WiFi.begin(ssid, password); // Start Wifi connection.

      //WiFi.reconnect();
    }
  }


  //loop1からのコピー
  if (Com_Err_Flag) {
    //MQTT無効時
    counter_mqtt = 0;
  } else {
    //MQTT有効時
    if (counter_mqtt > 100) {
      //mqttClient.update();  // should be called
      client.loop();
      counter_mqtt = 0;
    }
    func_mqtt_publish_task();
  }
}

//------ 関数 --------------------------------------------------------

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
            mqtt_incoming_Flag = 0;
            answerback_incomig_Flag = 1;
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


  sensorValue = 1500;

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
      SubTopic = SubTopic_matsu;
      machineName = machineName_matsu;
      MQTT_M = MQTT_M_TM;
      MQTT_U = MQTT_U_TM;
      MQTT_P = MQTT_P_TM;
      break;
    case 2:
      ssid = ssid_koza;
      password = password_koza;
      mqtt_server = mqtt_server_koza;
      PubTopic = PubTopic_koza;
      SubTopic = SubTopic_koza;
      machineName = machineName_koza;
      MQTT_M = MQTT_M_KR;
      MQTT_U = MQTT_U_KR;
      MQTT_P = MQTT_P_KR;
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
  WiFi.setTxPower(WIFI_POWER_17dBm); // 送信出力値を設定

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
    //MQTT設定
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback); // Sets the message callback function.  设置消息回调函数
    Serial.printf("mq_setup is ok.\n");
    wifi_Mqtt_state = 2;
  }
}

void setupWifi_2nd()
{
  int wifi_NG_counter = 0;

  delay(10);

  Serial.printf("\nStart WiFiMQTTSecure on \n");

  WiFi.mode(WIFI_STA);        // Set the mode to WiFi station mode.
  WiFi.begin(ssid, password); // Start Wifi connection.
  Serial.print("4 ");
  WiFi.setTxPower(WIFI_POWER_18_5dBm); // 送信出力値を設定

  wifi_connect_time = millis();
}

void setupWifi_connect_kakunin() {
  long now_time;
  if (wifi_Mqtt_state == 0)
  {
    if (WiFi.status() == WL_CONNECTED) {
      wifi_Mqtt_state = 1;
      Serial.println("wifi_ok");
      now_time = millis() - wifi_connect_time;
      Serial.println("( ms)" + String(now_time));
    }
  } else if (wifi_Mqtt_state == 1) {
    //MQTT
    //MQTT設定
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback); // Sets the message callback function.  设置消息回调函数
    Serial.printf("mq_setup is ok.\n");
    wifi_Mqtt_state = 2;
  }  else {
    //WIFI接続できているので、何もしない
  }

}

//------------------------------------------------

void callback(char *topic, byte * payload, unsigned int length)
{
  Serial.println("MQTT_IS_COMING");

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

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void button_BAT_read() {

  int ad_value;
  long temp;

  //  state_SOC
  ad_value = analogRead(AD_BATTERY_VOLT);
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

//-MQTT-publish----------------------------------------------
void func_mqtt_publish_task() {
  int count_number;


  if (answerback_incomig_Flag == 1) {
    //アンサーバック値をMQTTでリモコン側に返す。
    publish_message_answerback();
    answerback_incomig_Flag = 0;
    count_number = (serial_trans1 * 10000) + (serial_trans2 * 100) + serial_trans3;
    tgt_sakusen_num = count_number;

  }

  if (debug_signal_incomig_Flag == 1) {
    //デバッグ値をMQTTでリモコン側に返す。
    publish_message_debug();
    debug_signal_incomig_Flag = 0;
  }

  if (sensor_signal_incomig_Flag == 1) {
    //デバッグ値をMQTTでリモコン側に返す。
    publish_message_sens();
    sensor_signal_incomig_Flag = 0;
  }
}

void publish_message_answerback() {
  char counterStr[6];  // 必要に応じて適切なサイズに調整
  int count_number;

  count_number = (serial_trans1 * 10000) + (serial_trans2 * 100) + serial_trans3;
  //パブリッシュの実施
  sprintf(counterStr, "%d", count_number);
  //mqttClient.publish(PubTopic0, counterStr);  // Publishes a message to the specified
  client.publish(PubTopic, counterStr);
  Serial.println("Picoからサーバへ送信");
}

void publish_message_sens() {
  client.publish(PubTopic_s1, rx_sen1.c_str());
  client.publish(PubTopic_s2, rx_sen2.c_str());
  client.publish(PubTopic_s3, rx_sen3.c_str());
}
void publish_message_test() {

  /*パブリッシュの実施*/
  char counterStr[6]; // 必要に応じて適切なサイズに調整
  long count_number = 123456;
  Serial.println("mqtt_publish:" + String(PubTopic));
  sprintf(counterStr, "%d", count_number);
  client.publish(PubTopic, counterStr); // Publishes a message to the specified
}


void publish_message_debug() {
  client.publish(PubTopic_d1, rx_dbg1.c_str());
  client.publish(PubTopic_d2, rx_dbg2.c_str());
  client.publish(PubTopic_d3, rx_dbg3.c_str());
}

void reConnect()
{
  if (wifi_Mqtt_state > 1) {
    while (!client.connected())
    { wifi_Mqtt_state = 2;

      if (client.connect(MQTT_M, MQTT_U, MQTT_P))
      {
        delay(500);
      }
      if (client.connected()) {
        Serial.println("mqtt connected ");
        Serial.println("mqtt " + String(MQTT_M));
        Serial.println("mqtt " + String(MQTT_U));
        Serial.println("mqtt " + String(MQTT_P));
        Serial.println("Subt " + String(SubTopic));
        client.subscribe(SubTopic);

      }
    }
    wifi_Mqtt_state = 3;
  }
}


void serial_comu_soushin_remocon2() {
  int result;
  int receivedValue_soushin;
  char sendPacket1[24] = "";
  char sendPacket2[24] = "";
  char sendPacket3[24] = "";

  counter_6_1++;

  if (counter_6_1 > 5) {
    counter_6_1 = 1;
  }
  receivedValue_soushin = receivedValue;
  //Txに出力
  //甲乙丙:2桁/2桁/2桁 を 分割し、パディング
  serial_trans1 = receivedValue_soushin / 10000;
  sprintf(sendPacket1, "%02d", serial_trans1);
  //serial_trans1:90
  receivedValue_soushin = receivedValue_soushin - (serial_trans1 * 10000);
  serial_trans2 = receivedValue_soushin / 100;
  sprintf(sendPacket2, "%02d", serial_trans2);
  receivedValue_soushin = receivedValue_soushin - (serial_trans2 * 100);
  serial_trans3 = receivedValue_soushin;
  sprintf(sendPacket3, "%02d", serial_trans3);

  Serial1.println("S");

  Serial1.println(String(sendPacket1));
  Serial1.println(String(sendPacket2));
  Serial1.println(String(sendPacket3));
  //シリアルコンソールに出力
  Serial.print("picoからBPへ送信");
  Serial.print("\n");
  Serial.println(String(sendPacket1));
  //  Serial.print("\n");
  Serial.println(String(sendPacket2));
  //  Serial.print("\n");
  Serial.println(String(sendPacket3));
  Serial.print("\n");
  //  Serial.println(counter_6_1);
}
