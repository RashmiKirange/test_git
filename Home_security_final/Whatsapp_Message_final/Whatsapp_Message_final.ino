// Example Whatsapp Message
// Github :
// https://github.com/hafidhh
// https://github.com/hafidhh/Callmebot-ESP32

#include <WiFi.h>
#include <Callmebot_ESP32.h>

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

#define SENSOR_PIN 12 // ESP32 pin GPIO18 connected to OUT pin of IR obstacle avoidance sensor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 2, 14, 15, 13);


const char* ssid = "Joint family";
const char* password = "air45559";
// Note :
// phoneNumber : Indonesia +62, Example: "+62897461238"
// apiKey : Follow instruction on https://www.callmebot.com/blog/free-api-whatsapp-messages/
String phoneNumber = "+919373378043";
String apiKey = "3191160";
String messsage = "Someones inside your shop";

void setup() {
  Serial.begin(115200);

  // set the speed at 60 rpm:
  myStepper.setSpeed(60);

  // initialize the Arduino's pin as an input
  pinMode(SENSOR_PIN, INPUT);
  
  // initialize the serial port:
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Whatsapp Message
  //whatsappMessage(phoneNumber, apiKey, messsage);
  //Serial.println("Message sent");
}

void loop() 
{

  int state = digitalRead(SENSOR_PIN);

  if (state == LOW)
  {
    Serial.println("Obstacle detected");

    whatsappMessage(phoneNumber, apiKey, messsage);

    myStepper.step(stepsPerRevolution);
    delay(500);
    
  }
  else 
  {
    Serial.println("The obstacle is NOT present");
  }
}  
