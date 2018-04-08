#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);  // Set up dispalay

#define POWER_MAX (100)
#define POWER_MIN (0)
#define POWER_RATIO (50)

enum Caterpillar {LEFT, RIGHT};

int enA = 10;
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
  } else if (caterpillar == RIGHT) {
    inFirst = in3;
    inSecond = in4;
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

void setup()
{
  lcd.init();                     
  lcd.backlight();// Turn on the light of the display
  lcd.print("Display inited");
  lcd.setCursor(8, 1);
  lcd.print("LCD 1602");

  initMotor();
  moveCaterpillar(LEFT, MOTOR_RATIO);
  moveCaterpillar(RIGHT, MOTOR_RATIO);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(115200);  
}


void loop()
{
  delay(200);
  char buf[256];
  int num;
  if ((num = Serial.available()) > 0) {

    Serial.readBytes(buf, num);
    buf[num] = '\0';
    char getted[250];

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(buf);
   
    snprintf(getted, sizeof(getted), "get from, num of avaliable is: %d UART %s\n", num, buf);

    Serial.write(getted);
     
     if (buf[0] == '1') {
       digitalWrite(13, HIGH);
     }
     if(buf[0] == '0') {
         digitalWrite(13, LOW);
       }
     
  }
}

