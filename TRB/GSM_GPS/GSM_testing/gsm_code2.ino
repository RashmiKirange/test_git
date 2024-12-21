#include <Arduino.h>

void setup() {
  Serial.begin(115200); // Initialize ESP32 serial communication
  delay(1000); // Wait for serial port to stabilize
}

void loop() {
  // Configure SIMA7670C, replace the following commands with the correct AT commands for your module
  Serial.println("AT");
  delay(100);
  Serial.println("AT+CMGF=1"); // Set SMS mode to text mode
  delay(100);
  Serial.println("AT+CMGS=\"+8857853345\""); // Replace with destination phone number
  delay(100);
  Serial.println("Hello, this is a test SMS!"); // Replace with your desired message content
  delay(100);
  Serial.write(26); // ASCII code for Ctrl+Z (end of message)
  delay(1000); // Give some time for the SMS to be sent

  // Your main code logic here...
}
