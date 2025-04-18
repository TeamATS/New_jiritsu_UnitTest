//--display---
//void testdrawchar(uint8_t offset = 0) {


void oled_disp_control_lav() {
  //  oled_stop_state_lav();
  //  oled_menu_gamen_lav();
  //  oled_ready_set_gamen_lav();
  //oled_ready_sakusen_gamen_lav();
  ui_contorl();
  ui_disp();
}

void oled_disp_control_lav0() {
  //  oled_stop_state_lav();
  //  oled_menu_gamen_lav();
  //  oled_ready_set_gamen_lav();
  oled_ready_sakusen_gamen_lav();
}


void ui_contorl() {
  switch (ui_screen) {
    case 0://電源投入後の画面
      ui_0();
      break;
    case 10://待機状態の画面
      ui_10();
      break;
    case 11://センサ表示画面
      ui_10();
      break;
    case 12://ユーザ選択の画面
      ui_10();
      break;
    case 20://手動選択画面
      ui_20();
      break;
    case 21://作戦番号の画面
      ui_21();
      break;
    case 80://GO状態の画面
      ui_80();
      break;
    case 90://STOP状態の画面
      ui_90();
      break;
  }
}

void ui_disp() {
  switch (ui_screen) {
    case 0://電源投入後の画面
      disp_0();

      //oled_disp_initial();
      break;
    case 10://待機状態の画面
      disp_10();
      //disp_80();
      //Serial.println(ui_screen);

      //oled_wait_state();
      break;
    case 11://センサ表示画面
      disp_11();
      break;
    case 12://ユーザ選択の画面
      disp_12();
      break;
    case 20://手動選択画面
      disp_20();
      break;
    case 21://作戦番号の画面
      disp_21();
      //oled_ready_sakusen_gamen_lav();
      break;
    case 80://GO状態の画面
      disp_80();
      break;
    case 90://STOP状態の画面
      disp_90();
      break;
  }
}


void ui_0() {
  //起動時の画面　一定時間が経過すれば、通常画面NO10に移る
  ui_screen_sentaku1++; //use ui_screen_sentaku1  as counter;

  if (ui_screen_sentaku1 > 30) {
    ui_screen = 10;
    ui_screen_sentaku1 = 0;
  }
}

void ui_10() {
  int ui_screen_next;
  ui_screen_next = ui_screen;
  //通常画面　左右ボタンで切り替える
  if (ui_screen == 10 ) {
    if (button_1_downedge) {
      ui_screen_next = 12;
    }
    else if (button_3_downedge) {
      ui_screen_next = 11;
    } else if (button_2_downedge) {
      publish_message_test();

      ui_screen_next = 20;
    }
  }

  if (ui_screen == 11 ) {
    if (button_1_downedge) {
      ui_screen_next = 10;
    }
    else if (button_3_downedge) {
      ui_screen_next = 12;
    }
  }

  if (ui_screen == 12 ) {
    //アナログ抵抗器によるマシン認識変更画面
    //０　変更しない
    //１　アナログチューン
    //２　WIFI接続

    if (button_1_downedge) {
      ui_screen_next = 11;
    }
    else if (button_3_downedge) {
      //ui_screen_sentaku1
      if (ui_screen_sentaku1 == 0) {
        ui_screen_next = 3;
      } else if (ui_screen_sentaku1 == 2) {
        //wifi接続動作を行う（画面遷移はしない。）
        setupWifi();
      }
    } else if (button_2_downedge) {
      //中ボタン（選択ボタン）操作時
      ui_screen_sentaku1++;
      if (ui_screen_sentaku1 > 2) {
        ui_screen_sentaku1 = 0;
      }
    }
  }

  if (ui_screen_next == ui_screen) {
    //画面遷移なし
  } else {
    //画面遷移あり
    ui_screen = ui_screen_next;
    ui_screen_sentaku1 = 0;
  }
}

