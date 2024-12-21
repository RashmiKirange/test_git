#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define SENSOR_1_ADDRESS 0x5A
#define SENSOR_2_ADDRESS 0x5B

Adafruit_MLX90614 sensor1;
Adafruit_MLX90614 sensor2;

void setup() {
  Serial.begin(9600);

  // Initialize I2C communication
  Wire.begin(33, 25); // Use GPIO pins 33 and 25 for I2C communication

  // Initialize MLX90614 sensors
  sensor1.begin(SENSOR_1_ADDRESS);
  sensor2.begin(SENSOR_2_ADDRESS);
}

void loop() {
  // Read temperature values from both sensors
  float temp1 = sensor1.readObjectTempC();
  float temp2 = sensor2.readObjectTempC();

  // Print the temperature values
  Serial.print("Sensor 1 temperature: ");
  Serial.print(temp1);
  Serial.println(" °C");

  Serial.print("Sensor 2 temperature: ");
  Serial.print(temp2);
  Serial.println(" °C");

  delay(1000);
}
