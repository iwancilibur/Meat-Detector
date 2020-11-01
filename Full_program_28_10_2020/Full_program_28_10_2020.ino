#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <DFRobot_PH.h>
#include <EEPROM.h>
#include <GravityTDS.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#define CS 5
File myFile;

int prosesScanning=1000;
LiquidCrystal_I2C lcd(0x27, 20, 4);

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define PH_PIN A10 //Pin untuk PH
float voltage,phValue,temperature = 25;
int tdsValue = 0;
DFRobot_PH ph;

#define TdsSensorPin A9 //Pin Untuk TDS
GravityTDS gravityTds;

int sensorGAS=A8; //Pin Untuk Sensor GAS
int pinbuttonkirim=2;  ////Pin Untuk Button
int pinbuttonscanning=3;
String postsd;
int jeda=0;
int progressBari = 17;
int i = 0;

byte progressBar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

void setup() {
  Serial.begin(115200);
  lcd.createChar(0, progressBar);
  pinMode(pinbuttonkirim,INPUT);
  pinMode(pinbuttonscanning,INPUT);
  if (!SD.begin(CS)) {
  Serial.println("initialization failed!");
  while (1);
  }
  Serial.println("initialization done.");
  ph.begin();
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  pinMode(sensorGAS,INPUT);
  lcd.begin();
  stanby();
  tampildata();
}

void loop(){
  //BUTTON
  int button1=digitalRead(pinbuttonkirim);
  int button2=digitalRead(pinbuttonscanning);
  
  if (button1==1){
  Serial.println("Removing data.txt");
  SD.remove("data.txt"); // remove is a function in SD library to delete a file
  lcd.clear();
  for (int x=0; x < 200; x++) {delay(1);
     progressBari=map(x, 0, 200, 0, 21);
     int waktu=map(progressBari,0,21,0,100);
     lcd.setCursor(0,0);lcd.print("     INFORMASI! ");
     lcd.setCursor(0,1);lcd.print(" Silahkan Menunggu!");
     lcd.setCursor(0,2);lcd.print(" Proses Reset: ");lcd.print(waktu);lcd.print("%");
     Serial.print("Proses Reseting: ");Serial.print(waktu);Serial.println("%");
     for (i=0; i<progressBari; i++){
      lcd.setCursor(i, 3);lcd.write(byte(0));
     }
     delay(10);  
     }  
  //delay(2000);
  if (SD.exists("data.txt")) { //If the file still exist display message exist
    Serial.println("Data Berhasil di Reset!");
  } else { //If the file was successfully deleted display message that the file was already deleted.
    Serial.println("Data Sudah Di Reset!");
  }
  }
  
  if (button2==1 && jeda==0){
     lcd.clear();
     for (int x=0; x < prosesScanning; x++) {delay(1);
     progressBari=map(x, 0, prosesScanning, 0, 21);
     int waktu=map(progressBari,0,21,0,100);
     lcd.setCursor(0,0);lcd.print("     INFORMASI! ");
     lcd.setCursor(0,1);lcd.print(" Silahkan Menunggu!");
     lcd.setCursor(0,2);lcd.print("  Proses Scan: ");lcd.print(waktu);lcd.print("%");
     Serial.print("Proses Scaning: ");Serial.print(waktu);Serial.println("%");
     for (i=0; i<progressBari; i++){
      lcd.setCursor(i, 3);lcd.write(byte(0));
     }
     delay(10);  
     }    
     getdata();
     delay(1000);
     jeda=1;
  }else if (button2==1 && jeda==1){
    jeda=0;
  }
 
  if (jeda==0){
    stanby();
  }
  
}

void getdata() {
  delay(1000);
  //PENGAMBILAN NILAI SENSOR
  //Sensor Color
  float red, green, blue;
  tcs.getRGB(&red, &green, &blue);  
  tcs.setInterrupt(true);  // turn off LED
  int merah=red;
  int hijau=green;
  int biru=blue;
  int colValue=merah+hijau+biru;
  //Sensor PH
  voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
  phValue = ph.readPH(voltage,temperature);

  //Sensor TDS
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value

  //Sensor Gas / MQ
  int gasValue=analogRead(sensorGAS);
      gasValue=map(gasValue,0,1024,0,100);


  //KIRIM KE SERIAL
  //Serial.print("R:"); Serial.print(merah); Serial.print(" | G:"); Serial.print(hijau); Serial.print(" | B:"); Serial.print(biru);
  //Serial.print(" | PH:"); Serial.print(phValue); Serial.print(" | TDS:"); Serial.print(tdsValue); Serial.print(" | GAS:"); Serial.print(gasValue);
  // Serial.print(" | COL:"); Serial.print(colValue);
  // Serial.print("\n");
  
  //KIRIM KE DI LCD
    lcd.clear();
    lcd.setCursor(0,0);lcd.print("R:");lcd.print(merah);lcd.print(" | G:");lcd.print(hijau);lcd.print(" | B:");lcd.print(biru);
    lcd.setCursor(0,1);lcd.print("PH :");lcd.print(phValue); lcd.setCursor(8,1);lcd.print(" TDS:");lcd.print(tdsValue);lcd.print(" ppm");
    lcd.setCursor(0,2);lcd.print("GAS:");lcd.print(gasValue); lcd.print(" %");lcd.setCursor(8,2);lcd.print(" COL:");lcd.print(colValue);lcd.print(" %");
  
  //LOGIKA
  if (phValue>=6.84 && phValue<=7.99){ //KERBAU
    lcd.setCursor(0,3);lcd.print("HASIL DETEKSI KERBAU");
    postsd="HASIL DETEKSI KERBAU";
  }else if(phValue>=8){ //SAPI
    lcd.setCursor(0,3);lcd.print(" HASIL DETEKSI SAPI");
    postsd="HASIL DETEKSI SAPI";
  }else if(phValue<=6.83){ //BABI
    lcd.setCursor(0,3);lcd.print(" HASIL DETEKSI BABI");
    postsd="HASIL DETEKSI BABI";
  }
  

  //KIRIM KE MICROSD
  myFile = SD.open("data.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Kirim Data ke SD Card");
      myFile.print("R:"); myFile.print(merah); myFile.print(" | G:"); myFile.print(hijau); myFile.print(" | B:"); myFile.print(biru);
      myFile.print(" | PH:"); myFile.print(phValue); myFile.print(" | TDS:"); myFile.print(tdsValue); myFile.print(" | GAS:"); myFile.print(gasValue);
      myFile.print(" | COL:"); myFile.println(colValue);
      myFile.println(postsd);
      myFile.println();
      myFile.close();
      Serial.println("Kirim Sukses.");
      } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
      }
      delay(2000);
      tampildata(); //Menampilkan data setiap selesai scanning
}

void tampildata(){
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("data.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void stanby(){
     //Sensor PH
     voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
     phValue = ph.readPH(voltage,temperature);
     lcd.clear();
     lcd.setCursor(0,0);lcd.print("SYSTEM MEAT DETECTOR");
     lcd.setCursor(0,1);lcd.print("    MEAT OBJECT:");
     lcd.setCursor(0,2);lcd.print("SAPI | KERBAU | BABI");
     lcd.setCursor(0,3);lcd.print(" Calibration: ");lcd.print(phValue);
     delay(1000);
}
