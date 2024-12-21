volatile unsigned long int prim_currentMicros=0;
volatile unsigned long int prim_previousMicros=0;
volatile unsigned long int v_currentMicros=0;
volatile unsigned long int v_previousMicros=0;
volatile unsigned long int currentMicros=0;

volatile double distance=0;
volatile double prim_rpm, v_rpm;



void setup() {
  // put your setup code here, to run once:

  pinMode(12,INPUT); // digital pin attached to sensor of INDUCTIVE 1
  pinMode(32,INPUT); // digital pin attached to sensor of INDUCTIVE 2
  //attachInterrupt(digitalPinToInterrupt(2), prim_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(3), v_isr, RISING);

}

void loop() {
  //if (micros()-prim_previousMicros>=1000000 ) prim_rpm=0;
  if (micros()-v_previousMicros>=1000000 ) v_rpm=0;
  //int map_rpm;
  //Serial.println("rpm = "+ String (prim_rpm));
//  if (prim_rpm<2800)
//        {
//          map_rpm=map(prim_rpm,0,2800,0,80);
//          analogWrite(11,map_rpm);
//          //Serial.println(prim_rpm);
//         
//        }
//        else if(prim_rpm<3200)
//        {
//          map_rpm=map(prim_rpm,2800,3200,80,150);
//          analogWrite(11,map_rpm);
//          //Serial.println(prim_rpm);
//         
//        }
//        else
//        {
//          map_rpm=map(prim_rpm,3200,3720,150,200);
//          analogWrite(11,map_rpm);
//          //Serial.println(prim_rpm);
//         
//        }

    //disp_speed(v_rpm);
 

}

//void prim_isr()
//{
// 
//  prim_rpm=10000000/(micros()-prim_previousMicros);
//  prim_previousMicros= micros();
// 
//}

void v_isr()
{
 
  currentMicros= micros();
  v_rpm=10000000/(currentMicros-v_previousMicros);
  //v_rpm=264201.6/(currentMicros-v_previousMicros);
  v_previousMicros= micros();
 
  //distance+=0.0000733504;

}
