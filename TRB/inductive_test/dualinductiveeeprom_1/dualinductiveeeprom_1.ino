#define slot 4 //No. of slots on rotating disk
#define RPMPin1 32
#define RPMPin2 36

#include <EEPROM.h>

#define EEPROM_ADDR_DISTANCE_1 0
#define EEPROM_ADDR_DISTANCE_2 (EEPROM_ADDR_DISTANCE_1 + sizeof(distance1))


long RPMN1=0,RPMN2=0;
static volatile boolean flag_rpm1=0,flag_rpm2=0;
static volatile unsigned long lastDebounceTime1 = 0,currentMicros1 = 0,RPMN1period = 0,lastDebounceTime2 = 0,currentMicros2 = 0,RPMN2period = 0;
static const unsigned long debounceDelay = 10000; 
unsigned long distance1=0;
unsigned long distance2=0;

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
    EEPROM.put(EEPROM_ADDR_DISTANCE_1, distance1);
  }
}

static void IRAM_ATTR N2()
{
  currentMicros2 = micros();
  if (currentMicros2 - lastDebounceTime2 > debounceDelay)
  {
    // RPMN1count++;
    RPMN2period = currentMicros2 - lastDebounceTime2;
    flag_rpm2 = 1;
    lastDebounceTime2 = currentMicros2;
    distance2 = distance2 + ((2*3.14*0.278)/4000);
    EEPROM.put(EEPROM_ADDR_DISTANCE_2, distance2);
  }
}


void setup() {
  Serial.begin(9600);

  pinMode(RPMPin1, INPUT);  //Declaring RPM pin as input 

  pinMode(RPMPin2, INPUT);
  //attachInterrupt(digitalPinToInterrupt(35), v_isr, FALLING);

  EEPROM.begin(sizeof(distance1) + sizeof(distance2));

  attachInterrupt(digitalPinToInterrupt(RPMPin1), N1, RISING);
  attachInterrupt(digitalPinToInterrupt(RPMPin2), N2, RISING);

  Serial.println("Setup completed");
}

void loop() {
 if(flag_rpm1) {
    //RPMN1=((60*1000000)/(RPMN1period*slot));
    //RPMN1 = 2700/(3.14*RPMN1period);
    //RPMN1 = (30*3.14*1000000)/RPMN1period;
    //RPMN1 = (60.533*1000000)/(RPMN1period*6);
      RPMN1 = (24.964*1000000)/(RPMN1period*6);
    //speed = 
    flag_rpm1=0;
  }
  if(flag_rpm2) {
//    //RPMN2=((60*1000000)/(RPMN2period*slot));
      //RPMN2 = (60.533*1000000)/(RPMN2period*6);
      RPMN2 = ((24.964*1000000)/(RPMN2period*6))/8;
      flag_rpm2=0;
}
  if((micros()-lastDebounceTime1)>1000000) RPMN1=0.0;
  if((micros()-lastDebounceTime2)>1000000) RPMN2=0.0;
  //Serial.println(RPMN1period);
   Serial.println(RPMN1);
   Serial.print("\t");
   //Serial.println(RPMN2);

    EEPROM.get(EEPROM_ADDR_DISTANCE_1, distance1);
  Serial.print("Distance 1: ");
  Serial.println(distance1);
  EEPROM.get(EEPROM_ADDR_DISTANCE_2, distance2);
  Serial.print("Distance 2: ");
  Serial.println(distance2);
  
   EEPROM.commit();
}
