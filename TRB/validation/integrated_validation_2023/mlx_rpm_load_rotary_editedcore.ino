#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
#include <Wire.h>
//Using by default pins 21 & 22
#define I2C_SDA 2
#define I2C_SCL 4

#include "Adafruit_ADS1X15.h"
#include "HX711.h"

Adafruit_ADS1115 ads;

TwoWire MLX_NEW = TwoWire(0);
TwoWire ADS = TwoWire(0);
int hall_pin = 17;
volatile float old_time;
volatile int rpm;                                                          
const int n =4;

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

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() 
{
  
  Serial.begin(115200);
  pinMode(hall_pin, INPUT_PULLUP);
  Serial.println("Setup completed");
  attachInterrupt(digitalPinToInterrupt(17), Task1code, FALLING);
  
  MLX_NEW.begin(I2C_SDA, I2C_SCL, 100000);
  bool status;
  status = mlx.begin(0x5A, &MLX_NEW);  
  if (!status) 
  {
    Serial.println("Could not find a valid MLX sensor, check wiring!");
    while (1);
  } 
  //RotaryPot
  //ADS.begin(I2C_SDA, I2C_SCL, 100000);
  //ADS.begin(2,4,100000);
  // bool status;
  status = ads.begin(0x48, &MLX_NEW);  
  if (!status) 
  {
    Serial.println("Check wiring for ads!");
    while (1);
  } 

  //Loadcell

  Serial.println("HX711 Demo");

  scale1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1);
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);

  xTaskCreatePinnedToCore
  (
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0          /* pin task to core 0 */
  );                  
  delay(500); 
   
}

void Task1code( void * pvParameters )
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

for(;;)
{
  rpm=10000000/(micros()-old_time);
  old_time=micros();
  int flag=1;
}

if (micros()-old_time>=1000000)
  {
    rpm=0;
  }
Serial.println("The rpm of motor is :");
Serial.println(rpm);

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

void loop() 
{
int temp;
temp = mlx.readObjectTempC();
Serial.println("The temperature of the object in *C is :");
Serial.println(temp);
//delay(1000);

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
//  delay(1000);
/* Serial.print(" force for loadcell 1:  ");
  Serial.println(force1);
  Serial.print("\tforce for loadcell 2 :  ");
  Serial.println(force2);
 */
}
  
