
#include "Adafruit_ADS1X15.h"
#include "HX711.h"
#include <Wire.h>
Adafruit_ADS1115 ads;

 //1-->NewLoadcell; 2-->WhiteLoadcell;
//Pins for New Loadcell 
const int LOADCELL_DOUT_PIN1 = 26;
const int LOADCELL_SCK_PIN1 = 25;

//Pins for White Loadcell
const int LOADCELL_DOUT_PIN2 = 33;
const int LOADCELL_SCK_PIN2 = 32;

HX711 scale1;
HX711 scale2;
double force1, force2;
int16_t adc0;

float Voltage = 0.0;

void setup(void)
{
  Serial.begin(115200);
  ads.begin();//RotaryPot

  ///Loadcell

  Serial.println("HX711 Demo");

  scale1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1);
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);

}

void loop()
{
  Serial.print("Loadcell 1 : ");
  Serial.print(scale1.read());
  Serial.print("\t Loadcell 2:");
  Serial.println(scale2.read());
  double cell_1 = loadcell_1();
  double cell_2 = loadcell_2();
  

  Serial.print("ADC: ");
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();
  unsigned long long int pot = rotary();
  delay(100);
  Serial.print(" force for loadcell 1:  ");
  Serial.println(force1);
  Serial.print("\tforce for loadcell 2 :  ");
  Serial.println(force2);
  /////RotaryPot
}

unsigned long long int rotary()
{
  adc0 = ads.readADC_SingleEnded(0);
  Voltage = (adc0 * 0.1875) / 1000;
}

double loadcell_1()          //new loadcell-2000
{
  force1 = 0.002294 * (scale1.read()) - 186.9;
  
  return (force1);
}

double loadcell_2()           //white loadcell-59000
{
  force2 = 0.002412 * (scale2.read()) - 263.8;

  return (force2);
 
}
