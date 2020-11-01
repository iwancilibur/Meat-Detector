#include <DFRobot_PH.h>
#include <EEPROM.h>

#define PH_PIN A10
float voltage,phValue,temperature = 25;
DFRobot_PH ph;

void setup()
{
    Serial.begin(115200);  
    ph.begin();
}

void loop()
{
        //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
        Serial.print("pH:");
        Serial.println(phValue);  
        delay(100);// calibration process by Serail CMD
        //ph.calibration(voltage,temperature);
}

float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
}
