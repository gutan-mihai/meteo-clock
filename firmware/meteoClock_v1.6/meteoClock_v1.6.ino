/*
  Скетч к проекту "Домашняя метеостанция"
  Страница проекта (схемы, описания): https://alexgyver.ru/meteoclock/
  Исходники на GitHub: https://github.com/AlexGyver/MeteoClock
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
  Доработал Роман Новиков (с)НР
  обновления здесь: https://github.com/Norovl/meteoClock
*/

/*
  Время и дата устанавливаются атвоматически при загрузке прошивки (такие как на компьютере)
  График всех величин за час и за сутки (усреднённые за каждый час)
  В модуле реального времени стоит батарейка, которая продолжает отсчёт времени после выключения/сброса питания
  Как настроить время на часах. У нас есть возможность автоматически установить время на время загрузки прошивки, поэтому:
	- Ставим настройку RESET_CLOCK на 1
  - Прошиваемся
  - Сразу ставим RESET_CLOCK 0
  - И прошиваемся ещё раз
  - Всё
*/

/* Версия 1.5
  - Добавлено управление яркостью
  - Яркость дисплея и светодиода СО2 меняется на максимальную и минимальную в зависимости от сигнала с фоторезистора
  Подключите датчик (фоторезистор) по схеме. Теперь на экране отладки справа на второй строчке появится величина сигнала
  с фоторезистора.
*/

/* Версия 1.6
  - Добавлено возможность редактировать время
  - Добавлено возможность отключить дисплей ночью
*/
// ------------------------- НАСТРОЙКИ --------------------
#define RESET_CLOCK 0                   // сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define SENS_TIME 10000                 // время обновления показаний сенсоров на экране, миллисекунд
#define CLOCK_TIMER 500                 // два раза в секунду пересчитываем время и мигаем точками
#define BRIGHT_TIMER 2000               // время обновления яркость экрана
#define LED_MODE 0                      // тип RGB светодиода: 0 - главный катод, 1 - главный анод
#define SEALEVELPRESSURE_HPA (1013.25)  // Коэффициент для расчета высоты над уровнем моря

// управление яркостью
byte LED_BRIGHT = 10;         // при отсутствии сохранения в EEPROM: яркость светодиода СО2 (0 - 10) (коэффициент настраиваемой яркости индикатора по умолчанию, если нет сохранения и не автоматическая регулировка (с)НР)
byte LCD_BRIGHT = 10;         // при отсутствии сохранения в EEPROM: яркость экрана (0 - 10) (коэффициент настраиваемой яркости экрана по умолчанию, если нет сохранения и не автоматическая регулировка (с)НР)
byte powerStatus = 0;         // индикатор вида питания: 255 - не показывать, остальное автоматически (0 - от сети, 1-5 уровень заряда батареи) (с)НР
#define BRIGHT_THRESHOLD 350  // величина сигнала, ниже которой яркость переключится на минимум (0-1023)
#define LED_BRIGHT_MAX 255    // макс яркость светодиода СО2 (0 - 255)
#define LED_BRIGHT_MIN 10     // мин яркость светодиода СО2 (0 - 255)
#define LCD_BRIGHT_MAX 255    // макс яркость подсветки дисплея (0 - 255)
#define LCD_BRIGHT_MIN 10     // мин яркость подсветки дисплея (0 - 255)

#define DISP_MODE 1        // в правом верхнем углу отображать: 0 - год, 1 - день недели и секунды
#define WEEK_LANG 0        // язык дня недели: 0 - английский, 1 - русский
#define DEBUG 0            // вывод на дисплей лог инициализации датчиков при запуске. Для дисплея 1602 не работает! Но дублируется через порт!
#define PRESSURE 0         // 0 - график давления, 1 - график прогноза дождя (вместо давления). Не забудь поправить пределы графика
#define CO2_SENSOR 1       // включить или выключить поддержку/вывод с датчика СО2 (1 вкл, 0 выкл)
#define DISPLAY_TYPE 1     // тип дисплея: 1 - 2004 (большой), 0 - 1602 (маленький)
#define DISPLAY_ADDR 0x27  // адрес платы дисплея: 0x27 или 0x3f. Если дисплей не работает - смени адрес! На самом дисплее адрес не указан