void ui_20() {
  int ui_screen_next;
  long num_if;

  ui_screen_next = ui_screen;

  if (ui_mode_change) {
    //このモードに入った時に何かする場合はここに記述
    ui_mode_change = 0;
  }

  //通常画面　左右ボタンで切り替える
  if (button_1_downedge) {
    ui_screen_next = 10;
  }
  else if (button_3_downedge) {
    if (ui_screen_sentaku1 == 0) {
      ui_screen_next = 10;
    } else if (ui_screen_sentaku1 == 1) {
      ui_screen_next = 21;
    } else if (ui_screen_sentaku1 == 2) {
      ui_screen_next = 80;
    }
  }

  //中ボタン（選択ボタン）操作時
  if (button_2_downedge) {
    ui_screen_sentaku1++;
    if (ui_screen_sentaku1 > 2) {
      ui_screen_sentaku1 = 0;
    }
  }

  //
  if (ui_screen_next == ui_screen) {
    //画面遷移なし
    ui_mode_change = 0;
  } else {
    //画面遷移あり
    ui_screen = ui_screen_next;
    ui_screen_sentaku1 = 0;
    ui_mode_change = 1;
  }
}

void ui_21() {
  int num_temp;
  int ui_screen_next;
  long number_kari;
  ui_screen_next = ui_screen;

  int number = 123456;  // 6桁の数字

  if (ui_mode_change) {
    //このモードに入った時に何かする場合はここに記述
    number_kari = tgt_sakusen_num;
    for (int i = 0; i < 6; i++) {
      digits[i] = number_kari % 10;
      number_kari /= 10;
    }
    ui_mode_change = 0;
  }


  for ( int i = 0; i < 6; i++) {
    tgt_sakusen_num_kari = tgt_sakusen_num_kari * 10 + digits[i];
  }

  //通常画面　左右ボタンで切り替える
  if (ui_screen_sentaku1 == 0) {
    //セットモードのとき
    if (button_1_downedge) {
      ui_screen_next = 20;
    } else if (button_3_downedge) {
      //作戦番号を上書きする
      ui_screen_next = 20;
      tgt_sakusen_num = tgt_sakusen_num_kari;
      //serialTX
      receivedValue = tgt_sakusen_num_kari;
      mqtt_incoming_Flag = 1;

    } else if (button_2_downedge) {
      ui_screen_sentaku1++;
    }
  } else {

    //数字調整のとき
    num_temp = digits[7 - ui_screen_sentaku1];
    if (button_2_downedge) {
      //選択している桁を移動させる
      ui_screen_sentaku1++;
      if (ui_screen_sentaku1 > 6) {
        ui_screen_sentaku1 = 0;
      }
    } else if (button_1_downedge) {
      num_temp++;
    } else if (button_3_downedge) {
      num_temp--;
    }
    num_temp = (num_temp + 10) % 10;
    digits[7 - ui_screen_sentaku1] = num_temp;
  }

  //
  if (ui_screen_next == ui_screen) {
    //画面遷移なし
    ui_mode_change = 0;
  } else {
    //画面遷移あり
    ui_screen = ui_screen_next;
    ui_screen_sentaku1 = 0;
    ui_mode_change = 1;
  }

}

void ui_80() {
  int ui_screen_next;
  ui_screen_next = ui_screen;
  if (ui_mode_change) {
    //このモードに入った時に何かする場合はここに記述
    ui_mode_change = 0;
    //serialTX
    receivedValue = 909003;
    mqtt_incoming_Flag = 1;

  }

  if (tgt_sakusen_num == 909005) {
    ui_screen_next = 90;
  }

  //
  if (ui_screen_next == ui_screen) {
    //画面遷移なし
    ui_mode_change = 0;
  } else {
    //画面遷移あり
    ui_screen = ui_screen_next;
    ui_screen_sentaku1 = 0;
    ui_mode_change = 1;
  }
}

