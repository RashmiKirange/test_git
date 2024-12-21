#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels  

#define I2C_SDA 5//I2C Pins
#define I2C_SCL 4  
#define i2c_address 0x3c

TwoWire I2COLED = TwoWire(0);

//Select lines for multiplexer 4052
#define S1 25
#define S2 26

#define OLED_RESET -1 //OLED reset
Adafruit_SH1106G display(128, 64, &I2COLED, OLED_RESET);//OLED object creation

void OLED_init();  //initialise oleds
void oled_select(int id);   //select oled to work on it

//all the intros
void line_intro(); //line in the middle

void OLED1Display();     //on oled bottom left
void OLED2Display(); //on oled top left
void OLED3Display();   //on oled top right
void OLED4Display();  //on oled bottom right

int toggle_pin = 4;
int toggle_button = 1;
int p;
int q;
int r;

void setup() 
{
  Serial.begin(115200);

  //pins for multiplexer (output)
  pinMode(S1,OUTPUT);  
  pinMode(S2,OUTPUT);

  OLED_init();

  oled_select(1);
  line_intro();
  display.clearDisplay();
  display.display(); 
  
  oled_select(2);
  line_intro();
  display.clearDisplay();
  display.display();
   
  oled_select(3);
  line_intro();
  display.clearDisplay(); 
  display.display();

  oled_select(4);
  line_intro();
  display.clearDisplay();
  display.display();

  //oled_select(1);
  //soc_intro();

  display.display();   //implements the changes
  
}

uint32_t XYZ=100000;

void OLED_init()
{
  I2COLED.begin(I2C_SDA, I2C_SCL,XYZ);
  for(int i =1;i<5;i++)
  {
    oled_select(i);
    display.begin(i2c_address, true);
    display.clearDisplay();
    display.display();
    delay(1);
  }
}

void oled_select(int id)
{
  if (id==4)
  {
    digitalWrite(S1,1);
    digitalWrite(S2,1);
  }
  else if (id==3)
  {
    digitalWrite(S1,0);
    digitalWrite(S2,1);
  }
  else if (id==2)
  {
    digitalWrite(S1,1);
    digitalWrite(S2,0);
  }
  else if(id==1)
  {
    digitalWrite(S1,0);
    digitalWrite(S2,0);
  } 
}

void line_intro()
{
 display.clearDisplay();
 for(int i=0;i<123;i++)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(i,25);
    display.println(".");
    delay(1);
    display.display();
  }
}

void OLED1Display(int x , int y , int z)
{
  oled_select(1);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("Brakes :");

    display.setCursor(10,27);
    display.println("Neutral:");

    display.setCursor(10,49);
    display.println("Reverse:");

    display.setCursor(106, 5);
    display.println(x);

    display.setCursor(106, 27);
    display.println(y);

    display.setCursor(106, 49);
    display.println(z);

    display.display();
    delay(2000);
    display.clearDisplay();
}

void OLED2Display(int x , int y , int z)
{  
  oled_select(2);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("AIR1   :");

    display.setCursor(10,27);
    display.println("AIR2   :");

    display.setCursor(10,49);
    display.println("TS_COMP:");

    display.setCursor(110, 5);
    display.println(x);

    display.setCursor(110, 27);
    display.println(y);

    display.setCursor(110, 49);
    display.println(z);
    
    display.display();
    delay(2000);
    display.clearDisplay();
}

void OLED3Display(int x , int y , int z)
{
  oled_select(3);
  display.clearDisplay();

    //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("RTD    :");

    display.setCursor(10,27);
    display.println("SD_STAT:");

    display.setCursor(10,49);
    display.println("F_OUT  :");

    display.setCursor(110, 5);
    display.println(x);

    display.setCursor(110, 27);
    display.println(y);

    display.setCursor(110, 49);
    display.println(z);
    
    display.display();
    delay(2000);
    display.clearDisplay();
}

void OLED4Display(int x , int y , int z)
{
  oled_select(4);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("ACC_LIM  :");

    display.setCursor(10,27);
    display.println(" ");

    display.setCursor(10,49);
    display.println(" ");

    display.setCursor(110, 5);
    display.println(x);

    display.setCursor(110, 27);
    display.println(y);

    display.setCursor(110, 49);
    display.println(z);
    
    display.display();
    delay(2000);
    display.clearDisplay();
}

void loop() 
{
  toggle_button = digitalRead(toggle_pin);
  if(toggle_button == 1)
  {
    OLED1Display(p , q , r);
    OLED2Display(p , q , r);
    OLED3Display(p , q , r);
    OLED4Display(p , q , r);
  }
}
