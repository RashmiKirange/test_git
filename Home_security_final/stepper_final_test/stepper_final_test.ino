#include <Stepper.h>

const int motorspeed = 100;
Stepper myStepper = (2048 ,8 , 10 , 9  , 11);


void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(motorspeed);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  myStepper.step(1024);
  delay(100);

}