byte settingsTimeManual = 0;     // 0 - установка времени в автоматическом режиме, 1 - установка времени в ручной режиме
byte schedulePowerStatus = 1;    // 0/1 отключить или нет дисплей ночью
#define SCHEDULE_POWER_START 23  // начало отключения дисплея в часах
#define SCHEDULE_POWER_END 7     // конец отключения дисплея в часах

// пределы для индикатора (с)НР
#define normCO2 800       // и выше - желтый
#define maxCO2 1200       // и выше - красный
#define blinkLEDCO2 1500  // Значение СО2, при превышении которого будет мигать индикатор

#define minTemp 21       // и ниже для синего индикатора
#define normTemp 26      // и выше - желтый
#define maxTemp 28       // и выше - красный
#define blinkLEDTemp 35  // Значение температуры, при превышении которой будет мигать индикатор

#define maxHum 90       // и выше - синий
#define normHum 30      // и ниже - желтый
#define minHum 20       // и ниже - красный
#define blinkLEDHum 15  // Значение влажности, при показаниях ниже порога которого будет мигать индикатор

#define normPress 733  // и ниже - желтый
#define minPress 728   // и ниже - красный   // может, переделать на синий?

#define minRain -50   // и ниже - красный
#define normRain -20  // и ниже - желтый
#define maxRain 50    // и выше - синий

byte LEDType = 0;  //  при отсутствии сохранения в EEPROM: привязка индикатора к датчикам: 0 - СО2, 1 - Влажность, 2 - Температура, 3 - Осадки

#include <EEPROM.h>

int MAX_ONDATA = 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048;  // при отсутствии сохранения в EEPROM: максимальные показания графиков исходя из накопленных фактических (но в пределах лимитов) данных вместо указанных пределов, 0 - использовать фиксированные пределы (с)НР
int VIS_ONDATA = 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024 + 2048;  // при отсутствии сохранения в EEPROM: отображение показания графиков, 0 - Не отображать (с)НР

/* 1 - для графика СО2 часового, 2 - для графика СО2 суточного (с)НР
   4 - для графика влажности часовой, 8 - для графика влажности суточной (с)НР
   16 - для графика температуры часовой, 32 - для графика температуры суточной (с)НР
   64 - для прогноза дождя часового, 128 - для прогноза дождя суточного (с)НР
   256 - для графика давления часового, 512 - для графика давления суточного (с)НР
   1024 - для графика высоты часового, 2048 - для графика высоты суточного (с)НР
   для выборочных графиков значения нужно сложить (с)НР
   например: для изменения пределов у графиков суточной температуры и суточного СО2 складываем 2 + 32 и устанавливаем значение 34 (можно ставить сумму) (с)НР
*/

// пределы отображения для графиков
#define TEMP_MIN 15
#define TEMP_MAX 35
#define HUM_MIN 0
#define HUM_MAX 100
#define PRESS_MIN 720
#define PRESS_MAX 760
#define CO2_MIN 400
#define CO2_MAX 2000
#define ALT_MIN 0
#define ALT_MAX 1000

// адрес BME280 жёстко задан в файле библиотеки Adafruit_BME280.h
// стоковый адрес был 0x77, у китайского модуля адрес 0x76.
// Так что если юзаете не библиотеку из архива - не забудьте поменять

// если дисплей не заводится - поменяйте адрес (строка 54)

// пины
#define BACKLIGHT 10
#define PHOTO A3

#define MHZ_RX 2
#define MHZ_TX 3

#define LED_COM 7
#define LED_R 9
#define LED_G 6
#define LED_B 5
#define BTN_PIN 4

// библиотеки
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if (DISPLAY_TYPE == 1)
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
#else
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2);
#endif

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

