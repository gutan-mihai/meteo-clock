/*
  mode:
  0 - Главный экран
  1-8 - Графики: СО2 (час, день), Влажность (час, день), Температура (час, день), Осадки (час, день)
  251 - выбор режима отк. дисплея (podMode от 0 до 1: включить/отключить)
  252 - выбор режима индикатора (podMode от 0 до 3: индикация СО2, влажности, температуры, осадков)
  253 - настройка яркости экрана (podMode от 0 до 11: выбор от 0% до 100% или автоматическое регулирование)
  254 - настройка яркости индикатора (podMode от 0 до 11: выбор от 0% до 100% или автоматическое регулирование)
  255 - главное меню (podMode от 1 до 18:
                                        1 - Сохранить
                                        2 - Выход
                                        3 - Ярк.индикатора
                                        4 - Ярк.экрана
                                        5 - Режим индикатора
                                        6 - Режим отключения дисплея
                                        7 - Режим установки времени
                                        8-15 вкл/выкл графики: СО2 (час, день), Влажность (час, день), Температура (час, день), Осадки (час, день)
*/
void modesTick() {
  button.tick();
  boolean changeFlag = false;
  if (button.isSingle()) {    // одинарное нажатие на кнопку
    if (mode >= 240) {
      switch (mode) {
        case 250:             // Перебираем все варианты режима установки времени
          if (podMode > 3) {
            podMode = 0;
          }

          if (podMode == 2) {
            manualHours++;
            if (manualHours > 23) {
              manualHours = 0;
            }
          }

          if (podMode == 3) {
            manualMinutes++;
            if (manualMinutes > 59) {
              manualMinutes = 0;
            }
          }

          settingsTimeManual = podMode;
          changeFlag = true;
          break;
        case 251:             // Перебираем все варианты режима отк. дисплея
          podMode++;
          if (podMode > 1) {
            podMode = 0;
          }

          schedulePowerStatus = podMode;
          changeFlag = true;
          break;
        case 252:             // Перебираем все варианты режимов LED индикатора (с)НР
          podMode++;
          if (podMode > 3) {
            podMode = 0;
          }

          LEDType = podMode;
          changeFlag = true;
          break;
        case 253:             // Перебираем все варианты яркости LCD экрана (с)НР
          podMode++;
          if (podMode > 11) {
            podMode = 0;
          }

          LCD_BRIGHT = podMode;
          checkBrightness();
          changeFlag = true;
          break;
        case 254:             // Перебираем все варианты яркости LED индикатора (с)НР
          podMode++;
          if (podMode > 11) {
            podMode = 0;
          }

          LED_BRIGHT = podMode;
          changeFlag = true;
          break;
        case 255:             // Перебираем все варианты основных настроек (с)НР
          podMode++;
          if (podMode > 14) {
            podMode = 1;
          }
          
          changeFlag = true;
          break;
      }
    } else {
      do {
        mode++;
        if (mode > 10) {
          mode = 0;
        }

#if (CO2_SENSOR == 0 && mode == 1)
        mode = 3;
#endif
      } while (((VIS_ONDATA & (1 << (mode - 1))) == 0) && (mode > 0));   // проверка на отображение графиков (с)НР
      changeFlag = true;
    }
  }
  
  if (button.isDouble()) {                    // двойное нажатие (с)НР ----------------------------
    if (mode > 0 && mode < 11) {              // Меняет пределы графика на установленные/фактические максимумы (с)НР
      MAX_ONDATA = (int)MAX_ONDATA ^ (1 << (mode - 1));
    } else if (mode == 0)  {
      mode0scr++;
      if (CO2_SENSOR == 0 && mode0scr == 1) {
        mode0scr++;
      }

      if (mode0scr > 5) {
        mode0scr = 0;         // Переключение рехима работы главного экрана (с)НР
      }
    } else if (mode > 240) {
      if (mode == 250) {
        podMode++;
      } else {
        podMode = 1;       // Переключение на меню сохранения (с)НР
      }
    }
        
    changeFlag = true;
  }

  if ((button.isTriple()) && (mode == 0)) {  // тройное нажатие в режиме главного экрана - переход в меню (с)НР
    mode = 255;
    podMode = 3;
    changeFlag = true;
  }

  if (button.isHolded()) {    // удержание кнопки (с)НР
    switch (mode) {
      case 0:
        bigDig = !bigDig;
        break;
      case 250:       // реж. установки времени
      case 251:       // реж. отк. дисплея
      case 252:       // реж. индикатора
      case 253:       // ярк. экрана
      case 254:       // ярк. индикатора
        mode = 255;
        podMode = 1;
        break;
      case 255:       // главное меню
        if (podMode == 2 || podMode == 1) {
          mode = 0;                   // если Выход или Сохранить
        }

        if (podMode >= 3 && podMode <= 5) {
          mode = 255 - podMode + 2;   // если настройки яркостей, то переключаемся в настройки пункта меню
        }
        
        if (podMode == 6) {
          mode = 251;                 // если выбран режим отк. дисплея
        }
        
        if (podMode == 7) {
          mode = 250;                 // если выбран режим установки времени
        }

        if (podMode >= 8 && podMode <= 15) {
          VIS_ONDATA = VIS_ONDATA ^ (1 << (podMode - 8));  // вкл/выкл отображения графиков
        }

        if (podMode == 1) {           // если Сохранить
          if (EEPROM.read(2) != (MAX_ONDATA & 255)) {
            EEPROM.write(2, (MAX_ONDATA & 255));
          }

          if (EEPROM.read(3) != (MAX_ONDATA >> 8)) {
            EEPROM.write(3, (MAX_ONDATA >> 8));

          }
          if (EEPROM.read(4) != (VIS_ONDATA & 255)) {
            EEPROM.write(4, (VIS_ONDATA & 255));
          }

          if (EEPROM.read(5) != (VIS_ONDATA >> 8)) {
            EEPROM.write(5, (VIS_ONDATA >> 8));
          }

          if (EEPROM.read(6) != mode0scr) {
            EEPROM.write(6, mode0scr);
          }

          if (EEPROM.read(7) != bigDig) {
            EEPROM.write(7, bigDig);
          }

          if (EEPROM.read(8) != LED_BRIGHT) {
            EEPROM.write(8, LED_BRIGHT);
          }

          if (EEPROM.read(9) != LCD_BRIGHT) {
            EEPROM.write(9, LCD_BRIGHT);
          }

          if (EEPROM.read(10) != LEDType) {
            EEPROM.write(10, LEDType);
          }

          if (EEPROM.read(11) != schedulePowerStatus) {
            EEPROM.write(11, schedulePowerStatus);
          }

          if (EEPROM.read(12) != settingsTimeManual) {
            EEPROM.write(12, settingsTimeManual);
          }

          if (settingsTimeManual > 0) {
            now = rtc.now();
            rtc.adjust(DateTime(now.year(), now.month(), now.day(), manualHours, manualMinutes, now.second()));
          }

          if (EEPROM.read(0) != 122) {
            EEPROM.write(0, 122);
          }
        }
        
        if (podMode < 8) {
          podMode = 1;
        }
        
        if (mode == 250) {
          podMode = settingsTimeManual;       // если выбран режим установки времени
        }

        if (mode == 251) {
          podMode = schedulePowerStatus;    // если выбран режим отк. дисплея
        }

        if (mode == 252) {
          podMode = LEDType;                // если выбран режим LED - устанавливаем текущее значение (с)НР
        }

        if (mode == 254) {
          podMode = LED_BRIGHT;             // если выбрана яркость LED - устанавливаем текущее показание (с)НР
        }
        
        if (mode == 253) {
          podMode = LCD_BRIGHT;             // если выбрана яркость LCD - устанавливаем текущее показание (с)НР          
        }
        break;
      default:
        mode = 0;
    }
    
    changeFlag = true;
  }

  if (changeFlag) {
    if (mode >= 240) {
      lcd.clear();
      lcd.createChar(1, BM);  //Ь
      lcd.createChar(2, IY);  //Й
      lcd.createChar(3, DD);  //Д
      lcd.createChar(4, II);  //И
      lcd.createChar(5, IA);  //Я
      lcd.createChar(6, YY);  //Ы
      lcd.createChar(7, AA);  //Э
      lcd.createChar(0, ZZ);  //Ж
      lcd.setCursor(0, 0);
    }
    
    if (mode == 255) {          // Перебираем варианты в главном меню (с)НР
#if (WEEK_LANG == 1)
      lcd.print(F("HACTPO\2K\4:"));              // ---Настройки
#else
      lcd.print(F("Setup:"));
#endif
      lcd.setCursor(0, 1);
      switch (podMode) {
        case 1:
#if (WEEK_LANG == 1)
          lcd.print(F("COXPAH\4T\1"));     // ---Сохранить
#else
          lcd.print(F("Save"));
#endif
          break;
        case 2:
#if (WEEK_LANG == 1)
          lcd.print(F("B\6XO\3"));         // --- Выход
#else
          lcd.print(F("Exit"));
#endif
          break;
        case 3:
#if (WEEK_LANG == 1)
          lcd.print(F("\5PK.\4H\3\4KATOPA"));  // ---Ярк.индик.
#else
          lcd.print(F("indicator brt."));
#endif
          break;
        case 4:
#if (WEEK_LANG == 1)
          lcd.print(F("\5PK.\7KPAHA"));    // ---Ярк.экрана
#else
          lcd.print(F("Bright LCD"));
#endif
          break;
        case 5:
#if (WEEK_LANG == 1)
          lcd.print(F("PE\10.\4H\3\4KATOPA"));  // ---Реж.индик.
#else
          lcd.print(F("indicator mode"));
#endif
          break;
        case 6:
#if (WEEK_LANG == 1)
          lcd.print(F("OTK.\7KPAH"));  // ---Режим отк. экран
#else
          lcd.print(F("Schedule power"));
#endif
          break;
        case 7:
#if (WEEK_LANG == 1)
          lcd.print(F("YCT.BPEMEH\4"));  // ---Режим установка времени
#else
          lcd.print(F("Setting time"));
#endif
          break;
      }
      
      if (podMode >= 8 && podMode <= 15) {
        lcd.createChar(8, FF);  //ф
        lcd.createChar(7, GG);  //Г
        lcd.createChar(5, LL);  //Л
        lcd.setCursor(10, 0);
#if (WEEK_LANG == 1)
        lcd.print(F("\7PA\10\4KOB"));            // ---графиков
#else
        lcd.print(F("Charts  "));
#endif
        lcd.setCursor(0, 1);
        if ((3 & (1 << (podMode - 8))) != 0) {
          lcd.print(F("CO2 "));
        }
        
        if ((12 & (1 << (podMode - 8))) != 0) {
#if (WEEK_LANG == 1)
          lcd.print(F("B\5,% "));
#else
          lcd.print(F("Hum,%"));
#endif
        }
        
        if ((48 & (1 << (podMode - 8))) != 0) {
          lcd.print(F("t\337 "));
        }

        if ((192 & (1 << (podMode - 8))) != 0) {
          if (PRESSURE) {
            lcd.print(F("p,rain "));
          } else {
            lcd.print(F("p,mmPT "));
          }
        }
        
        if ((768 & (1 << (podMode - 8))) != 0) {
#if (WEEK_LANG == 1)
          lcd.print(F("B\6C,m  "));
#else
          lcd.print(F("hgt,m  "));
#endif
        }

        if ((1365 & (1 << (podMode - 8))) != 0) {
          lcd.createChar(3, CH);  //Ч
          lcd.setCursor(8, 1);
#if (WEEK_LANG == 1)
          lcd.print(F("\3AC:"));
#else
          lcd.print(F("Hour:"));
#endif
        } else {
          lcd.setCursor(7, 1);
#if (WEEK_LANG == 1)
          lcd.print(F("\3EH\1:"));
#else
          lcd.print(F("Day: "));
#endif
        }
        
        if ((VIS_ONDATA & (1 << (podMode - 8))) != 0) {
#if (WEEK_LANG == 1)
          lcd.print(F("BK\5 "));
#else
          lcd.print(F("On  "));
#endif
        } else {
#if (WEEK_LANG == 1)
          lcd.print(F("B\6K\5"));
#else
          lcd.print(F("Off "));
#endif
        }
      }
    }

    if (mode == 250) {                        // --------------------- показать  "Установка времени"
      settingsTimeManual = podMode;
#if (WEEK_LANG == 1)
      lcd.print(F("YCT.BPEMEH\4:"));
#else
      lcd.print(F("Setting time:"));
#endif
      if (podMode == 0) {
#if (WEEK_LANG == 1)
        lcd.print(F("ABTO "));
#else
        lcd.print(F("Auto "));
#endif
      } else {
#if (WEEK_LANG == 1)
        lcd.print(F("BPYCHHYIU "));
#else
        lcd.print(F("Manual "));
#endif
        lcd.setCursor(0, 1);
#if (WEEK_LANG == 1)
        lcd.print(F("\3AC\6: "));
#else
        lcd.print(F("Hours: "));
#endif
        lcd.print(manualHours);
        lcd.setCursor(0, 2);
#if (WEEK_LANG == 1)
        lcd.print(F("M\4HYT\6: "));
#else
        lcd.print(F("Minutes: "));
#endif
        lcd.print(manualMinutes);
      }
    }

    if (mode == 251) {                        // --------------------- показать  "Отк. дисплея"
      schedulePowerStatus = podMode;
#if (WEEK_LANG == 1)
      lcd.print(F("OTK.\7KPAH:"));
#else
      lcd.print(F("Schedule power:"));
#endif
      if (podMode == 0) {
#if (WEEK_LANG == 1)
        lcd.print(F("B\6K\5 "));
#else
        lcd.print(F("Off "));
#endif
      } else {
#if (WEEK_LANG == 1)
        lcd.print(F("BK\5 "));
#else
        lcd.print(F("On "));
#endif
      }
    }

    if (mode == 252) {                        // --------------------- показать  "Реж.индикатора"
      LEDType = podMode;
      lcd.createChar(6, LL);  //Л
      lcd.createChar(3, DD);  //Д
      lcd.createChar(5, II);  //И
      lcd.createChar(8, ZZ);  //Ж
      lcd.setCursor(0, 0);
#if (WEEK_LANG == 1)
      lcd.print(F("PE\10.\4H\3\4KATOPA:"));
#else
      lcd.print(F("indicator mode:"));
#endif
      lcd.setCursor(0, 1);
      switch (podMode) {
        case 0:
          lcd.print(F("CO2   "));
          break;
        case 1:
#if (WEEK_LANG == 1)
          lcd.print(F("B\6A\10H."));          // влажн.
#else
          lcd.print(F("Humid."));
#endif
          break;
        case 2:
          lcd.print(F("t\337     "));
          break;
        case 3:
#if (WEEK_LANG == 1)
          lcd.print(F("OCA\3K\5"));          // осадки
#else
          lcd.print(F("rain  "));
#endif
          break;
        case 4:
#if (WEEK_LANG == 1)
          lcd.print(F("\3AB\6EH\5E"));       // давление
#else
          lcd.print(F("pressure"));
#endif
          break;
      }
    }
    
    if (mode == 253) {                        // --------------------- показать  "Ярк.экрана"
#if (WEEK_LANG == 1)
      lcd.print(F("\5PK.\7KPAHA:"));
#else
      lcd.print(F("Bright LCD:"));
#endif
      if (LCD_BRIGHT == 11) {
#if (WEEK_LANG == 1)
        lcd.print(F("ABTO "));
#else
        lcd.print(F("Auto "));
#endif
      } else {
        lcd.print(String(LCD_BRIGHT * 10) + F("%"));
      }
    }
    
    if (mode == 254) {                        // --------------------- показать  "Ярк.индикатора"
#if (WEEK_LANG == 1)
      lcd.print(F("\5PK.\4H\3\4K.:"));
#else
      lcd.print(F("indic.brt.:"));
#endif
      if (LED_BRIGHT == 11) {
#if (WEEK_LANG == 1)
        lcd.print(F("ABTO "));
#else
        lcd.print(F("Auto "));
#endif
      } else {
        lcd.print(String(LED_BRIGHT * 10) +F( "%"));
      }
    }

    if (mode == 0) {
      lcd.clear();
      loadClock();
      drawSensors();
      if (DISPLAY_TYPE == 1) {
        drawData();
      }
    } else if (mode <= 10) {
      loadPlot();
      redrawPlot();
    }
  }
}

