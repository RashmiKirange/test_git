#define slot 4 //No. of slots on rotating disk
#define RPMPin1 32
#define RPMPin2 36

#include<Preferences.h>
Preferences preferences;

long RPMN1=0,RPMN2=0;
static volatile boolean flag_rpm1=0,flag_rpm2=0;
static volatile unsigned long lastDebounceTime1 = 0,currentMicros1 = 0,RPMN1period = 0,lastDebounceTime2 = 0,currentMicros2 = 0,RPMN2period = 0;
static const unsigned long debounceDelay = 10000; 
static double distance1;
//unsigned long distance2=0;
static float speed1=0.0;
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

    static double getdistance = preferences.getDouble("distance", 0.0);
    Serial.print("getdistance");
    Serial.print(getdistance);
    Serial.print("  ");
    // Update distance
    distance1 = getdistance + ((2 * 3.14 * 0.278) / 4000);
    // Store updated distance
    // if (!preferences.putDouble("distance1", distance1)) {
    // Serial.print("Error!");
    // Serial.print("  ");
    // Serial.print("Preferences space used: ");
    // Serial.print(preferences.getBytesLength("distance1"));
    // Serial.print("  ");
    //static double storeddistance = preferences.putDouble("distance1" , distance1);
    //  Serial.print("storeddistance");
    // Serial.print(storeddistance);
    // Serial.print("  ");

    static double uploadeddata = preferences.putDouble("distance" , distance1);
    Serial.print("uploadeddata: ");
    Serial.print(uploadeddata);
    }
    //Serial.print("storedDistance");
    //Serial.print(storedDistance);
    //Serial.print("  ");
    preferences.end();
//    distance1 = preferences.getDouble("distance1" , 0);
//    distance1 = distance1 + ((2*3.14*0.278)/4000);
//    preferences.putDouble("distance1" , distance1);
//    preferences.end();
}

void setup() {
  Serial.begin(9600);

  preferences.begin("my-distance" , false);

  pinMode(RPMPin1, INPUT);  //Declaring RPM pin as input 

  //pinMode(RPMPin2, INPUT);
  //attachInterrupt(digitalPinToInterrupt(35), v_isr, FALLING);

  attachInterrupt(digitalPinToInterrupt(RPMPin1), N1, RISING);
  //attachInterrupt(digitalPinToInterrupt(RPMPin2), N2, RISING);

  //Serial.println("Setup completed");
  preferences.end();


}

void loop() {
  speed1 = 0.0;
 if(flag_rpm1) {
    //RPMN1=((60*1000000)/(RPMN1period*slot));
    //RPMN1 = 2700/(3.14*RPMN1period);
    //RPMN1 = (30*3.14*1000000)/RPMN1period;
      //RPMN1 = (24.964*1000000)/(RPMN1period*6);
      //RPMN1 = ((45.43*1000000)/(RPMN1period*6));
      RPMN1 = ((88.03*1000000)/(RPMN1period*6));
      speed1 = ((2*3.14*RPMN1*0.2789*3.6)/60);
      
    //   double storedDistance = preferences.getDouble("distance1", 0);
    // if (storedDistance == 0 && preferences.getBytesLength("distance1") == 0) {
    //   Serial.println("Error retrieving distance preference!");
    // }
    // preferences.end();
    // // Update distance
    // distance1 = storedDistance + ((2 * 3.14 * 0.278) / 4000);

    // // Store updated distance
    // if (!preferences.putDouble("distance1", distance1)) {
    //   Serial.println("Error storing distance preference!");
    // }
    // preferences.end();
    flag_rpm1=0;
  }

  if((micros()-lastDebounceTime1)>1000000) RPMN1=0 , speed1=0;
  //if((micros()-lastDebounceTime2)>1000000) RPMN2=0.0 ;
  //Serial.println(RPMN1period);
   Serial.print("RPM");
   Serial.print(RPMN1);
   Serial.print("  ");
   Serial.print("speed");
   Serial.print(speed1);
   Serial.print("  ");
   delay(1000);
   Serial.print("distance");
   Serial.print(distance1);
   Serial.print("  ");
   Serial.println("  ");

  preferences.end();
}
