#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <LiquidCrystal.h>

// declare variables.
float adj = 0.610;
float senseVal = 0;
float temperature = 0;
float temp = 4.00;  // final temperature fix value(in celsius) for LM61BIZ.
const int aPin = 3;
int DS1307_ADDRESS = 0x68;
byte command;

// for DFRobot LCD Shield.
LiquidCrystal lcd (8, 9, 4, 5, 6, 7);  // (rs, enable, d4, d5, d6, d7), rw pin -> Ground.

// for SD1602HULB
//LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  // Serial.println("DS1307RTC Read Test");
  // Serial.println("-------------------");
  // for DFR only.
  lcd.init(1, 8, 255, 9, 4, 5, 6, 7, 0, 0, 0, 0);

  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  tmElements_t tm;
  lcd.setCursor(0, 0);
  // read the input on analog pin x:
  int sensorValue = analogRead(aPin);
  //  for(int i=0;i<=sensorMax;i++){
  senseVal = analogRead(aPin);
  temperature = (senseVal * (5.0 / 1023.0) - adj) * 100 - temp;
  if (RTC.read(tm)) {
    Serial.println(senseVal * (5.0 / 1023.0));
    // Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    zeroPadding(tm.Hour);
    lcd.print((tm.Hour));
    // // Serial.write(':');
    lcd.print(":");
    print2digits(tm.Minute);
    zeroPadding(tm.Minute);
    lcd.print((tm.Minute));
    // Serial.write('.');
    lcd.print(".");
    print2digits(tm.Second);
    zeroPadding(tm.Second);
    lcd.print((tm.Second));
    // print temperature on LCD.
    lcd.print("  ");
    lcd.print(temperature);
    lcd.write(0xDF);
    lcd.print(" ");
    // Serial.print(", Date (D/M/Y) = ");
    zeroPadding(tm.Day);
    if (Serial.available()) {
      if (command == 0x74) { // "t"=0x74
        setTime();
      }
      if (command == 0x64) { // "d"=0x64
        setDay();
      }    // Serial.print(tm.Day);
      // Serial.write('/');
      // Serial.print(tm.Month);
      // Serial.write('/');
      // Serial.print(tmYearToCalendar(tm.Year));
      // Serial.println();
      lcd.setCursor(0, 1);
      lcd.print(tmYearToCalendar(tm.Year));
      lcd.print("/");
      zeroPadding(tm.Month);
      lcd.print(tm.Month);
      lcd.print("/");
      zeroPadding(tm.Day);
      lcd.print(tm.Day);
      lcd.print(".");
      lcd.print(" ");
      // weekday SMTWTFS
      //         1234567
      // Serial.println();
      // Serial.println(tm.Wday);
      weekPrint(tm.Wday);
      Serial.println(tm.Wday);
    } else {
      if (RTC.chipPresent()) {
        // Serial.println("The DS1307 is stopped.  Please run the SetTime");
        // Serial.println("example to initialize the time and begin running.");
        // Serial.println();
        lcd.print("DS1307RTC STOP!");
      } else {
        // Serial.println("DS1307 read error!  Please check the circuitry.");
        // Serial.println();
        lcd.print("RTC not exist!");
      }
      delay(9000);
    }
    delay(300);
  }

  void print2digits(int number) {
    if (number >= 0 && number < 10) {
      // Serial.write('0');
    }
    // Serial.print(number);
  }
}


void zeroPadding(int num) {
  if (num >= 0 && num < 10) {
    lcd.print("0");
    // Serial.print("0");
  }
}
void weekPrint(int x) {
  switch (x) {
    case 1:
      lcd.print("Sun.");
      break;
    case 2:
      lcd.print("Mon.");
      break;
    case 3:
      lcd.print("Tue.");
      break;
    case 4:
      lcd.print("Wed.");
      break;
    case 5:
      lcd.print("Thu.");
      break;
    case 6:
      lcd.print("Fri.");
      break;
    case 7:
      lcd.print("Sat.");
      break;
    default:
      if (x == 0) {
        // cannot read an Weekday value from DS1307RTC module!
        lcd.print("Err");
        lcd.print(x);  // prints weekday value out.
      }
      break;
  }
}
void setTime() {
  byte hour = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
  byte minute = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
  byte sec = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
}
void setDay() {
  byte year = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
  byte month = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
  byte date = (Serial.read() - 0x30) * 0x10 + (Serial.read() - 0x30);
  byte day_of_week = (Serial.read() - 0x30);
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.send(0x03);
  Wire.send(day_of_week);
  Wire.send(date);
  Wire.send(month);
  Wire.send(year);
  Wire.endTransmission();
}
void printTime() {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.send(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  byte r_sec = Wire.receive();
  byte r_minute = Wire.receive();
  byte r_hour = Wire.receive();
  byte r_day_of_week = Wire.receive();
  byte r_date = Wire.receive();
  byte r_month = Wire.receive();
  byte r_year = Wire.receive();
  Serial.print(r_year, HEX);
  Serial.print("/");
  Serial.print(r_month, HEX);
  Serial.print("/");
  Serial.print(r_date, HEX);
  Serial.print(" ");
  Serial.print(r_hour, HEX);
  Serial.print(":");
  Serial.print(r_minute, HEX);
  Serial.print(":");
  Serial.print(r_sec, HEX);
  Serial.print(" smtwtfs= ");
  Serial.println(r_day_of_week, HEX);
}
char zeroPadding(int num) {
  char str[2];
  char result[2];
  if (num >= 0 && num < 10) {
    str[0] = '0';
    str[1] = (char)num;
    //    result = atoi(str);
    //result[0] = str[0];
    //result[1] = str[1];
    return str;
  }
}

