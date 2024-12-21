#include "Adafruit_ADS1X15.h"
#include <Wire.h>
Adafruit_ADS1115 ads;

#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#include <Wire.h>
//Using by default pins 21 & 22
#define I2C_SDA 12
#define I2C_SCL 14

TwoWire MLX_NEW = TwoWire(0);
int16_t adc0;
float Voltage = 0.0;

void setup() 
{
  Serial.begin(115200);
  ads.begin();//RotaryPot

  MLX_NEW.begin(I2C_SDA, I2C_SCL, 100000);
  bool status;
  status = mlx.begin(0x5A, &MLX_NEW);  
  if (!status) 
  {
    Serial.println("Could not find a valid MLX sensor, check wiring!");
    while (1);
  }   
}

void loop() 
{
  Serial.print("ADC: ");
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7);
  Serial.println();
  unsigned long long int pot = rotary();
  delay(100);

  int temp;
  temp = mlx.readObjectTempC();
  Serial.println("The temperature of the object in *C is :");
  Serial.println(temp);
}

unsigned long long int rotary()
{
  adc0 = ads.readADC_SingleEnded(0);
  Voltage = (adc0 * 0.1875) / 1000;
}
