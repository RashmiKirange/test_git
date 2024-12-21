
#define BLYNK_TEMPLATE_ID "TMPL3fI7Jcfxy"
#define BLYNK_TEMPLATE_NAME "Home auto"
#define BLYNK_AUTH_TOKEN "cPPtgpjGoHvzgEtRsZ2jf3oGN_wBaPwT"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

const int LDR_SENSOR = A0;
const int GARDEN_LIGHT = 3;
//#define LED 3 // Define the LED pin

void init_ldr(void)
{
  pinMode(LDR_SENSOR, INPUT);
  pinMode(GARDEN_LIGHT, OUTPUT);
}

void setup() {
    pinMode(GARDEN_LIGHT, OUTPUT);
    Blynk.begin(BLYNK_AUTH_TOKEN); // Connect to Blynk Cloud

    init_ldr();
}

void brightness_control()
{
  unsigned int ADC_VAL = 0;
  // Read the values from LDR sensor
  ADC_VAL = analogRead(LDR_SENSOR);

  // Scale it down from (0 to 1023) to (255 to 0)
  ADC_VAL = map(ADC_VAL, 0, 1023, 255, 0);

  analogWrite(GARDEN_LIGHT, ADC_VAL);
  delay(100);
}

BLYNK_WRITE(V0) {
    int value = param.asInt();
    if (value) 
    {
        digitalWrite(GARDEN_LIGHT, HIGH); // Turn on LED
    }
    else 
    {
        digitalWrite(GARDEN_LIGHT, LOW); // Turn off LED
    }
}

void loop() {
    Blynk.run(); // Maintain connection with Blynk Cloud
    brightness_control();
}
