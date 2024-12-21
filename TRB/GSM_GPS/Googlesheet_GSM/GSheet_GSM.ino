#include <HardwareSerial.h>
#include "time.h"

HardwareSerial A7670C(2);   // Use the appropriate UART port on ESP32 for communication

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

//#define button1 15                      // Button pin, wired to GND when pressed
//bool button_State; // Button state

String GOOGLE_SCRIPT_ID = "AKfycbynr9to217w9ZskeWa5WByCf9ZnYCA8S7_UZwAehWoOo6jo_JoSom7mf6rrfXRnEmqm";

void setup()
{
 // pinMode(button1, INPUT_PULLUP);  // The button is normally HIGH, goes LOW when pressed
  A7670C.begin(115200, SERIAL_8N1, 16, 17); // RX, TX pins on ESP32 (16, 17), 115200 baud
  Serial.begin(115200);
  delay(1000);
  //Serial.println("Button pressed");   // Show message on the serial monitor
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
  //button_State = digitalRead(button1);   // Read button state

  //  if (Serial.available()) {  // If button is pressed
  //   if(Serial.read()=='s'){
  //   Serial.println("Button1 pressed");
  //   delay(200);
  //   SendHTTP();
  //   }
  SendHTTP();

  

  if (A7670C.available()) {   // Display communication from the module
    Serial.write(A7670C.read());
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