void ui_90() {
  int ui_screen_next;
  ui_screen_next = ui_screen;

  if (ui_mode_change) {
    //このモードに入った時に何かする場合はここに記述
    ui_mode_change = 0;
  }


  if (button_1_downedge || button_2_downedge || button_3_downedge) {
    //ボタン操作があれば、
    ui_screen_next = 10;
  } else if (tgt_sakusen_num != 909005) {
    ui_screen_next = 10;
  }


  //
  if (ui_screen_next == ui_screen) {
    //画面遷移なし
    ui_mode_change = 0;
  } else {
    //画面遷移あり
    ui_screen = ui_screen_next;
    ui_screen_sentaku1 = 0;
    ui_mode_change = 1;
  }
}


void disp_0() {
  char WORD[] = "team ATS    ";

  oled_init_count++;
  /*DIPSW の表示*/
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）

  for (int i = 0; i < oled_init_count; i++) {
    canvas.setCursor(i * 12, 30);  // 表示開始位置左上角（X,Y）
    canvas.printf("%c", WORD[i]);  // カウント数
  }
  display.setTextSize(1);  // 文字サイズ（1）

  for (int i = 0; i < 11; i++) {
    //ユーザモードの表示
    canvas.setCursor(i * 6, 50);  // 表示開始位置左上角（X,Y）
    canvas.printf("%c", machineName[i]);  // カウント数
  }

  if (oled_init_count > 11) {
    oled_init_disp = 0;
  }
  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
}

void disp_10() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）

  canvas.setTextSize(1);              // 文字倍率を戻す

  canvas.setCursor(0, 0);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_8);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println(machineName);                // 表示内容をcanvasに準備

  canvas.setCursor(16, 16);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("WIFI");                // 表示内容をcanvasに準備

  canvas.setCursor(16 + 8, 32);                       // 座標を指定（x, y）
  if (wifi_Mqtt_state >= 3) {
    canvas.println("接");                // 表示内容をcanvasに準備
  } else {
    canvas.println("未");                // 表示内容をcanvasに準備
  }


  canvas.setCursor(64, 16);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("SOC");                // 表示内容をcanvasに準備
  canvas.setTextSize(2);              // 文字倍率を戻す
  canvas.setCursor(64, 24);                         // 座標を指定（x, y）
  canvas.printf("%02d%%\n", state_SOC);  // SOC表示

  canvas.setTextSize(1);              // 文字倍率を戻す
  canvas.setCursor(0, 48);                         // 座標を指定（x, y）
  canvas.printf("%02d%%\n", tgt_sakusen_num);  // 作戦番号表示

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
  canvas.setTextSize(1);              // 文字倍率を戻す

}

