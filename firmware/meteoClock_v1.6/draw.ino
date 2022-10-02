void drawSensors() {
#if (DISPLAY_TYPE == 1)
  // дисплей 2004 ----------------------------------

  if (mode0scr != 2) {                        // Температура (с)НР ----------------------------
    lcd.setCursor(0, 2);
    if (bigDig) {
      if (mode0scr == 1) {
        lcd.setCursor(15, 2);
      }
      
      if (mode0scr != 1) {
        lcd.setCursor(15, 0);
      }
    }
  
    lcd.print(String(dispTemp, 1));
    lcd.write(223);
  } else {
    drawTemp(dispTemp, 0, 0);
  }

  if (mode0scr != 4) {                        // Влажность (с)НР ----------------------------
    lcd.setCursor(5, 2);
    if (bigDig) {
      lcd.setCursor(15, 1);
    }
    
    lcd.print(" " + String(dispHum) + F("% "));
  } else {
    drawHum(dispHum, 0, 0);
  }

#if (CO2_SENSOR == 1)
  if (mode0scr != 1) {                       // СО2 (с)НР ----------------------------
    if (bigDig) {
      lcd.setCursor(15, 2);
      lcd.print(String(dispCO2) + F("p"));
    } else {
      lcd.setCursor(11, 2);
      lcd.print(String(dispCO2) + F("ppm "));
    }
  } else {
    drawPPM(dispCO2, 0, 0);
  }
#endif

  if (mode0scr != 3) {                      // Давление (с)НР ---------------------------
    lcd.setCursor(0, 3);
    if (bigDig && mode0scr == 0) {
      lcd.setCursor(15, 3);
    }

    if (bigDig && (mode0scr == 1 || mode0scr == 2)) {
      lcd.setCursor(15, 0);
    }

    if (bigDig && mode0scr == 4) {
      lcd.setCursor(15, 1);
    }

    if (!(bigDig && mode0scr == 1)) {
      lcd.print(String(dispPres) + F("mm"));
    }
  } else {
    drawPres(dispPres, 0, 0);
  }

  if (!bigDig) {                            // дождь (с)НР -----------------------------
    lcd.setCursor(5, 3);
    lcd.print(F(" rain     "));
    lcd.setCursor(11, 3);
    if (dispRain < 0) {      
      lcd.setCursor(10, 3);      
    }

    lcd.print(String(dispRain) + F("%"));
  }

  if (mode0scr != 0) {                      // время (с)НР ----------------------------
    lcd.setCursor(15, 3);
    if (hrs / 10 == 0) {
      lcd.print(F(" "));
    }

    lcd.print(hrs);
    lcd.print(F(":"));
    if (mins / 10 == 0) {
      lcd.print(F("0"));
    }

    lcd.print(mins);
  } else {
    drawClock(hrs, mins, 0, 0); //, 1);
  }
#else

  // дисплей 1602 ----------------------------------
  if (!bigDig) {              // если только мелкими цифрами (с)НР
    lcd.setCursor(0, 0);
    lcd.print(String(dispTemp, 1));
    lcd.write(223);
    lcd.setCursor(6, 0);
    lcd.print(String(dispHum) + F("% "));

#if (CO2_SENSOR == 1)
    lcd.print(String(dispCO2) + F("ppm"));
    if (dispCO2 < 1000) {
      lcd.print(F(" "));
    }
#endif

    lcd.setCursor(0, 1);
    lcd.print(String(dispPres) + F(" mm  rain "));
    lcd.print(String(dispRain) + F("% "));
  } else {                    // для крупных цифр (с)НР
    switch (mode0scr) {
      case 0:
        drawClock(hrs, mins, 0, 0);
        break;
      case 1:
#if (CO2_SENSOR == 1)
        drawPPM(dispCO2, 0, 0);
#endif
        break;
      case 2:
        drawTemp( f, 2, 0);
        break;
      case 3:
        drawPres(dispPres, 2, 0);
        break;
      case 4:
        drawHum(dispHum, 0, 0);
        break;
    }
  }
#endif
}

