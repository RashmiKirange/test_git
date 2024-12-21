#include <EEPROM.h>

const byte PulsesPerRevolution = 8;
const unsigned long ZeroTimeout = 100000;
const byte numReadings = 2;

volatile unsigned long LastTimeWeMeasured = 0;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long RPM;
unsigned int PulseCounter = 1;

unsigned long PeriodSum = 0;
unsigned long LastTimeCycleMeasure = 0;
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra = 0;

unsigned long readings[numReadings];
unsigned long readIndex;
unsigned long total;
unsigned long average;

float Circumference = 1.7584; // Replace this with the actual circumference of the wheel
float DistancePerPulse = Circumference / PulsesPerRevolution; // Distance covered per pulse (in meters)

int eepromAddress = 0; // Starting EEPROM address to store the total distance

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(36), Pulse_Event, RISING);
  delay(1000);
}

void loop() {
  LastTimeCycleMeasure = LastTimeWeMeasured;
  unsigned long CurrentMicros = micros();

  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }

  FrequencyRaw = 10000000000 / PeriodAverage;

  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {
    FrequencyRaw = 0;
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;
  }

    FrequencyReal = FrequencyRaw / 10000;


  RPM = (FrequencyRaw * 60) / PulsesPerRevolution;

  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings;

  float Speed = (RPM * Circumference * 60) / 1000;  // Speed in km/h
  float Time = (float)(millis() - LastTimeCycleMeasure) / 1000;  // Convert time to seconds
  float Distance = RPM * DistancePerPulse * PulseCounter / 1000; // Distance in km

  // Save the total distance to EEPROM
  float totalDistance;
  EEPROM.get(eepromAddress, totalDistance);
  totalDistance += Distance;
  EEPROM.put(eepromAddress, totalDistance);
  EEPROM.commit();

  Serial.print("\tRPM: ");
  Serial.println(RPM);

  Serial.print("\tSpeed: ");
  Serial.println(Speed);

  Serial.print("\tTotal Distance: ");
  Serial.println(totalDistance);

  delay(1000);
}

void Pulse_Event() {
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();

  if (PulseCounter >= AmountOfReadings) {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;

    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;
  } else {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
}
