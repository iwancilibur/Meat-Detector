#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
 Serial.begin(115200);
 lcd.begin();
}

void loop() {
  //TAMPIL DATA LCD
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Baris ke 1");
  lcd.setCursor(11,0); lcd.print("| Kolm 11");
  lcd.setCursor(0,1); lcd.print("Baris ke 2");
  lcd.setCursor(11,1); lcd.print("| Kolm 11");
  lcd.setCursor(0,2); lcd.print("Baris ke 3");
  lcd.setCursor(11,2); lcd.print("| Kolm 11");
  lcd.setCursor(0,3); lcd.print("Baris ke 4");
  lcd.setCursor(11,3); lcd.print("| Kolm 11");  
  delay(500);
  
}