void redrawPlot() {
  lcd.clear();
#if (DISPLAY_TYPE == 1)       // для дисплея 2004
  switch (mode) {             // добавлена переменная для "растягивания" графика до фактических максимальных и(или) минимальных значений(с)НР
    case 1:
      drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Hour, "c ", "hr", mode);
      break;
    case 2:
      drawPlot(0, 3, 15, 4, CO2_MIN, CO2_MAX, (int*)co2Day, "c ", "da", mode);
      break;
    case 3:
      drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humHour, "h%", "hr", mode);
      break;
    case 4:
      drawPlot(0, 3, 15, 4, HUM_MIN, HUM_MAX, (int*)humDay, "h%", "da", mode);
      break;
    case 5:
      drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempHour, "t\337", "hr", mode);
      break;
    case 6:
      drawPlot(0, 3, 15, 4, TEMP_MIN, TEMP_MAX, (int*)tempDay, "t\337", "da", mode);
      break;
    case 7:
      drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressHour, "p ", "hr", mode);
      break;
    case 8:
      drawPlot(0, 3, 15, 4, PRESS_MIN, PRESS_MAX, (int*)pressDay, "p ", "da", mode);
      break;
  }
#else                         // для дисплея 1602
  switch (mode) {
    case 1:
      drawPlot(0, 1, 12, 2, CO2_MIN, CO2_MAX, (int*)co2Hour, "c", "h", mode);
      break;
    case 2:
      drawPlot(0, 1, 12, 2, CO2_MIN, CO2_MAX, (int*)co2Day, "c", "d", mode);
      break;
    case 3:
      drawPlot(0, 1, 12, 2, HUM_MIN, HUM_MAX, (int*)humHour, "h", "h", mode);
      break;
    case 4:
      drawPlot(0, 1, 12, 2, HUM_MIN, HUM_MAX, (int*)humDay, "h", "d", mode);
      break;
    case 5:
      drawPlot(0, 1, 12, 2, TEMP_MIN, TEMP_MAX, (int*)tempHour, "t", "h", mode);
      break;
    case 6:
      drawPlot(0, 1, 12, 2, TEMP_MIN, TEMP_MAX, (int*)tempDay, "t", "d", mode);
      break;
    case 7:
      drawPlot(0, 1, 12, 2, PRESS_MIN, PRESS_MAX, (int*)pressHour, "p", "h", mode);
      break;
    case 8:
      drawPlot(0, 1, 12, 2, PRESS_MIN, PRESS_MAX, (int*)pressDay, "p", "d", mode);
      break;
  }
