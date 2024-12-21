#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int toggle_pin = 4;
int toggle_button = 1;
int p;
int q;
int r;


void setup()
{
  Serial.begin(9600);
  //pinMode(toggle_pin , INPUT)
  
    // initialize the OLED object
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer

}
void loop() 
{
  oled_display(p , q , r);
}
void oled_display(int x , int y , int z)
{

  toggle_button = digitalRead(toggle_pin);
  if(toggle_button == 1)
  {
    //display.clearDisplay();

    //Display Text
    // display.setTextSize(2);
    // display.setTextColor(WHITE);
    // display.setCursor(10,5);
    // display.println(x);

    // display.setTextSize(2);
    // display.setTextColor(WHITE);
    // display.setCursor(10,27);
    // display.println(y);

    // display.setTextSize(2);
    // display.setTextColor(WHITE);
    // display.setCursor(10,49);
    // display.println(z);

    display.clearDisplay();
    display.drawRect(6, 5, 116, 54, WHITE);
    //display.fillRect(122, 25, 6, 15, WHITE);

    display.display();
    delay(2000);
    display.clearDisplay();
  }
  else
  {
    Serial.println("ooooooooops error!!!");
  }
}
