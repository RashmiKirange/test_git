//#include <ADS1115-Driver.h>
//#include <Adafruit_ADS1115.h>

#include "Adafruit_ADS1X15.h"
#include <Wire.h>
#include "ADS1X15.h"

ADS1115 ads(0x48);
float Voltage = 0.0;

void setup(void) 
{
  Serial.begin(115200); 
  ads.begin();
}

void loop(void) 
{
  int16_t adc0;

  adc0 = ads.readADC_SingleEnded(0);
  Voltage = (adc0 * 0.1875)/1000;

  Serial.print("ADC: "); 
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7); 
  Serial.println();
}
