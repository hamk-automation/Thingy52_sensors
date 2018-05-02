#include "Adafruit_CCS811.h"
#include <SparkFunSX1509.h>
#include <Wire.h>
#include <Sodaq_LPS22HB.h>
#include <HTS221.h>

#define COLOR_SENSOR 0x38
#define SX1509_ADDRESS  0x3E
#define VDD_CCS_ADDRESS 10
#define WAKE_ADDRESS 12
#define COLOR_GREEN 5
#define COLOR_BLUE 6
#define COLOR_RED 7


SX1509 io;
Adafruit_CCS811 ccs;
Sodaq_LPS22HB barometricSensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  smeHumidity.begin();
  
  io.begin(SX1509_ADDRESS);
  io.pinMode(VDD_CCS_ADDRESS, OUTPUT);
  io.pinMode(WAKE_ADDRESS, OUTPUT);
  
  io.pinMode(COLOR_GREEN, ANALOG_OUTPUT);
  io.pinMode(COLOR_BLUE, ANALOG_OUTPUT);
  io.pinMode(COLOR_RED, ANALOG_OUTPUT);
  
  io.digitalWrite(VDD_CCS_ADDRESS, HIGH);
  io.digitalWrite(WAKE_ADDRESS, LOW);
  delay(100);
  ccs.begin();
  color_init();
  
  if (barometricSensor.init()) {
        barometricSensor.enableSensor(Sodaq_LPS22HB::OdrOneShot);
        Serial.println("Initialized Baro sensor");
    }
  else Serial.println("Failed");
    
  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);

}

void loop() {
  static int i=0;

  ccs.readData();
  Serial.print("CO2: ");
  Serial.print(ccs.geteCO2());
  Serial.print("ppm, TVOC: ");
  Serial.println(ccs.getTVOC());
  Serial.print("Pressure: ");
  Serial.println(barometricSensor.readPressureHPA());
  Serial.print("Temperature: ");
  Serial.println(smeHumidity.readTemperature());
  Serial.print("Humidity: ");
  Serial.println(smeHumidity.readHumidity());
  color_read();
  Serial.println();
 
  delay(500); 
}

void color_init(){
  Wire.beginTransmission(COLOR_SENSOR);
  Wire.write(0x41);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(COLOR_SENSOR);
  Wire.write(0x42);
  Wire.write(0x90);
  Wire.endTransmission();
  
  Wire.beginTransmission(COLOR_SENSOR);
  Wire.write(0x44);
  Wire.write(0x02);
  Wire.endTransmission();
  delay(300);
}

void color_read(){
  unsigned int data[8];
    for(int i = 0; i < 8; i++)
    {
        Wire.beginTransmission(COLOR_SENSOR);
        Wire.write((80+i));
        Wire.endTransmission();
        
        Wire.requestFrom(COLOR_SENSOR, 1);

        if(Wire.available() == 1)
        {
            data[i] = Wire.read();
        }
    }
    int red = ((data[1] & 0xFF) * 256) + (data[0] & 0xFF);
    int green = ((data[3] & 0xFF) * 256) + (data[2] & 0xFF);
    int blue = ((data[5] & 0xFF) * 256) + (data[4] & 0xFF);
    int cData = ((data[7] & 0xFF) * 256) + (data[6] & 0xFF);
    
    Serial.print("Red Color luminance  : ");
    Serial.println(red);
    Serial.print("Green Color luminance : ");
    Serial.println(green);
    Serial.print("Blue Color luminance : ");
    Serial.println(blue);
    Serial.print("Clear Data Color luminance : ");
    Serial.println(cData);
}

