#include <AccelStepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
int IR;
int IR_Sensor = 5;   

// ULN2003 Motor Driver Pins
#define IN1 4
#define IN2 3
#define IN3 1
#define IN4 2

// initialize the stepper library
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() 
{
  // initialize the serial port
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);   /*IR Pin D14 defined as Input*/
  pinMode(IN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  
  // set the speed and acceleration
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(100);
  // set target position
  stepper.moveTo(stepsPerRevolution);

}

void loop(){
  IR=digitalRead(IR_Sensor);  /*digital read function to check IR pin status*/
  if(IR==LOW){   
    stepper.run();
}
else {
  Serial.print("No person detected");
}
}