void disp_11() {
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
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）

  canvas.drawRect(13, 5, 65, 23, TFT_WHITE);   // 四角
  canvas.drawRect(13, 30, 65, 30, TFT_WHITE);  // 四角

  /*------------照度センサ----------------------*/
  canvas.drawCircle(L2x, S1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(L1x, S1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(C1x, S1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(R1x, S1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(R2x, S1y, 4, TFT_WHITE);  // 円

  canvas.drawCircle(L1x, S2y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(R1x, S2y, 4, TFT_WHITE);  // 円

  /*------------対物センサ----------------------*/
  canvas.drawCircle(L2x, T1y, 4, TFT_WHITE);   // 円
  canvas.drawCircle(L1x, T1y, 4, TFT_WHITE);   // 円
  canvas.drawCircle(C1x, Tcfy, 4, TFT_WHITE);  // 円
  canvas.drawCircle(R1x, T1y, 4, TFT_WHITE);   // 円
  canvas.drawCircle(R2x, T1y, 4, TFT_WHITE);   // 円

  canvas.drawCircle(L2x, T2y, 4, TFT_WHITE);   // 円
  canvas.drawCircle(C1x, Tcry, 4, TFT_WHITE);  // 円
  canvas.drawCircle(R2x, T2y, 4, TFT_WHITE);   // 円

  /*-----------白線センサ-----------------------*/
  canvas.drawCircle(HLx, H1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(HRx, H1y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(HLx, H2y, 4, TFT_WHITE);  // 円
  canvas.drawCircle(HRx, H2y, 4, TFT_WHITE);  // 円


  /*----------------------------------*/
  /*対物センサ*/
  /*対物センサの表示　遠距離*/
  if (sens_object[0] == 1) {
    canvas.fillCircle(L2x, T2y, 2, TFT_WHITE);  // L1
  }
  if (sens_object[1] == 1) {
    canvas.fillCircle(L2x, T1y, 2, TFT_WHITE);  //L2
  }
  if (sens_object[2] == 1) {
    canvas.fillCircle(L1x, T1y, 2, TFT_WHITE);  // L3
  }
  if (sens_object[3] == 1) {
    canvas.fillCircle(C1x, Tcfy, 2, TFT_WHITE);  //C1
  }
  if (sens_object[4] == 1) {
    canvas.fillCircle(C1x, Tcry, 2, TFT_WHITE);  // C2
  }
  if (sens_object[5] == 1) {
    canvas.fillCircle(R1x, T1y, 2, TFT_WHITE);  // R3
  }
  if (sens_object[6] == 1) {
    canvas.fillCircle(R2x, T1y, 2, TFT_WHITE);  // R2
  }
  if (sens_object[7] == 1) {
    canvas.fillCircle(R2x, T2y, 2, TFT_WHITE);  // R1
  }
  /*----------------------------------*/
  /*照度センサ*/
  if (sens_blade_shoudo[0] == 1) {
    canvas.fillCircle(L2x, S1y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  if (sens_blade_shoudo[1] == 1) {
    canvas.fillCircle(L1x, S1y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  if (sens_blade_shoudo[2] == 1) {
    canvas.fillCircle(C1x, S1y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  if (sens_blade_shoudo[3] == 1) {
    canvas.fillCircle(R1x, S1y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  if (sens_blade_shoudo[4] == 1) {
    canvas.fillCircle(R2x, S1y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  /*----------------------------------*/
  /*ブレード上スイッチ*/
  if (sens_blade_sw[0] == 1) {
    canvas.fillCircle(L1x, S2y, 2, TFT_WHITE);  // 円（塗り潰し）
  }
  if (sens_blade_sw[1] == 1) {
    canvas.fillCircle(R1x, S2y, 2, TFT_WHITE);  // 円（塗り潰し）
  }

  /*----------------------------------*/
  /*白線センサ*/
  if (sens_white[0] == 1) {
    canvas.fillCircle(HLx, H1y, 2, TFT_WHITE);  // FL
  }
  if (sens_white[2] == 1) {
    canvas.fillCircle(HRx, H1y, 2, TFT_WHITE);  // FR
  }
  if (sens_white[1] == 1) {
    canvas.fillCircle(HLx, H2y, 2, TFT_WHITE);  // RL
  }
  if (sens_white[3] == 1) {
    canvas.fillCircle(HRx, H2y, 2, TFT_WHITE);  // RR
  }
  /*----------------------------------*/

  //SOC表示
  canvas.setTextSize(2);                 // 文字サイズ（2）
  canvas.setCursor(90, 16);              // 表示開始位置左上角（X,Y）
  canvas.printf("%02d%%\n", state_SOC);  // SOC表示

  canvas.setCursor(90, 35);  // 表示開始位置左上角（X,Y）


  //display.display();  // 表示実行
  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
  canvas.setTextSize(1);              // 文字倍率を戻す

}


void disp_12() {

  int sensorValue ;
  int haba;  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  // 日本語表示

  func_machine_identify();
  canvas.clearDisplay();  // 表示クリア
  canvas.setTextSize(2);  // 文字サイズ（1）
  canvas.setCursor(2, 30);              // 表示開始位置左上角（X,Y）
  canvas.printf(machineName);       // SOC表示

  sensorValue = analogRead(AD_MACHINE_IDENTIFY); //0~4096
  haba = (sensorValue * 60) / 4096;
  canvas.drawRect(1, 10, 65, 10, TFT_WHITE);
  canvas.fillRect(3, 12, haba, 8, TFT_WHITE);
  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
  canvas.setTextSize(1);              // 文字倍率を戻す

}

void disp_20() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  // 日本語表示
  canvas.setCursor(0, 0);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("手動設定");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 16);                        // 座標を指定（x, y）
  //  canvas.setFont(&fonts::lgfxJapanMincho_16);     // 明朝体P（8,12,16,20,24,28,32,36,40）
  canvas.println("セットしない");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 32);                        // 座標を指定（x, y）
  canvas.println("作戦 set");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 48);                        // 座標を指定（x, y）
  canvas.println("READY set");                // 表示内容をcanvasに準備
  canvas.drawLine(0, 16, 128, 15, TFT_WHITE);  // 線（始点x,始点y,終点x,終点y,色）
  canvas.fillCircle(8, 8 + (ui_screen_sentaku1 + 1) * 16, 6, TFT_WHITE); // 塗り潰し円（始点x,始点y,半径,色）

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
}

void disp_21() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  // 日本語表示
  canvas.setCursor(0, 0);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("←    ↓   　→");                // 表示内容をcanvasに準備
  canvas.drawLine(0, 16, 128, 15, TFT_WHITE);  // 線（始点x,始点y,終点x,終点y,色）
  canvas.setCursor(14, 16);                        // 座標を指定（x, y）
  //  canvas.setFont(&fonts::lgfxJapanMincho_16);     // 明朝体P（8,12,16,20,24,28,32,36,40）
  canvas.println("作戦セットする");                // 表示内容をcanvasに準備
  //  canvas.setCursor(14, 32);                        // 座標を指定（x, y）
  //  canvas.println("セットする");                // 表示内容をcanvasに準備
  //  canvas.setCursor(14, 48);                        // 座標を指定（x, y）
  ui_screen_sentaku1 = 2;
  if (ui_screen_sentaku1 < 1) {
    canvas.fillCircle(8, 8 + (ui_screen_sentaku1 + 1) * 16, 6, TFT_WHITE); // 塗り潰し円（始点x,始点y,半径,色）
  } else {
    canvas.drawRect(10 + ui_screen_sentaku1 * 16, 51, 12, 12, TFT_WHITE); // 四角（始点x,始点y,縦長さ,横長さ）
  }

  canvas.setTextSize(0.5);            // 文字倍率変更
  canvas.setCursor(16, 32, &Font7);  // 座標とフォントを指定（x, y, フォント）
  //  canvas.printf("%06d", 2020);         // カウント数表示
  canvas.printf("%06d", tgt_sakusen_num_kari);         // カウント数表示
  canvas.setTextSize(1);              // 文字倍率を戻す
  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する

}

void disp_80() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  oled_init_count =  oled_init_count + 2;
  if (0) {
    canvas.fillCircle(64, 32, oled_init_count + 72, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count + 64, TFT_WHITE); // FL
    canvas.fillCircle(64, 32, oled_init_count + 56, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count + 48, TFT_WHITE); // FL
    canvas.fillCircle(64, 32, oled_init_count + 40, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count + 32, TFT_WHITE); // FL
    canvas.fillCircle(64, 32, oled_init_count + 24, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count + 16, TFT_WHITE); // FL
    canvas.fillCircle(64, 32, oled_init_count + 8, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count, TFT_WHITE);  // FL
    canvas.fillCircle(64, 32, oled_init_count - 8, TFT_BLACK); // FL
    canvas.fillCircle(64, 32, oled_init_count - 16, TFT_WHITE); // FL

    if ( oled_init_count > 16) {
      oled_init_count = 0;
    }
  } else {
    canvas.fillCircle(64, 64, 8 * oled_init_count + 4, TFT_WHITE); // FL
    canvas.fillCircle(64, 64, 8 * oled_init_count, TFT_BLACK); // FL
    if ( oled_init_count > 16) {
      oled_init_count = 0;
    }
  }

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
}

void disp_90() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する

}

//------------------------------------------------------------

void oled_menu_gamen_lav() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  // 日本語表示
  canvas.setCursor(0, 0);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("手動設定");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 16);                        // 座標を指定（x, y）
  //  canvas.setFont(&fonts::lgfxJapanMincho_16);     // 明朝体P（8,12,16,20,24,28,32,36,40）
  canvas.println("セットしない");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 32);                        // 座標を指定（x, y）
  canvas.println("作戦 set");                // 表示内容をcanvasに準備
  canvas.setCursor(14, 48);                        // 座標を指定（x, y）
  canvas.println("READY set");                // 表示内容をcanvasに準備
  canvas.drawLine(0, 16, 128, 15, TFT_WHITE);  // 線（始点x,始点y,終点x,終点y,色）
  canvas.fillCircle(8, 8 + (ui_screen_sentaku1 + 1) * 16, 6, TFT_WHITE); // 塗り潰し円（始点x,始点y,半径,色）

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
}

void oled_ready_set_gamen_lav() {

}

void oled_ready_sakusen_gamen_lav() {

}


void oled_stop_state_lav() {
  canvas.fillScreen(TFT_BLACK);    // 背景塗り潰し
  canvas.setTextColor(TFT_WHITE);  // 文字色と背景を指定（文字色, 背景）
  // 日本語表示
  canvas.setCursor(0, 0);                         // 座標を指定（x, y）
  canvas.setFont(&fonts::lgfxJapanGothic_16);     // ゴシック体（8,12,16,20,24,28,32,36,40）
  canvas.println("液晶表示ゴシック");                // 表示内容をcanvasに準備
  canvas.setCursor(0, 14);                        // 座標を指定（x, y）
  // canvas.setFont(&fonts::lgfxJapanMincho_16);     // 明朝体P（8,12,16,20,24,28,32,36,40）
  canvas.println("液晶表示 明朝体");                // 表示内容をcanvasに準備

  canvas.setCursor(65, 32, &Font2);   // 座標とフォントを指定（x, y, フォント）
  canvas.print("CNT=");               //「CNT=」表示
  canvas.setTextSize(0.3);            // 文字倍率変更
  canvas.setCursor(100, 32, &Font7);  // 座標とフォントを指定（x, y, フォント）
  canvas.printf("%03d", 0);         // カウント数表示
  canvas.setTextSize(1);              // 文字倍率を戻す
  // 線
  canvas.drawLine(0, 30, 128, 30, TFT_WHITE);  // 線（始点x,始点y,終点x,終点y,色）
  canvas.drawFastVLine(60, 31, 16, TFT_WHITE); // 線（始点x,始点y,始点からの垂線長さ,色）
  canvas.drawFastHLine(0, 47, 128, TFT_WHITE); // 線（始点x,始点y,始点からの平行線長さ,色）
  // 円
  canvas.drawCircle(7, 57, 6, TFT_WHITE);      // 円（始点x,始点y,半径,色）
  canvas.fillCircle(23, 57, 6, TFT_WHITE);     // 塗り潰し円（始点x,始点y,半径,色）
  // 三角
  canvas.drawTriangle(34, 62, 41, 50, 47, 62, TFT_WHITE);  // 三角（点1 x,点1 y,点2 x,点2 y,点3 x,点3 y,色）
  canvas.fillTriangle(51, 62, 58, 50, 65, 62, TFT_WHITE);  // 塗り潰し三角（点1 x,点1 y,点2 x,点2 y,点3 x,点3 y,色）
  // 円
  canvas.drawRect(70, 51, 12, 12, TFT_WHITE);  // 四角（始点x,始点y,縦長さ,横長さ）
  canvas.fillRect(86, 51, 12, 12, TFT_WHITE);  // 塗り潰し四角（始点x,始点y,縦長さ,横長さ）

  canvas.pushSprite(0, 0);    // メモリ内に描画したcanvasを座標を指定して表示する
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

void oled_wait_state() {
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







void oled_disp_robo_sens_ver3() {

}


void testdrawchar(void) {

  int offset = 0;
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
