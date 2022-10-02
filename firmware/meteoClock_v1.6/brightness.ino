bool isScheduleTime() {
  if (schedulePowerStatus == 0) {
    return false;
  }

  if (SCHEDULE_POWER_START < SCHEDULE_POWER_END) {
    return (hrs >= SCHEDULE_POWER_START && hrs < SCHEDULE_POWER_END);
  }

  return (hrs >= SCHEDULE_POWER_START || hrs < SCHEDULE_POWER_END);
}

void checkBrightness() {
  if (isScheduleTime()) {                    // если установлен автоматический режим отключение дисплея
    analogWrite(BACKLIGHT, 0);
    LED_ON = (0);
  } else {
    backlightBrightness();
    ledBrightness();
  }
}

void backlightBrightness() {
  if (LCD_BRIGHT == 11) {                       // если установлен автоматический режим для экрана (с)НР
    analogWrite(BACKLIGHT, analogRead(PHOTO) < BRIGHT_THRESHOLD ? LCD_BRIGHT_MIN : LCD_BRIGHT_MAX);
  } else {
    analogWrite(BACKLIGHT, LCD_BRIGHT * LCD_BRIGHT * 2.5);
  }
}

void ledBrightness() {
  if (analogRead(PHOTO) < BRIGHT_THRESHOLD) {   // если темно
#if (LED_MODE == 0)
    LED_ON = (LED_BRIGHT_MIN);
#else
    LED_ON = (255 - LED_BRIGHT_MIN);
#endif
  } else {                                     // если светло
#if (LED_MODE == 0)
    LED_ON = (LED_BRIGHT_MAX);
#else
    LED_ON = (255 - LED_BRIGHT_MAX);
#endif
  }
}