void drawDig(byte dig, byte x, byte y) {        // рисуем цифры (с)НР ---------------------------------------
  if (bigDig && DISPLAY_TYPE == 1) {
    switch (dig) {            // четырехстрочные цифры (с)НР
      case 0:
        digSeg(x, y, 255, 0, 255, 255, 32, 255);
        digSeg(x, y + 2, 255, 32, 255, 255, 3, 255);
        break;
      case 1:
        digSeg(x, y, 32, 255, 32, 32, 255, 32);
        digSeg(x, y + 2, 32, 255, 32, 32, 255, 32);
        break;
      case 2:
        digSeg(x, y, 0, 0, 255, 1, 1, 255);
        digSeg(x, y + 2, 255, 2, 2, 255, 3, 3);
        break;
      case 3:
        digSeg(x, y, 0, 0, 255, 1, 1, 255);
        digSeg(x, y + 2, 2, 2, 255, 3, 3, 255);
        break;
      case 4:
        digSeg(x, y, 255, 32, 255, 255, 1, 255);
        digSeg(x, y + 2, 2, 2, 255, 32, 32, 255);
        break;
      case 5:
        digSeg(x, y, 255, 0, 0, 255, 1, 1);
        digSeg(x, y + 2, 2, 2, 255, 3, 3, 255);
        break;
      case 6:
        digSeg(x, y, 255, 0, 0, 255, 1, 1);
        digSeg(x, y + 2, 255, 2, 255, 255, 3, 255);
        break;
      case 7:
        digSeg(x, y, 0, 0, 255, 32, 32, 255);
        digSeg(x, y + 2, 32, 255, 32, 32, 255, 32);
        break;
      case 8:
        digSeg(x, y, 255, 0, 255, 255, 1, 255);
        digSeg(x, y + 2, 255, 2, 255, 255, 3, 255);
        break;
      case 9:
        digSeg(x, y, 255, 0, 255, 255, 1, 255);
        digSeg(x, y + 2, 2, 2, 255, 3, 3, 255);
        break;
      case 10:
        digSeg(x, y, 32, 32, 32, 32, 32, 32);
        digSeg(x, y + 2, 32, 32, 32, 32, 32, 32);
        break;
    }
  } else {
    switch (dig) {            // двухстрочные цифры
      case 0:
        digSeg(x, y, 255, 1, 255, 255, 2, 255);
        break;
      case 1:
        digSeg(x, y, 32, 255, 32, 32, 255, 32);
        break;
      case 2:
        digSeg(x, y, 3, 3, 255, 255, 4, 4);
        break;
      case 3:
        digSeg(x, y, 3, 3, 255, 4, 4, 255);
        break;
      case 4:
        digSeg(x, y, 255, 0, 255, 5, 5, 255);
        break;
      case 5:
        digSeg(x, y, 255, 3, 3, 4, 4, 255);
        break;
      case 6:
        digSeg(x, y, 255, 3, 3, 255, 4, 255);
        break;
      case 7:
        digSeg(x, y, 1, 1, 255, 32, 255, 32);
        break;
      case 8:
        digSeg(x, y, 255, 3, 255, 255, 4, 255);
        break;
      case 9:
        digSeg(x, y, 255, 3, 255, 4, 4, 255);
        break;
      case 10:
        digSeg(x, y, 32, 32, 32, 32, 32, 32);
        break;
    }
  }
}

void drawPPM(int dispCO2, byte x, byte y) {     // Уровень СО2 крупно на главном экране (с)НР ----------------------------
  dispCO2 / 1000 == 0 ? drawDig(10, x, y) : drawDig(dispCO2 / 1000, x, y);
  drawDig((dispCO2 % 1000) / 100, x + 4, y);
  drawDig((dispCO2 % 100) / 10, x + 8, y);
  drawDig(dispCO2 % 10 , x + 12, y);
  lcd.setCursor(15, 0);
#if (DISPLAY_TYPE == 1)
  lcd.print(F("ppm"));
#else
  lcd.print(F("p"));
#endif
}

void drawPres(int dispPres, byte x, byte y) {   // Давление крупно на главном экране (с)НР ----------------------------
  drawDig((dispPres % 1000) / 100, x , y);
  drawDig((dispPres % 100) / 10, x + 4, y);
  drawDig(dispPres % 10 , x + 8, y);
  lcd.setCursor(x + 11, 1);
  if (bigDig) {
    lcd.setCursor(x + 11, 3);
  }

  lcd.print(F("mm"));
}

void drawTemp(float dispTemp, byte x, byte y) { // Температура крупно на главном экране (с)НР ----------------------------
  dispTemp / 10 == 0 ? drawDig(10, x, y) : drawDig(dispTemp / 10, x, y);
  drawDig(int(dispTemp) % 10, x + 4, y);
  drawDig(int(dispTemp * 10.0) % 10, x + 9, y);
  if (bigDig && DISPLAY_TYPE == 1) {
    lcd.setCursor(x + 7, y + 3);
    lcd.write(1);             // десятичная точка
  } else {
    lcd.setCursor(x + 7, y + 1);
    lcd.write(0B10100001);    // десятичная точка
  }

  lcd.setCursor(x + 13, y);
  lcd.write(223);             // градусы
}

void drawHum(int dispHum, byte x, byte y) {   // Влажность крупно на главном экране (с)НР ----------------------------
  dispHum / 100 == 0 ? drawDig(10, x, y) : drawDig(dispHum / 100, x, y);
  (dispHum % 100) / 10 == 0 ? drawDig(0, x + 4, y) : drawDig(dispHum / 10, x + 4, y);
  drawDig(int(dispHum) % 10, x + 8, y);
  if (bigDig && DISPLAY_TYPE == 1) {
    lcd.setCursor(x + 12, y + 1);
    lcd.print(F("\245\4"));
    lcd.setCursor(x + 12, y + 2);
    lcd.print(F("\5\245"));
  } else {
    lcd.setCursor(x + 12, y + 1);
    lcd.print(F("%"));
  }
}

