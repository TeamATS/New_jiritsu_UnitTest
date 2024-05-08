
void setup() {

  //敵センサ
  pinMode(PB12, INPUT);
  pinMode(PB13, INPUT);
  pinMode(PB14, INPUT);
  pinMode(PB15, INPUT);
  pinMode(PA8, INPUT);
  pinMode(PA9, INPUT);
  pinMode(PA10, INPUT);
  pinMode(PA11, INPUT);

  //白線センサ
  pinMode(PB6, INPUT);
  pinMode(PB7, INPUT);
  pinMode(PB1, INPUT);
  pinMode(PB0, INPUT);

  //照度センサ(ブレ上)
  pinMode(PA12, INPUT);
  pinMode(PA15, INPUT);
  pinMode(PB3, INPUT);
  pinMode(PB8, INPUT);
  pinMode(PB9, INPUT);

  //照度センサ(ブレ下)
  pinMode(PB4, INPUT);
  pinMode(PB5, INPUT);

  //審判モジュール
  pinMode(PB2, INPUT);
  
  //デバッグLED
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PC15, OUTPUT);
  pinMode(PC14, OUTPUT);
  pinMode(PC13, OUTPUT);

}

void loop() {

//  unsigned char flg = false;

//  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW

  digitalWrite(PC15, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC14, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC13, LOW);   // turn the LED off by making the voltage LOW

  //敵センサのデバッグ処理
  if(digitalRead(PB12) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PB13) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PB14) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PB15) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PA8) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PA9) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PA10) == 0)  digitalWrite(PC13, HIGH); //D5点灯
  if(digitalRead(PA11) == 0)  digitalWrite(PC13, HIGH); //D5点灯

  //白線センサのデバッグ処理
  if(digitalRead(PB6) == 1)  digitalWrite(PC15, HIGH); //D4点灯
  if(digitalRead(PB7) == 1)  digitalWrite(PC15, HIGH); //D4点灯
  if(digitalRead(PB1) == 1)  digitalWrite(PC15, HIGH); //D4点灯
  if(digitalRead(PB0) == 1)  digitalWrite(PC15, HIGH); //D4点灯

  //照度センサ(ブレ上)のデバッグ処理
  if(digitalRead(PA12) == 1)  digitalWrite(PC14, HIGH); //D5点灯
  if(digitalRead(PA15) == 1)  digitalWrite(PC14, HIGH); //D5点灯
  if(digitalRead(PB3) == 1)  digitalWrite(PC14, HIGH); //D5点灯
  if(digitalRead(PB8) == 1)  digitalWrite(PC14, HIGH); //D5点灯
  if(digitalRead(PB9) == 1)  digitalWrite(PC14, HIGH); //D5点灯

  //照度センサ(ブレ下)のデバッグ処理
  if(digitalRead(PB4) == 1)  digitalWrite(PC14, HIGH); //D5点灯
  if(digitalRead(PB5) == 1)  digitalWrite(PC14, HIGH); //D5点灯
 /*
  if(digitalRead(PB2) == 0)  digitalWrite(LED_BUILTIN, HIGH);

*/

/*
  if(flg){
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  }
*/
}
