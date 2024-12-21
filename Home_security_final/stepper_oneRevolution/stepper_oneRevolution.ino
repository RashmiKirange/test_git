
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

#define SENSOR_PIN 6 // ESP32 pin GPIO18 connected to OUT pin of IR obstacle avoidance sensor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);

  // initialize the Arduino's pin as an input
  pinMode(SENSOR_PIN, INPUT);
  
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution  in one direction:

  int state = digitalRead(SENSOR_PIN);

  if (state == LOW)
  {
    Serial.println("Obstacle detected");
    myStepper.step(stepsPerRevolution);
    delay(500);
  }
  else 
  {
    Serial.println("The obstacle is NOT present");
  }
}