#endif
}

void readSensors() {
  bme.takeForcedMeasurement();
  dispTemp = bme.readTemperature();
  dispHum = bme.readHumidity();
  dispPres = (float)bme.readPressure() * 0.00750062;
#if (CO2_SENSOR == 1)
  dispCO2 = mhz19.getPPM();
#else
  dispCO2 = 0;
#endif
}

void plotSensorsTick() {
  // 4 или 5 минутный таймер
  if (testTimer(hourPlotTimerD, hourPlotTimer)) {
    for (byte i = 0; i < 14; i++) {
      tempHour[i] = tempHour[i + 1];
      humHour[i] = humHour[i + 1];
      pressHour[i] = pressHour[i + 1];
      co2Hour[i] = co2Hour[i + 1];
    }

    tempHour[14] = dispTemp;
    humHour[14] = dispHum;
    pressHour[14] = dispPres;
    co2Hour[14] = dispCO2;
    pressHour[14] = PRESSURE ? dispRain : dispPres;
  }

  // 1.5 или 2 часовой таймер
  if (testTimer(dayPlotTimerD, dayPlotTimer)) {
    long averTemp = 0, averHum = 0, averPress = 0, averCO2 = 0;
    for (byte i = 0; i < 15; i++) {
      averTemp += tempHour[i];
      averHum += humHour[i];
      averPress += pressHour[i];
      averCO2 += co2Hour[i];
    }

    averTemp /= 15;
    averHum /= 15;
    averPress /= 15;
    averCO2 /= 15;

    for (byte i = 0; i < 14; i++) {
      tempDay[i] = tempDay[i + 1];
      humDay[i] = humDay[i + 1];
      pressDay[i] = pressDay[i + 1];
      co2Day[i] = co2Day[i + 1];
    }
 
    tempDay[14] = averTemp;
    humDay[14] = averHum;
    pressDay[14] = averPress;
    co2Day[14] = averCO2;
  }

  // 10 минутный таймер
  if (testTimer(predictTimerD, predictTimer)) {
    // тут делаем линейную аппроксимацию для предсказания погоды
    long averPress = 0;
    for (byte i = 0; i < 10; i++) {
      bme.takeForcedMeasurement();
      averPress += bme.readPressure();
      delay(1);
    }
    
    averPress /= 10;
    for (byte i = 0; i < 5; i++) {                   // счётчик от 0 до 5 (да, до 5. Так как 4 меньше 5)
      pressure_array[i] = pressure_array[i + 1];     // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
    }
    
    pressure_array[5] = averPress;                   // последний элемент массива теперь - новое давление
    sumX = 0;
    sumY = 0;
    sumX2 = 0;
    sumXY = 0;
    for (int i = 0; i < 6; i++) {                    // для всех элементов массива
      sumX += i;
      sumY += (long)pressure_array[i];
      sumX2 += i * i;
      sumXY += (long)i * pressure_array[i];
    }
    
    a = 0;
    a = (long)6 * sumXY;                            // расчёт коэффициента наклона приямой
    a = a - (long)sumX * sumY;
    a = (float)a / (6 * sumX2 - sumX * sumX);
    delta = a * 6;      // расчёт изменения давления
    dispRain = map(delta, -250, 250, 100, -100);    // пересчитать в проценты
  }
}

