//------ 定数設定 ------

//PIN設定
#define PIN_SDA 4
#define PIN_SCL 5
#define PIN_TX0 9
#define PIN_RX0 8

//------ グローバル変数設定 ------
unsigned int answerback_deta = 0;
unsigned int answerback_deta_prev = 0;

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

//------ 関数定義 ------

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

  if (Serial1.available()) {  // 受信データ(Serial1)があれば
    rx_str1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)

    if (rx_str1.equals("M\r") == 1) {
      //文字列比較
      //https://greenhornprofessional.hatenablog.com/entry/2020/09/13/223155

      rx_sen1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sen2 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sen3 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)

      Serial.print("rx_sen1:");
      Serial.print(rx_sen1);
      Serial.print(",");
      Serial.print("rx_sen2:");
      Serial.print(rx_sen2);
      Serial.print(",");
      Serial.print("rx_sen3:");
      Serial.print(rx_sen3);

      while (Serial1.available()) {
        Serial1.read();
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
      //sensor_signal_incomig_Flag = 1;

    }
    else if (rx_str1.equals("D\r") == 1) {
      rx_dbg1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg2 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg3 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg4 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_dbg5 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      while (Serial1.available()) {
        Serial1.read();
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
      rx_sak1 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sak2 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      rx_sak3 = Serial1.readStringUntil('\n');  // 受信データを\nの手前まで取得(Serial1)
      while (Serial1.available()) {
        Serial1.read();
      }


      Serial.println("BPからのアンサーバック値を表示");
      Serial.println(rx_sak1);
      Serial.println(rx_sak2);
      Serial.println(rx_sak3);
      Serial.println("\n");
      num_state = rx_sak1.toInt();
      answerback_deta = 0;
      if (num_state == serial_trans1) {        
        answerback_deta = answerback_deta + num_state*10000;
        num_state = rx_sak2.toInt();
        if (num_state == serial_trans2) {
          answerback_deta = answerback_deta + num_state*100;
          num_state = rx_sak3.toInt();
          if (num_state == serial_trans3) {
            answerback_deta = answerback_deta + num_state;
            //mqtt_incoming_Flag = 0;
            //answerback_incomig_Flag = 1;
            //アンサーバック値がGo/Stopコマンドの場合、捨てる(dispに表示したくない)
            if(answerback_deta == 909003 || answerback_deta == 909005){
              answerback_deta = answerback_deta_prev;
            }
            else{
              answerback_deta_prev = answerback_deta;
              //dekamoji_Flag = 1;
            } 
          }
        }
      }
      //Serial.print(mqtt_incoming_Flag);
    } else {
      while (Serial1.available()) {
        Serial1.read();
      }
    }
  }
}


void setup() {
  // 本体初期化


  //Serial通信用
  Serial.begin(115200);     // PC通信 通信速度
  Serial1.begin(38400);     // マイコン間通信 通信速度


}

void loop() {
  // put your main code here, to run repeatedly:

  serial_comu_micom();

}
