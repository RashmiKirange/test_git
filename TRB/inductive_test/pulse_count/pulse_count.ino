
const int sensorPin = 34;

unsigned long previousMicros = 0;
unsigned long currentMicros = 0;
unsigned long pulsePeriod = 0;
unsigned long pulseInterval = 10000; // Set the pulse interval in milliseconds
unsigned long pulseCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, RISING);
}

void loop() 
{
//    // Check for RPM calculation every minute
//    if (currentMillis - previousMillis >= 60000) 
//    {
//      
//    float rpm = (float)pulseCount / (float)(currentMillis - previousMillis) * 60000.0;
//
//    Serial.println("RPM: " + String(rpm));
//
//    // Reset pulse count and update previousMillis for the next interval
//    pulseCount = 0;
//    previousMillis = currentMillis;
  }

void countPulse()
{
  unsigned long currentMicros = micros();
  
  // Calculate the period between pulses
  pulsePeriod = currentMicros - previousMicros;
  previousMicros = currentMicros;

  Serial.println(" PulsePeriod: " + String(pulsePeriod));
  
  int sensorState = digitalRead(sensorPin);

  if (sensorState == HIGH )
  //if (sensorState == HIGH && currentMicros - previousMicros >= pulseInterval)
  {
    
    pulseCount++;

    Serial.println("Pulse Count: " + String(pulseCount));
  }
}