void drawClock(byte hours, byte minutes, byte x, byte y) {    // рисуем время крупными цифрами -------------------------------------------
  if (hours > 23 || minutes > 59) {
    return;
  }

  hours / 10 == 0 ? drawDig(10, x, y) : drawDig(hours / 10, x, y);
  drawDig(hours % 10, x + 4, y);
  // тут должны быть точки. Отдельной функцией
  drawDig(minutes / 10, x + 8, y);
  drawDig(minutes % 10, x + 12, y);
}

#if (WEEK_LANG == 0)
static const char *dayNames[]  = {
  "Su",
  "Mo",
  "Tu",
  "We",
  "Th",
  "Fr",
  "Sa",
};
#else
static const char *dayNames[]  = {  // доработал дни недели на двухсимвольные русские (ПН, ВТ, СР....) (с)НР
  "BC",
  "\7H",
  "BT",
  "CP",
  "\7T",
  "\7T",
  "C\7",
};
#endif

void drawData() {                     // выводим дату -------------------------------------------------------------
  if (!bigDig) {              // если 4-х строчные цифры, то дату, день недели (и секунды) не пишем - некуда (с)НР
    lcd.setCursor(15, 0 + (DISPLAY_TYPE == 1 && mode0scr == 1 ? 2 : 0));
    if (now.day() < 10) {
      lcd.print(0);
    }

    lcd.print(now.day());
    lcd.print(F("."));
    if (now.month() < 10) {
      lcd.print(0);
    }

    lcd.print(now.month());
    if (DISP_MODE == 0) {
      lcd.setCursor(16, 1);
      lcd.print(now.year());
    } else {
      loadClock();              // принудительно обновляем знаки, т.к. есть жалобы на необновление знаков в днях недели (с)НР
      lcd.setCursor(18, 1);
      lcd.print(dayNames[now.dayOfTheWeek()]);
    }
  }
}

void drawPlot(byte pos, byte row, byte width, byte height, int min_val, int max_val, int *plot_array, String label1, String label2, int stretch) {  // график ---------------------------------
  int max_value = -32000;
  int min_value = 32000;

  for (byte i = 0; i < 15; i++) {
    max_value = max(plot_array[i] , max_value);
    min_value = min(plot_array[i] , min_value);
  }

  // меняем пределы графиков на предельные/фактические значения, одновременно рисуем указатель пределов (стрелочки вверх-вниз) (с)НР
  lcd.setCursor(15, 0);
  if ((MAX_ONDATA & (1 << (stretch - 1))) > 0) {    // побитовое сравнение 1 - растягиваем, 0 - не растягиваем (по указанным пределам) (с)НР
    max_val = max_value;
    min_val = min_value;
#if (DISPLAY_TYPE == 1)
    lcd.write(0b01011110);
    lcd.setCursor(15, 3);
    lcd.write(0);
#endif
  } else {
#if (DISPLAY_TYPE == 1)
    lcd.write(0);
    lcd.setCursor(15, 3);
    lcd.write(0b01011110);
#endif
  }

  if (min_val >= max_val) {
    max_val = min_val + 1;
  }
#if (DISPLAY_TYPE == 1)
  lcd.setCursor(15, 1); lcd.write(0b01111100);
  lcd.setCursor(15, 2); lcd.write(0b01111100);
  lcd.setCursor(16, 0); lcd.print(max_value);
  lcd.setCursor(16, 1); lcd.print(label1); lcd.print(label2);
  lcd.setCursor(16, 2); lcd.print(plot_array[14]);
  lcd.setCursor(16, 3); lcd.print(min_value);
#else
  lcd.setCursor(12, 0); lcd.print(label1);
  lcd.setCursor(13, 0); lcd.print(max_value);
  lcd.setCursor(12, 1); lcd.print(label2);
  lcd.setCursor(13, 1); lcd.print(min_value);
#endif
  for (byte i = 0; i < width; i++) {                  // каждый столбец параметров
    int fill_val = plot_array[i];
    fill_val = constrain(fill_val, min_val, max_val);
    byte infill, fract;
    // найти количество целых блоков с учётом минимума и максимума для отображения на графике
    infill = (plot_array[i]) > min_val ? floor((float)(plot_array[i] - min_val) / (max_val - min_val) * height * 10) : 0;
    fract = (float)(infill % 10) * 8 / 10;            // найти количество оставшихся полосок
    infill = infill / 10;
    for (byte n = 0; n < height; n++) {     // для всех строк графика
      if (n < infill && infill > 0) {       // пока мы ниже уровня
        lcd.setCursor(i, (row - n));        // заполняем полными ячейками
        lcd.write(255);
      }

      if (n >= infill) {                    // если достигли уровня
        lcd.setCursor(i, (row - n));
        if (n == 0 && fract == 0) {
          fract++;      // если нижний перел графика имеет минимальное значение, то рисуем одну полоску, чтобы не было пропусков (с)НР
        }
        
        lcd.write(fract > 0 ? fract : 16);        // заполняем дробные ячейки        
        for (byte k = n + 1; k < height; k++) { // всё что сверху заливаем пустыми
          lcd.setCursor(i, (row - k));
          lcd.write(16);
        }

        break;
      }
    }
  }
}