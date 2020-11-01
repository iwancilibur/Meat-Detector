int sensorGAS=A8;

void setup() {
pinMode(sensorGAS,INPUT);
Serial.begin(115200);
}

void loop() {
int AmbilSensor=analogRead(sensorGAS);
    AmbilSensor=map(AmbilSensor,0,1024,0,100);
Serial.print("Data Sensor M :");
Serial.println(AmbilSensor);
delay(100);
}
