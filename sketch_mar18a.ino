#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);  // Set up dispalay

void setup()
{
  lcd.init();                     
  lcd.backlight();// Turn on the light of the display
  lcd.print("Display inited");
  lcd.setCursor(8, 1);
  lcd.print("LCD 1602");

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(115200);

//  Serial.write("hello!\n");
  
}


void loop()
{
  delay(500);
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

