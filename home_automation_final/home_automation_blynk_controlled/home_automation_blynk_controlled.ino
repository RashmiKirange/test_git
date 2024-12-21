/*************************************************************
Title         :   Home automation using Blynk
Description   :   To control light's brightness, monitor temperature, monitor water level in the tank through Blynk app
Pheripherals  :   Arduino UNO, Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#define BLYNK_TEMPLATE_ID "TMPL3fI7Jcfxy"
#define BLYNK_TEMPLATE_NAME "Home auto"
#define BLYNK_AUTH_TOKEN "bGqwVTKzhoAlmzg_n3uTKJg9ooGWy-q"

#define ON    1
#define OFF   0

#define TEMPERATURE_SENSOR  A1
#define HEATER 5
#define COOLER 4

#define TEMPERATURE_GAUGE   V1
#define COOLER_V_PIN        V0
#define HEATER_V_PIN        V2
#define WATER_VOL_GAUGE     V3
#define INLET_V_PIN         V4
#define OUTLET_V_PIN        V5
#define BLYNK_TERMINAL_V_PIN V6

// Comment this out to disable prints 
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw, inlet_sw, outlet_sw, cooler_sw;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int LDR_SENSOR = A0;
const int GARDEN_LIGHT = 13;
unsigned int ADC_VAL = 0;

// This function is called every time the Virtual Pin 0 state changes
// To turn ON and OFF cooler based on virtual PIN value
BLYNK_WRITE(COOLER_V_PIN)
{
  cooler_sw = param.asInt();
  cooler_control(cooler_sw);
}

// To turn ON and OFF heater based on virtual PIN value
BLYNK_WRITE(HEATER_V_PIN)
{
  heater_sw = param.asInt();
  heater_control(heater_sw);
}

// To turn ON and OFF inlet valve based on virtual PIN value
BLYNK_WRITE(INLET_V_PIN)
{
  inlet_sw = param.asInt();
  // Implement inlet valve control logic here
}

// To turn ON and OFF outlet valve based on virtual PIN value
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  // Implement outlet valve control logic here
}

// To display temperature and water volume as gauge on the Blynk App
void update_temperature_reading()
{
  float temperature = read_temperature();
  Blynk.virtualWrite(TEMPERATURE_GAUGE, temperature);
}

// To turn off the heater if the temperature rises above 35 deg C
void handle_temp(void)
{
  if (read_temperature() > 35.0 && heater_sw)
  {
    heater_sw = 0;
    Blynk.virtualWrite(HEATER_V_PIN, 0);
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is more than 35 degree C\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the heater\n");
    lcd.setCursor(8, 0);
    lcd.print("HTR_OFF");
    digitalWrite(HEATER, LOW);
  }
}

// Implement water tank volume control logic here
void handle_tank(void)
{
  // Add your tank volume handling code here
}

void init_temp_sys(void)
{
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(TEMPERATURE_SENSOR, INPUT);
}

void init_ldr(void)
{
  pinMode(LDR_SENSOR, INPUT);
  pinMode(GARDEN_LIGHT, OUTPUT);
}

void setup(void)
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  //lcd.setCursor(0, 0);
  //lcd.print("Home automation");
  //delay(500);
  lcd.setCursor(0, 0);
  lcd.print("T =");

  init_ldr();
  init_temp_sys();
  timer.setInterval(1000, update_temperature_reading);

  Blynk.begin(auth);
}

float read_temperature(void)
{
  float temperature;
  temperature = ((analogRead(TEMPERATURE_SENSOR) * 5.0 / 1024.0) / 0.01);
  return temperature;
}

void brightness_control()
{
  // Read the values from LDR sensor
  ADC_VAL = analogRead(LDR_SENSOR);

  // Scale it down from (0 to 1023) to (255 to 0)
  ADC_VAL = map(ADC_VAL, 0, 1023, 255, 0);

  analogWrite(GARDEN_LIGHT, ADC_VAL);
  delay(100);
}

void loop(void)
{
  Blynk.run();
  timer.run();

  brightness_control();

  String temp = String(read_temperature(), 2);
  lcd.setCursor(3, 0);
  lcd.print(temp);

  handle_temp();
  handle_tank();
  delay(1000);
}

void cooler_control(bool control)
{
  if (control)
  {
    digitalWrite(COOLER, HIGH);
  }
  else
  {
    digitalWrite(HEATER, LOW);
  }
}

void heater_control(bool control)
{
  if (control)
  {
    digitalWrite(HEATER, HIGH);
  }
  else
  {
    digitalWrite(COOLER, LOW);
  }
}
