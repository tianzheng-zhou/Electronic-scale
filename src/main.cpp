#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DT 3   //DT用来接受数据（data）
#define SCK 2  //sck为输出的时钟信号
#define WEIGHT 6.61  //用于校准的标准质量
#define READING_NUM 8  //读取次数

long init_Val;
float scale_factor;

long getData(void);
long dataOut(void);

long getData(void) {
  //仅读取传感器数据
  long sum = 0;
  long maxmum = 0;
  long minmum = 0;
  long dat = 0;
  for (unsigned char i = 0; i < READING_NUM; i++) {
    dat = dataOut();
    maxmum = dat;
    minmum = dat;
    if (dat > maxmum) {
      maxmum = dat;
    }
    if (dat < minmum) {
      minmum = dat;
    }
    sum = sum + dat;
  }
  sum -= maxmum;
  sum -= minmum;

  return sum / (READING_NUM - 2);
}


long dataOut(void) {
  long val = 0;
  pinMode(DT, OUTPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(DT, HIGH);
  digitalWrite(SCK, LOW);
  pinMode(DT, INPUT);
  while (digitalRead(DT))
    ;  //到dt=0说明有数据准备传输
  for (unsigned char i = 0; i < 24; i++) {
    digitalWrite(SCK, HIGH);
    val = val << 1;
    digitalWrite(SCK, LOW);
    if (digitalRead(DT))
      val++;
  }
  digitalWrite(SCK, HIGH);
  val = val ^ 0x800000;  //输出的是二进制补码
  digitalWrite(SCK, LOW);
  return (val);
}


void setup() {


  pinMode(A0, INPUT_PULLUP);
  lcd.begin();
  Serial.begin(9600);
  lcd.clear();                //清除原来的显示内容
  lcd.setCursor(0, 0);        //设置显示的起始位置（列，行）
  lcd.print("remove_load!");  //在第一列，第一行显示的内容


  init_Val = getData();

  lcd.clear();                      //清除原来的显示内容
  lcd.setCursor(0, 0);              //设置显示的起始位置（列，行）
  lcd.print("load_then_button !");  //在第一列，第一行显示的内容
  lcd.setCursor(0, 1);
  lcd.print(WEIGHT);
  while (digitalRead(A0) == 1)
    ;

  scale_factor = WEIGHT / (getData() - init_Val);
}

void loop() {
  float reading = 0;

  reading = (getData() - init_Val) * scale_factor;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("reading:");
  lcd.setCursor(0, 1);
  lcd.print(reading);
}
