const int potPin = A1; // Potentiometer input pin
const int ledBarPin = A0; // Analog input for LM3914

void setup() {
  for (int i = 2; i <= 11; i++) {
    pinMode(i, OUTPUT); // Set the LED pins as outputs
  }
  analogWrite(ledBarPin, 255); // Set the bar graph to full scale
}

void loop() {
  int potValue = analogRead(potPin); // Read potentiometer value
  int ledLevel = map(potValue, 0, 1023, 0, 10);

  for (int i = 0; i < 10; i++) {
    digitalWrite(2 + i, i < ledLevel);
  }
}