#if (CO2_SENSOR == 1)
#include <MHZ19_uart.h>
MHZ19_uart mhz19;
#endif

#if (DISPLAY_TYPE == 1)                                     // для дисплея 2004 график "длиннее", поэтому интервалы времени на сегмент короче (с)НР
unsigned long hourPlotTimer = ((long)4 * 60 * 1000);        // 4 минуты
unsigned long dayPlotTimer = ((long)1.6 * 60 * 60 * 1000);  // 1.6 часа
#else
unsigned long hourPlotTimer = ((long)5 * 60 * 1000);      // 5 минуты
unsigned long dayPlotTimer = ((long)2 * 60 * 60 * 1000);  // 2 часа
#endif

unsigned long predictTimer = ((long)10 * 60 * 1000);  // 10 минут
unsigned long sensorsTimerD = 0;
unsigned long drawSensorsTimerD = 0;
unsigned long clockTimerD = 0;
unsigned long hourPlotTimerD = 0;
unsigned long dayPlotTimerD = 0;
unsigned long plotTimerD = 0;
unsigned long predictTimerD = 0;
unsigned long brightTimerD = 0;

#include "GyverButton.h"
GButton button(BTN_PIN, LOW_PULL, NORM_OPEN);

int8_t hrs, mins, secs;
byte mode = 0;
/*
  0 часы и данные
  1 график углекислого за час
  2 график углекислого за сутки
  3 график влажности за час
  4 график влажности за сутки
  5 график температуры за час
  6 график температуры за сутки
  7 график дождя/давления за час
  8 график дождя/давления за сутки
  9 график высоты за час
  10 график высоты за сутки
*/

byte podMode = 1;  // подрежим меню(с)НР
byte mode0scr = 0;
/* (с)НР
  0 - Крупно время
  1 - Крупно содержание СО2
  2 - Крупно температура
  3 - Крупно давление
  4 - Крупно влажность
  5 - Крупно высота
*/
boolean bigDig = false;  // true - цифры на главном экране на все 4 строки (для LCD 2004) (с)НР

// переменные для вывода
float dispTemp;
byte dispHum;
int dispPres;
int dispCO2 = -1;
int dispRain;

// массивы графиков
int tempHour[15], tempDay[15];
int humHour[15], humDay[15];
int pressHour[15], pressDay[15];
int co2Hour[15], co2Day[15];
int delta;
uint32_t pressure_array[6];
uint32_t sumX, sumY, sumX2, sumXY;
float a;

/*
  Характеристики датчика BME:
  Температура: от-40 до + 85 °C
  Влажность: 0-100%
  Давление: 300-1100 hPa (225-825 ммРтСт)
  Разрешение:
  Температура: 0,01 °C
  Влажность: 0.008%
  Давление: 0,18 Pa
  Точность:
  Температура: +-1 °C
  Влажность: +-3%
  Давление: +-1 Па
*/