boolean dotFlag;
void clockTick() {
  dotFlag = !dotFlag;
  if (dotFlag) {            // каждую секунду пересчёт времени
    secs++;
    if (secs > 59) {        // каждую минуту
      secs = 0;
      mins++;
      if (mins <= 59 && mode == 0) {
        drawSensors();      // (с)НР
      }
    }
    if (mins > 59) {        // каждый час
      now = rtc.now();
      secs = now.second();
      mins = now.minute();
      hrs = now.hour();
      if (mode == 0) {
        drawSensors();
      }

      if (hrs > 23) {
        hrs = 0;
      }

      if (mode == 0 && DISPLAY_TYPE) {
        drawData();
      }
    }
    if ((DISP_MODE != 0 && mode == 0) && DISPLAY_TYPE == 1 && !bigDig) {   // Если режим секунд или дни недели по-русски, и 2-х строчные цифры то показывать секунды (с)НР
      lcd.setCursor(15, 1);
      if (secs < 10) {
        lcd.print(F(" "));
      }

      lcd.print(secs);
    }
  }

  if (mode == 0) {                                                     // Точки и статус питания (с)НР
    if (!bigDig && powerStatus != 255 && DISPLAY_TYPE == 1) {          // отображаем статус питания (с)НР
      if (analogRead(A1) > 900 || analogRead(A0) < 300 || (analogRead(A1) < 300 && analogRead(A0) < 300)) {
        powerStatus = 0;
      } else {
        powerStatus = (constrain((int)analogRead(A0) * 5.2 / 1023.0, 3.0, 4.2) - 3.0) / ((4.2 - 3.0) / 6.0) + 1;
      }

      if (powerStatus) {
        for (byte i = 2; i <= 6; i++) {         // рисуем уровень заряда батареи (с)НР
          DC[i] = ((7 - powerStatus) < i) ? 0b11111 : 0b10001;
        }

        lcd.createChar(6, DC);
      } else {
        lcd.createChar(6, AC);
      }

      lcd.setCursor(19, mode0scr != 1 ? 2 : 0);
      lcd.write(!dotFlag && powerStatus == 1 ? 32 : 6);
    }

    byte code;
    code = dotFlag ? 165 : 32;
    if (mode0scr == 0 && (bigDig && DISPLAY_TYPE == 0 || DISPLAY_TYPE == 1)) {          // мигание большими точками только в нулевом режиме главного экрана (с)НР
      lcd.setCursor(7, bigDig && DISPLAY_TYPE == 1 ? 2 : 0);
      lcd.write(code);
      lcd.setCursor(7, 1);
      lcd.write(code);
    } else {
#if (DISPLAY_TYPE == 1)
      if (code == 165) {
        code = 58;
      }

      lcd.setCursor(17, 3);
      lcd.write(code);
#endif
    }
  }

  if ((dispCO2 >= blinkLEDCO2 && LEDType == 0 || dispHum <= blinkLEDHum && LEDType == 1 || dispTemp >= blinkLEDTemp && LEDType == 2) && !dotFlag) {
    setLEDcolor(0);     // мигание индикатора в зависимости от значения и привязанного сенсора (с)НР
  } else {
    setLED();
  }
}

boolean testTimer(unsigned long & dataTimer, unsigned long setTimer) {   // Проверка таймеров (с)НР
  if (millis() - dataTimer >= setTimer) {
    dataTimer = millis();

    return true;
  }
  
  return false;
}