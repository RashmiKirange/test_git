
#define THERMISTORPIN 4

const double VCC = 3.3;           
const double R2 = 47000;

double Vout;
double Rth;
double Vin;
double temperature;
const double A =95.81;
const double B =202.11;
const double C =050.74;

void setup() 
{
  Serial.begin(115200);  /* Define baud rate for serial communication */
}

void loop() 
{

  temp();

  Serial.print(Vin);
  Serial.print("Temperature = ");

  //Serial.println(A);
  Serial.print(temperature);
  Serial.println(" degree celsius");
  //delay(500);

}
void temp()
{
  
  Vin = analogRead(THERMISTORPIN);
  Vout = (Vin * VCC) / 4095;
  Rth = (VCC * R2 / Vout) - R2;

  temperature = (1 / (A*pow(10,-5) + (B*pow(10,-6) * log(Rth)) + (C*pow(10,-9) * pow((log(Rth)),3))));   // Temperature in kelvin

  temperature = temperature - 273.15;  // Temperature in degree celsius
  
}
