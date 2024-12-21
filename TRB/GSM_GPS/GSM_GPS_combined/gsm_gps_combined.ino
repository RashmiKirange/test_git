#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "time.h"

// Create a TinyGPS++ object
TinyGPSPlus gps;
int GPSBaud = 115200;

String GOOGLE_SCRIPT_ID = "AKfycbynr9to217w9ZskeWa5WByCf9ZnYCA8S7_UZwAehWoOo6jo_JoSom7mf6rrfXRnEmqm";

// Create a hardware serial port
HardwareSerial A7670C(2);   // Use the appropriate UART port on ESP32 for communication
HardwareSerial gpsSerial(1); // Use UART1 (Serial1) on ESP32

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;


void setup()
{
 // pinMode(button1, INPUT_PULLUP);  // The button is normally HIGH, goes LOW when pressed
  A7670C.begin(115200, SERIAL_8N1, 16, 17);     // RX, TX pins on ESP32 (16, 17), 115200 baud
  Serial.begin(115200);
  delay(1000);

  gpsSerial.begin(GPSBaud, SERIAL_8N1, 1, 3);   // Start the GPS hardware serial port at the GPS's default baud
  Serial.begin(115200);    // Start the ESP32 hardware serial port at 9600 baud


  A7670C.write("AT+CCID");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CREG?");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGATT=1");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGACT=1,1");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"");             // Send Ctrl+Z to indicate end of message
  delay(8000);
}
void loop()
{
  SendHTTP();  //call gsm function 
  
  if (A7670C.available()) 
  {   // Display communication from the module
    Serial.write(A7670C.read());
  }

  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while (true);
  }
}
void SendHTTP()
{
  String http_str = "AT+HTTPPARA=\"URL\",\"https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"date=" + "0"+"\"\r\n";
  A7670C.print("AT+HTTPINIT\r\n");                   // Set the module to SMS mode
  delay(10);
  A7670C.print(http_str);   // Recipient's phone number (include country code)
  delay(10);
  A7670C.print("AT+HTTPACTION=0\r\n");   // Text to send
  delay(10);
  A7670C.write("AT+HTTPTERM\r\n");             // Send Ctrl+Z to indicate end of message
  delay(10);
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }

  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1);
}
  
