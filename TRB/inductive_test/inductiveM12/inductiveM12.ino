volatile int rpmCount = 0; // Variable to count the RPM
unsigned int rpm = 0;     // RPM value
unsigned long previousMillis = 0;
const int sensorPin = 4; // Define the GPIO pin connected to the inductive sensor

void setup() {
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), rpmInterrupt, FALLING);
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();

  // Calculate RPM every 1000 milliseconds (1 second)
  if (currentMillis - previousMillis >= 1000) {
    detachInterrupt(digitalPinToInterrupt(sensorPin)); // Stop the interrupt
    rpm = (rpmCount / 8); // Multiply by 60 to get RPM

    Serial.print("RPM: ");
    Serial.println(rpm);

    rpmCount = 0; // Reset the count
    previousMillis = currentMillis;
    attachInterrupt(digitalPinToInterrupt(sensorPin), rpmInterrupt, FALLING); // Re-attach the interrupt
  }
}

void rpmInterrupt() {
  rpmCount++;
}
