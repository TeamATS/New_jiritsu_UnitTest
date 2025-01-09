const int QH=  16;    // データの入力ピン(74HC165-QH). GP16
const int CK=  14;    // クロック出力ピン(74HC165-CK/CP).  GP28
const int SL=  15;     // レジスタロードピン(74HC165-SL/PL) GP27
//const int dataPin = 2;   /* Q7 */
//const int clockPin = 3;  /* CP */
//const int latchPin = 4;  /* PL */

//参考　http://zattouka.net/GarageHouse/micon/circuit/HC165.htm$0

byte ShiftData ;
byte ShiftData2 ;
const int pulseWidth = 1;      // pulse width in microseconds

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello1!");

  pinMode(SL,OUTPUT);
  pinMode(QH,INPUT);
  pinMode(CK,OUTPUT);

  digitalWrite(SL,HIGH) ;
  digitalWrite(SCK,LOW) ;
      Serial.println("test3") ;

  // ３秒後に開始
  delay(3000);
  // 74HC165入力端子のデータを読込み表示を行う
  ShiftData = ShiftIn(MISO,SCK,SL) ;
  Serial.println(ShiftData, BIN);
      Serial.println("test4") ;

}

void loop() {
// put your main code here, to run repeatedly:
  byte dt ;
  byte dt2;
     double  t1;
     double t2;
  int i;
    delay(500); // this speeds up the simulation
    //Serial.println("test2") ;

if(0){
   // 74HC165入力端子のデータを読込む
    dt = ShiftIn(QH,CK,SL) ;
     if ( dt != ShiftData ) {
          // データに変化が有れば表示する
          ShiftData = dt ;
          Serial.println(dt, BIN) ;
     }

     dt2 =ShiftInNext(QH,CK,SL);
     if ( dt2 != ShiftData2 ) {
          // データに変化が有れば表示する
          ShiftData2 = dt2 ;
          Serial.println(dt2, BIN) ;
     }
     //Serial.println(dt, BIN) ;
}else{
    t1 =micros();
    dt = ShiftIn(QH,CK,SL) ;
    dt2 =ShiftInNext(QH,CK,SL);
    t2 = micros();
    
     if ( dt != ShiftData  || dt2 != ShiftData2) {
          // データに変化が有れば表示する
          ShiftData = dt ;
                  ShiftData2 = dt2 ;
          Serial.print(dt, BIN) ;
          Serial.print(" ") ;
          Serial.println(dt2, BIN) ;
          Serial.println(t2-t1) ;

     }

}

}

// 74HC165のデータを読込む(受信する)関数
// DataPin ：データの入力ピン
// clockPin：クロック出力ピン
// loadPin ：レジスタロードピン
// 74HC165のデータを読込む(受信する)関数
byte ShiftIn(int dataPin,int clockPin,int loadPin)
{
     unsigned char x ;
     int  i ;

    //Serial.println("test") ;
     x = 0 ;
     digitalWrite(loadPin, LOW);// 入力端子(A-H)の情報をレジスタに読込めと指示する
      delayMicroseconds( pulseWidth);
     digitalWrite(loadPin, HIGH);

     x = x | (digitalRead(dataPin) << 7) ;      // H端子の内容を受信する
     for (i=6 ; i>=0 ; i--) { // G端子～A端子まで繰り返す
          digitalWrite(clockPin, HIGH);// 1ビットシフト指示を出す
          delayMicroseconds( pulseWidth);
          
          x = x | (digitalRead(dataPin) << i) ;      // シフトされた内容を受信する
          digitalWrite(clockPin, LOW);
          delayMicroseconds( pulseWidth);
     }
     return x ;
}

byte ShiftInNext(int dataPin,int clockPin,int loadPin)
{
     unsigned char x ;
     int  i ;

    //Serial.println("test") ;
     x = 0 ;
     //digitalWrite(loadPin, LOW);// 入力端子(A-H)の情報をレジスタに読込めと指示する
     // delayMicroseconds( pulseWidth);
     //digitalWrite(loadPin, HIGH);

     for (i=7 ; i>=0 ; i--) { // G端子～A端子まで繰り返す
          digitalWrite(clockPin, HIGH);// 1ビットシフト指示を出す
          delayMicroseconds( pulseWidth);
          
          x = x | (digitalRead(dataPin) << i) ;      // シフトされた内容を受信する
          digitalWrite(clockPin, LOW);
          delayMicroseconds( pulseWidth);
     }
     return x ;
}