// символы
// график
byte rowS[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b10001, 0b01010, 0b00100, 0b00000 };  // стрелка вниз (с)НР
byte row7[8] = { 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row6[8] = { 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row5[8] = { 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };  // в т.ч. для четырехстрочных цифр 2, 3, 4, 5, 6, 8, 9, 0 (с)НР
byte row4[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111 };
byte row3[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };  // в т.ч. для двустрочной цифры 0, для четырехстрочных цифр 2, 3, 4, 5, 6, 8, 9 (с)НР
byte row2[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };  // в т.ч. для двустрочной цифры 4 (с)НР
byte row1[8] = { 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111 };

// цифры //  (с)НР
uint8_t UB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 };   // для двустрочных 7, 0   // для четырехстрочных 2, 3, 4, 5, 6, 8, 9
uint8_t UMB[8] = { 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 };  // для двустрочных 2, 3, 5, 6, 8, 9
uint8_t LMB[8] = { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 };  // для двустрочных 2, 3, 5, 6, 8, 9
uint8_t LM2[8] = { 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000 };  // для двустрочной 4
uint8_t UT[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000 };   // для четырехстрочных 2, 3, 4, 5, 6, 7, 8, 9, 0

uint8_t KU[8] = { 0b00000, 0b00000, 0b00000, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000 };  // для верхней части %
uint8_t KD[8] = { 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b00000, 0b00000, 0b00000 };  // для нижней части %

// русские буквы (с)НР
uint8_t PP[8] = { 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b00000 };  // П
uint8_t BB[8] = { 0b11111, 0b10000, 0b10000, 0b11111, 0b10001, 0b10001, 0b11111, 0b00000 };  // Б
uint8_t CH[8] = { 0b10001, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b00001, 0b00000 };  // Ч
uint8_t II[8] = { 0b10001, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b10001, 0b00000 };  // И
uint8_t BM[8] = { 0b10000, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b11110, 0b00000 };  // Ь
uint8_t IY[8] = { 0b01100, 0b00001, 0b10011, 0b10101, 0b11001, 0b10001, 0b10001, 0b00000 };  // Й
uint8_t DD[8] = { 0b01110, 0b01010, 0b01010, 0b01010, 0b01010, 0b01010, 0b11111, 0b10001 };  // Д
uint8_t AA[8] = { 0b11100, 0b00010, 0b00001, 0b00111, 0b00001, 0b00010, 0b11100, 0b00000 };  // Э
uint8_t IA[8] = { 0b01111, 0b10001, 0b10001, 0b01111, 0b00101, 0b01001, 0b10001, 0b00000 };  // Я
uint8_t YY[8] = { 0b10001, 0b10001, 0b10001, 0b11101, 0b10011, 0b10011, 0b11101, 0b00000 };  // Ы
uint8_t GG[8] = { 0b11110, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b00000 };  // Г
uint8_t FF[8] = { 0b00100, 0b01110, 0b10101, 0b10101, 0b10101, 0b01110, 0b00100, 0b00000 };  // Ф
uint8_t LL[8] = { 0b01111, 0b01001, 0b01001, 0b01001, 0b01001, 0b01001, 0b10001, 0b00000 };  // Л
uint8_t ZZ[8] = { 0b10101, 0b10101, 0b10101, 0b01110, 0b10101, 0b10101, 0b10101, 0b00000 };  // Ж

// индикатор питания (с)НР
// сеть
uint8_t AC[8] = { 0b01010, 0b01010, 0b11111, 0b11111, 0b01110, 0b00100, 0b00100, 0b00011 };
// батарея
uint8_t DC[8] = { 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };  // уровень батареи - изменяется в коде скетча (с)НР

void digSeg(byte x, byte y, byte z1, byte z2, byte z3, byte z4, byte z5, byte z6) {  // отображение двух строк по три символа с указанием кодов символов (с)НР
  lcd.setCursor(x, y);
  lcd.write(z1);
  lcd.write(z2);
  lcd.write(z3);
  if (x <= 11) {
    lcd.print(F(" "));
  }

  lcd.setCursor(x, y + 1);
  lcd.write(z4);
  lcd.write(z5);
  lcd.write(z6);
  if (x <= 11) {
    lcd.print(F(" "));
  }
}

void loadClock() {
  if (bigDig && (DISPLAY_TYPE == 1)) {  // для четырехстрочных цифр (с)НР
    lcd.createChar(0, UT);
    lcd.createChar(1, row3);
    lcd.createChar(2, UB);
    lcd.createChar(3, row5);
    lcd.createChar(4, KU);
    lcd.createChar(5, KD);
  } else {  // для двустрочных цифр (с)НР
    lcd.createChar(0, row2);
    lcd.createChar(1, UB);
    lcd.createChar(2, row3);
    lcd.createChar(3, UMB);
    lcd.createChar(4, LMB);
    lcd.createChar(5, LM2);
  }

  if (now.dayOfTheWeek() == 4) {  // Для четверга в ячейку запоминаем "Ч", для субботы "Б", иначе "П" (с)НР
    lcd.createChar(7, CH);        // Ч (с)НР
  } else if (now.dayOfTheWeek() == 6) {
    lcd.createChar(7, BB);  // Б (с)НР
  } else {
    lcd.createChar(7, PP);  // П (с)НР
  }
}

void loadPlot() {
  lcd.createChar(0, rowS);  // Стрелка вниз для индикатора пределов (с)НР
  lcd.createChar(1, row1);
  lcd.createChar(2, row2);
  lcd.createChar(3, row3);
  lcd.createChar(4, row4);
  lcd.createChar(5, row5);
  lcd.createChar(6, row6);
  lcd.createChar(7, row7);
}

#if (LED_MODE == 0)
byte LED_ON = (LED_BRIGHT_MAX);
byte LED_OFF = (0);
#else
byte LED_ON = (255 - LED_BRIGHT_MAX);
byte LED_OFF = (255);
#endif

void setLEDcolor(byte color) {  // цвет индикатора задается двумя битами на каждый цвет (с)НР
  analogWrite(LED_R, LED_ON + LED_ON * ((LED_MODE << 1) - 1) * (3 - (color & 3)) / 3);
  analogWrite(LED_G, LED_ON + LED_ON * ((LED_MODE << 1) - 1) * (3 - ((color & 12) >> 2)) / 3);
  analogWrite(LED_B, LED_ON + LED_ON * ((LED_MODE << 1) - 1) * (3 - ((color & 48) >> 4)) / 3);
}

void setLED() {
  if (LED_BRIGHT < 11) {  // если ручные установки яркости
    LED_ON = 255 / 100 * LED_BRIGHT * LED_BRIGHT;
  } else {
    checkBrightness();
  }

  if (LED_MODE != 0) {
    LED_ON = 255 - LED_ON;
  }

  // ниже задается цвет индикатора в зависимости от назначенного сенсора: красный, желтый, зеленый, синий (с)НР
  if ((dispCO2 >= maxCO2) && LEDType == 0 || (dispHum <= minHum) && LEDType == 1 || (dispTemp >= maxTemp) && LEDType == 2 || (dispRain <= minRain) && LEDType == 3 || (dispPres <= minPress) && LEDType == 4) {
    setLEDcolor(3);  // красный
  } else if ((dispCO2 >= normCO2) && LEDType == 0 || (dispHum <= normHum) && LEDType == 1 || (dispTemp >= normTemp) && LEDType == 2 || (dispRain <= normRain) && LEDType == 3 || (dispPres <= normPress) && LEDType == 4) {
    setLEDcolor(3 + 8);  // желтый
  } else if (LEDType == 0 || (dispHum <= maxHum) && LEDType == 1 || (dispTemp >= minTemp) && LEDType == 2 || (dispRain <= maxRain) && LEDType == 3 || LEDType == 4) {
    setLEDcolor(12);  // зеленый
  } else {
    setLEDcolor(48);  // синий (если влажность превышает заданный максимум, температура ниже минимума, вероятность осадков выше maxRain)
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(LED_COM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLEDcolor(0);

  digitalWrite(LED_COM, LED_MODE);
  analogWrite(BACKLIGHT, LCD_BRIGHT_MAX);

  if (EEPROM.read(0) == 122) {  // если было сохранение настроек, то восстанавливаем их (с)НР
    MAX_ONDATA = EEPROM.read(2);
    MAX_ONDATA += (long)(EEPROM.read(3) << 8);
    VIS_ONDATA = EEPROM.read(4);
    VIS_ONDATA += (long)(EEPROM.read(5) << 8);
    mode0scr = EEPROM.read(6);
    bigDig = EEPROM.read(7);
    LED_BRIGHT = EEPROM.read(8);
    LCD_BRIGHT = EEPROM.read(9);
    LEDType = EEPROM.read(10);
    schedulePowerStatus = EEPROM.read(11);
    settingsTimeManual = EEPROM.read(12);
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();

#if (DEBUG == 1 && DISPLAY_TYPE == 1)
  boolean status = true;
  setLEDcolor(3);

#if (CO2_SENSOR == 1)
  lcd.setCursor(0, 0);
  lcd.print(F("MHZ-19... "));
  Serial.print(F("MHZ-19... "));
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(false);
  mhz19.getStatus();  // первый запрос, в любом случае возвращает -1
  delay(500);
  if (mhz19.getStatus() == 0) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }
#endif
  setLEDcolor(3 + 12);
  lcd.setCursor(0, 1);
  lcd.print(F("RTC... "));
  Serial.print(F("RTC... "));
  delay(50);
  if (rtc.begin()) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLEDcolor(12);
  lcd.setCursor(0, 2);
  lcd.print(F("BME280... "));
  Serial.print(F("BME280... "));
  delay(50);
  if (bme.begin(BME280_ADDRESS, &Wire)) {
    lcd.print(F("OK"));
    Serial.println(F("OK"));
  } else {
    lcd.print(F("ERROR"));
    Serial.println(F("ERROR"));
    status = false;
  }

  setLEDcolor(0);
  lcd.setCursor(0, 3);
  if (status) {
    lcd.print(F("All good"));
    Serial.println(F("All good"));
  } else {
    lcd.print(F("Check wires!"));
    Serial.println(F("Check wires!"));
  }

  for (byte i = 1; i < 20; i++) {  // убрал бесконечный цикл, сделал 5-ти секундное ожидание (с)НР
    lcd.setCursor(14, 1);
    lcd.print(F("P:    "));
    lcd.setCursor(16, 1);
    lcd.print(analogRead(PHOTO), 1);
    delay(250);
  }
#else

#if (CO2_SENSOR == 1)
  mhz19.begin(MHZ_TX, MHZ_RX);
  mhz19.setAutoCalibration(false);
#endif
  rtc.begin();
  bme.begin(BME280_ADDRESS, &Wire);
#endif

  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1,  // temperature
    Adafruit_BME280::SAMPLING_X1,  // pressure
    Adafruit_BME280::SAMPLING_X1,  // humidity
    Adafruit_BME280::FILTER_OFF
  );

  if (RESET_CLOCK) { // || rtc.lostPower()
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.clear();
  now = rtc.now();
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();

  bme.takeForcedMeasurement();
  uint32_t Pressure = bme.readPressure();
  for (byte i = 0; i < 6; i++) {   // счётчик от 0 до 5
    pressure_array[i] = Pressure;  // забить весь массив текущим давлением
  }

  // заполняем графики текущим значением
  readSensors();
  for (byte i = 0; i < 15; i++) {  // счётчик от 0 до 14
    tempHour[i] = dispTemp;
    tempDay[i] = dispTemp;
    humHour[i] = dispHum;
    humDay[i] = dispHum;
    pressHour[i] = PRESSURE ? 0 : dispPres;
    pressDay[i] = PRESSURE ? 0 : dispPres;
  }

  if (DISPLAY_TYPE == 1) {
    drawData();
  }

  loadClock();
  drawSensors();
}

void loop() {
  if (testTimer(brightTimerD, BRIGHT_TIMER)) {
    checkBrightness();  // яркость
  }

  if (testTimer(sensorsTimerD, SENS_TIME)) {
    readSensors();    // читаем показания датчиков с периодом SENS_TIME
  }

  if (testTimer(clockTimerD, CLOCK_TIMER)) {
    clockTick();          // два раза в секунду пересчитываем время и мигаем точками
  }

  plotSensorsTick();                                // тут внутри несколько таймеров для пересчёта графиков (за час, за день и прогноз)
  modesTick();                                      // тут ловим нажатия на кнопку и переключаем режимы
  if (mode == 0) {                                  // в режиме "главного экрана"
    if (testTimer(drawSensorsTimerD, SENS_TIME)) {
      drawSensors();  // обновляем показания датчиков на дисплее с периодом SENS_TIME
    }
  } else {                                          // в любом из графиков
    if (testTimer(plotTimerD, hourPlotTimer)) {
      redrawPlot();  // перерисовываем график
    }
  }
}