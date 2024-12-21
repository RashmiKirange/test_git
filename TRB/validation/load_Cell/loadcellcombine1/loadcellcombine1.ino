#include "HX711.h"

const int LOADCELL_DOUT_PIN1 = 16;
const int LOADCELL_SCK_PIN1 = 4;

const int LOADCELL_DOUT_PIN2 = 33;
const int LOADCELL_SCK_PIN2 = 32;

HX711 scale1;
HX711 scale2;
double force1,force2;
 
void setup() 
{ 
  Serial.begin(115200);
  
  Serial.println("HX711 Demo");
  
  scale1.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1);
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);

}
void loop() 
{
  Serial.print("\t Loadcell 1 : ");
  Serial.print(scale1.read()); 
  Serial.print("\t Loadcell 2:");
  Serial.println(scale2.read());
  double cell_1= loadcell_1();
  double cell_2= loadcell_2();
  
  //delay(5000);
}
double loadcell_1()          //new loadcell-2000
{
   force1 = 0.002412*(scale1.read()) - 263.8;
  //Serial.print(" force for loadcell 1:  "); 
  return(force1);
}
double loadcell_2()           //white loadcell-59000
{
  force2 = 0.002294*(scale2.read()) - 186.9;
  //Serial.print("\tforce for loadcell 2 :  ");
  return(force2);
}
