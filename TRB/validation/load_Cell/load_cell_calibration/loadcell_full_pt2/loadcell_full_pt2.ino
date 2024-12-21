/**
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 * MIT License
 * (c) 2018 Bogdan Necula
**/
#include "HX711.h"

const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

void setup() 
{
  Serial.begin(115200);
  
  Serial.println("HX711 Demo");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}
void loop() 
{
  Serial.print("read: \t\t");
  Serial.println(scale.read()); 
  //Serial.print("read average: \t\t");
  //Serial.println(scale.read_average(10));       // print the average of 20 readings from the ADC
  //delay(10);
}
