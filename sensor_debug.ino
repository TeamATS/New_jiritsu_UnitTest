
void setup() {

  pinMode(PB12, INPUT_PULLUP);
  pinMode(PB13, INPUT_PULLUP);
  pinMode(PB14, INPUT_PULLUP);
  pinMode(PB15, INPUT_PULLUP);
  pinMode(PA8, INPUT_PULLUP);
  pinMode(PA9, INPUT_PULLUP);
  pinMode(PA10, INPUT_PULLUP);
  pinMode(PA11, INPUT_PULLUP);
  pinMode(PA12, INPUT_PULLUP);
  pinMode(PA15, INPUT_PULLUP);
  pinMode(PB3, INPUT_PULLUP);
  pinMode(PB4, INPUT_PULLUP);
  pinMode(PB5, INPUT_PULLUP);
  pinMode(PB6, INPUT_PULLUP);
  pinMode(PB7, INPUT_PULLUP);
  pinMode(PB8, INPUT_PULLUP);
  pinMode(PB9, INPUT_PULLUP);

  pinMode(PB2, INPUT_PULLUP);
  pinMode(PB1, INPUT_PULLUP);
  pinMode(PB0, INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PC15, OUTPUT);
  pinMode(PC14, OUTPUT);
  pinMode(PC13, OUTPUT);

}

void loop() {

//  unsigned char flg = false;

//  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
/*
  digitalWrite(PC15, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC14, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC13, LOW);   // turn the LED off by making the voltage LOW
*/

  digitalWrite(PC15, HIGH);   // turn the LED off by making the voltage LOW
  digitalWrite(PC14, HIGH);   // turn the LED off by making the voltage LOW
  digitalWrite(PC13, HIGH);   // turn the LED off by making the voltage LOW

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
  if(digitalRead(PB6) == 1)  digitalWrite(PC14, HIGH); //D4点灯
  if(digitalRead(PB7) == 1)  digitalWrite(PC14, HIGH); //D4点灯
  if(digitalRead(PB1) == 1)  digitalWrite(PC14, HIGH); //D4点灯
  if(digitalRead(PB0) == 1)  digitalWrite(PC14, HIGH); //D4点灯

  //照度センサ(ブレ上)のデバッグ処理
  if(digitalRead(PA12) == 0)  digitalWrite(PC15, HIGH); //D5点灯
  if(digitalRead(PA15) == 0)  digitalWrite(PC15, HIGH); //D5点灯
  if(digitalRead(PB3) == 0)  digitalWrite(PC15, HIGH); //D5点灯
  if(digitalRead(PB8) == 0)  digitalWrite(PC15, HIGH); //D5点灯
  if(digitalRead(PB9) == 0)  digitalWrite(PC15, HIGH); //D5点灯

  //照度センサ(ブレ下)のデバッグ処理
  if(digitalRead(PB4) == 0)  digitalWrite(PC15, HIGH); //D5点灯
  if(digitalRead(PB5) == 0)  digitalWrite(PC15, HIGH); //D5点灯
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
