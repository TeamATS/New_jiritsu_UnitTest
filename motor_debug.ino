
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

  pinMode(PA4, OUTPUT); //DIR_R
  pinMode(PA5, OUTPUT); //DIR_L
  analogWriteFrequency(1000);

  digitalWrite(PC15, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC14, LOW);   // turn the LED off by making the voltage LOW
  digitalWrite(PC13, LOW);   // turn the LED off by making the voltage LOW

}

void loop() {

  digitalWrite(PA4, HIGH);
  digitalWrite(PA5, HIGH);
  analogWrite(PA6, 126); //PWM_R
  analogWrite(PA7, 126); //PWM_L
  digitalWrite(PC13,HIGH);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,LOW);

delay(1000);

  digitalWrite(PA4, HIGH);
  digitalWrite(PA5, LOW);
  analogWrite(PA6, 126); //PWM_R
  analogWrite(PA7, 126); //PWM_L
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,HIGH);

delay(1000);

  digitalWrite(PA4, LOW);
  digitalWrite(PA5, HIGH);
  analogWrite(PA6, 126); //PWM_R
  analogWrite(PA7, 126); //PWM_L
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,HIGH);

delay(1000);

  digitalWrite(PA4, LOW);
  digitalWrite(PA5, LOW);
  analogWrite(PA6, 126); //PWM_R
  analogWrite(PA7, 126); //PWM_L
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,HIGH);

delay(1000);

/*
  //正転
  digitalWrite(PA4, HIGH);
//  digitalWrite(PA5, HIGH);
  analogWrite(PA6, 255); //PWM_R
//  analogWrite(PA7, 255); //PWM_L
  digitalWrite(PC13,HIGH);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,LOW);

  delay(1000);

  //停止
  digitalWrite(PA4, HIGH);
//  digitalWrite(PA5, HIGH);
  analogWrite(PA6, 0); //PWM_R
//  analogWrite(PA7, 0); //PWM_L
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,HIGH);
  digitalWrite(PC15,LOW);

  //逆転
  digitalWrite(PA4, LOW);
//  digitalWrite(PA5, LOW);
  analogWrite(PA6, 255); //PWM_R
//  analogWrite(PA7, 255); //PWM_L
  digitalWrite(PC13,HIGH);
  digitalWrite(PC14,HIGH);
  digitalWrite(PC15,LOW);

  delay(1000);

  //停止
  digitalWrite(PA4, LOW);
//  digitalWrite(PA5, LOW);
  analogWrite(PA6, 0); //PWM_R
//  analogWrite(PA7, 0); //PWM_L
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,HIGH);

  delay(1000);
  digitalWrite(PC13,LOW);
  digitalWrite(PC14,LOW);
  digitalWrite(PC15,LOW);
  */
}
