#define slot 4 //No. of slots on rotating disk

#include <EEPROM.h>

#define RPMPin1 32
//#define RPMPin2 36

#define EEPROM_SIZE 4096 // Size of EEPROM in bytes (4 KB)

long RPMN1=0,RPMN2=0;
static volatile boolean flag_rpm1=0,flag_rpm2=0;
static volatile unsigned long lastDebounceTime1 = 0,currentMicros1 = 0,RPMN1period = 0,lastDebounceTime2 = 0,currentMicros2 = 0,RPMN2period = 0;
static const unsigned long debounceDelay = 10000; 
double distance1=0.0;
//unsigned long distance2=0;
float speed1=0.0;
//unsigned long speed2=0;

static void IRAM_ATTR N1()
{
  currentMicros1 = micros();
  if (currentMicros1 - lastDebounceTime1 > debounceDelay)
  {
    // RPMN1count++;
    RPMN1period = currentMicros1 - lastDebounceTime1;
    flag_rpm1 = 1;
    lastDebounceTime1 = currentMicros1;
    
    distance1 = distance1 + ((2*3.14*0.278)/4000);
     // Check if the vehicle is stationary for a certain period of time
    if (millis() - lastDebounceTime1 > 60000)
    {// If the vehicle is stationary for 1 minute
    // Write distance1 to EEPROM
    EEPROM.put(0, distance1);
    EEPROM.commit(); // Save to EEPROM
    lastDebounceTime1 = millis(); // Reset the timer
    }
}
}




//static void IRAM_ATTR N2()
//{
//  currentMicros2 = micros();
//  if (currentMicros2 - lastDebounceTime2 > debounceDelay)
//  {
//    // RPMN1count++;
//    RPMN2period = currentMicros2 - lastDebounceTime2;
//    flag_rpm2 = 1;
//    lastDebounceTime2 = currentMicros2;
//    distance2 = distance2 + ((2*3.14*0.278)/4000);
//  }
//}


void setup() {
  Serial.begin(9600);
  EEPROM.get(0, distance1);

  pinMode(RPMPin1, INPUT);  //Declaring RPM pin as input 

  //pinMode(RPMPin2, INPUT);
  //attachInterrupt(digitalPinToInterrupt(35), v_isr, FALLING);

  attachInterrupt(digitalPinToInterrupt(RPMPin1), N1, RISING);
  //attachInterrupt(digitalPinToInterrupt(RPMPin2), N2, RISING);

  Serial.println("Setup completed");

}

void loop() {
 //speed1 = 0.0;
 if(flag_rpm1) {
    //RPMN1=((60*1000000)/(RPMN1period*slot));
    //RPMN1 = 2700/(3.14*RPMN1period);
    //RPMN1 = (30*3.14*1000000)/RPMN1period;
      //RPMN1 = (24.964*1000000)/(RPMN1period*6);
      //RPMN1 = ((45.43*1000000)/(RPMN1period*6));
      RPMN1 = ((88.03*1000000)/(RPMN1period*6));
      speed1 = ((2*3.14*RPMN1*0.2789*3.6)/60);
    flag_rpm1=0;
  }
//  if(flag_rpm2) {
////    //RPMN2=((60*1000000)/(RPMN2period*slot));
//      //RPMN2 = ((88.03*1000000)/(RPMN2period*6));
//      //RPMN2 = ((45.43*1000000)/(RPMN2period*6))/8;
//      flag_rpm2=0;
//}
  if((micros()-lastDebounceTime1)>1000000) RPMN1=0 , speed1=0;
  //if((micros()-lastDebounceTime2)>1000000) RPMN2=0.0 ;
  //Serial.println(RPMN1period);
   Serial.print(RPMN1);
   Serial.print("speed");
   Serial.print(speed1);
   
   EEPROM.get(0, distance1);
   Serial.print("Stored EEPROM distance: ");
   Serial.println(distance1);
  
   //delay(500);
   //Serial.println("distance");
   //Serial.println(distance1);
   //Serial.println(RPMN2);
}
