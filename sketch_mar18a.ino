#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Set up dispalay
Servo servoCamera;

#define POWER_MAX (100)
#define POWER_MIN (0)
#define POWER_RATIO (50)

#define LIGHT_TURN "LIGHT_TURN"
#define MOVE_LEFT "MOVE_LEFT"
#define MOVE_RIGHT "MOVE_RIGHT"
#define MOVE_CAMERA "MOVE_CAMERA"

enum Caterpillar {LEFT, RIGHT};

int enA = 9;
int in1 = 7;
int in2 = 6;

int enB = 3;
int in3 = 5;
int in4 = 4;

void initMotor()
{
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void moveCaterpillar(int caterpillar, int power) {
  if (power < POWER_MIN || power > POWER_MAX) {
    return;
  }
  int inFirst;
  int inSecond;
  int en;

  if (caterpillar == LEFT) {
    inFirst = in1;
    inSecond = in2;
    en = enA;
  } else if (caterpillar == RIGHT) {
    inFirst = in3;
    inSecond = in4;
    en = enB;
  } else {
    return;
  }

  if (power == POWER_RATIO) {
    digitalWrite(inFirst, LOW);
    digitalWrite(inSecond, LOW);
    return;
  }


  bool moveForward = (power > POWER_RATIO);
  digitalWrite(inFirst, moveForward ? HIGH : LOW);
  digitalWrite(inSecond, moveForward ? LOW : HIGH);

  if (power > POWER_RATIO) {
    power -= POWER_RATIO;
  } else {
    power = POWER_RATIO - power;
  }

  int powerConverted = (double)(power) / (double)POWER_RATIO * 255.0;

  analogWrite(en, powerConverted);
}

void moveCamera(int position) {
  int angle = 170 / 100 * position + 5;

  servoCamera.write(angle);
}

void setup()
{
  lcd.init();
  lcd.backlight();// Turn on the light of the display
  lcd.print("Display inited");
  lcd.setCursor(8, 1);
  lcd.print("LCD 1602");

  initMotor();
  moveCaterpillar(LEFT, POWER_RATIO);
  moveCaterpillar(RIGHT, POWER_RATIO);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  servoCamera.attach(10);
  servoCamera.write(90);
  Serial.begin(115200);
}

int getValue(char *str) {
  char *endOfVal = strstr(str, ";");
  if (endOfVal == NULL) {
    return -1;
  }
  //*endOfVal = '\0';
  return atoi(str);
}

void loop()
{
  delay(100);
  char bufData[512];
  char *buf = bufData;
  int num;
  if ((num = Serial.available()) > 0) {

    Serial.readBytes(buf, num);
    buf[num] = '\0';

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buf);

    if (buf[0] == '[')
      return;

    while (num > 0) {
      
      char *delimer = strstr(buf, ":");
      if (delimer == NULL) {
        delimer = strstr(buf, ";");
        if (delimer == NULL) {
          return;
        }
      }

      *delimer = '\0';
      delimer++;
      
      if (strcmp(MOVE_LEFT, buf) == 0) {
        int val = getValue(delimer);
        if (val != -1) {
          moveCaterpillar(LEFT, val);
        }
      } else if (strcmp(MOVE_RIGHT, buf) == 0) {
        int val = getValue(delimer);
        if (val != -1) {
          moveCaterpillar(RIGHT, val);
        }
      } else if (strcmp(LIGHT_TURN, buf) == 0) {
        int val = getValue(delimer);
        if (val == 0) {
          digitalWrite(13, LOW);
        } else if (val == 1) {
          digitalWrite(13, HIGH);
        }
      } else if (strcmp(MOVE_CAMERA, buf) == 0) {
        int val = getValue(delimer);
        if (val != -1) {
          moveCamera(val);
        }        
      }

      delimer = strstr(delimer, ";"); 
      if (delimer == NULL)
        break;
        
      int commandSize = delimer - buf + 1;
      buf += commandSize;
      num -= commandSize;
      char string[512];
      sprintf(string, "num = %d, commandSize = %d, delimer = %p, buf = %p, buf = %s \n", num, commandSize, delimer, buf, buf);
      Serial.write(string);
      delay(20);
    }
  }
}

