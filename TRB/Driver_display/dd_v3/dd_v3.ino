#include "driver/twai.h"//include libraries and drivers

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <EEPROM.h>

#define address 0//address for EEPROM
#define EEPROM_SIZE 4//EEPROM Size


#define I2C_SDA 4//I2C Pins
#define I2C_SCL 5  
#define i2c_address 0x3c

TwoWire I2COLED = TwoWire(0);

//Select lines for multiplexer 4052
#define S1 25
#define S2 26

// Pins used to connect to CAN bus transceiver: !!!Don't Change
#define RX_PIN 22
#define TX_PIN 21
#define BGpin 32 // Bargraph Pin

// Intervall:
#define POLLING_RATE_MS 500

#define OLED_RESET -1 //OLED reset
Adafruit_SH1106G display(128, 64, &I2COLED, OLED_RESET);//OLED object creation

static bool driver_installed = false; //flag for CAn driver installation

float totalDistanceCovered=0; // total distance covered by vehicle

float voltage, packAh, packSoC, packSoH;//Variables to read data on CAN bus
uint8_t failsafe,DTC1, DTC2;//failsafe mode , Diagonostic Trouble Code status
int8_t temperatureHigh,relays;//Temperature relay state and customized flag
float DCL=0;
float PC=0;

int file_no,vali_file_no=0;

int rpm=0;
int controller_temp=0;
uint16_t motor_temp=0;
//int flag_BMS=0;

// setting PWM properties
const int freq = 300000;   // was 300000
const int ledChannel = 0;  // was 0
const int resolution = 8;  //was 8

int throttle=0;

int neutral;
//int prech;
int AIR1;
int AIR2;
int TS_COMP;
int ready;
int sdStatus;
int freq_out;
int reverse;
int accLmt;
int brake;

void OLED_init();  //initialise oleds
void oled_select(int id);   //select oled to work on it

//all the intros
void soc_intro(); //shape of cell
void line_intro(); //line in the middle

void OLED1Display(float SoC);     //on oled bottom left
void OLED2Display(float voltage, int8_t temperatureHigh); //on oled top left
void OLED3Display(float speed1, float distance1);   //on oled top right
void OLED4Display(int SoH, uint8_t failsafe, uint8_t DTC1, uint8_t DTC,int rpm, int mtemp, int mctemp);  //on oled bottom right

void OLED1Display1();     //on oled bottom left
void OLED2Display1(); //on oled top left
void OLED3Display1();   //on oled top right
void OLED4Display1();  //on oled bottom right

void setup() {
 // Serial.begin(115200);

  //attachInterrupt(digitalPinToInterrupt(RPMPin), Pulse_Event, FALLING);

  //pins for multiplexer (output)
  pinMode(S1,OUTPUT);  
  pinMode(S2,OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(BGpin, ledChannel);
 
   for(int i=0 ; i<256 ; i++)
  {
    ledcWrite(ledChannel, i);
    delay(2);
  }
  for(int i=255 ; i>=0 ; i--)
  {
    ledcWrite(ledChannel, i);
    delay(2);
  }

  OLED_init();

  //totalDistanceCovered = EEPROM.read(address); //read total distance covered from EEPROM

  //calling intro functions
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

  oled_select(1);
  soc_intro();

  display.display();   //implements the changes
  
  // Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_PIN, (gpio_num_t)RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  //Look in the api-reference for other speed sets.
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  
  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) 
  {
   // Serial.println("Driver installed");
    driver_installed = true;
  }

  else 
  {
    //Serial.println("Failed to install driver");
    driver_installed = false;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) 
  {
   // Serial.println("Driver started");
    driver_installed = true;
  } 
  else 
  {
  //  Serial.println("Failed to start driver");
    driver_installed = false;
  }

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
  
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
  {
   // Serial.println("CAN Alerts reconfigured");
  }
  else
  {
    //Serial.println("Failed to reconfigure alerts");
    driver_installed = false;
  }

 // Serial.println("Setup completed");
}

void loop() {

int toggle= digitalRead(18);
//Serial.print(toggle);
  
  uint32_t alerts_triggered;
  twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));
  twai_status_info_t twaistatus;
  twai_get_status_info(&twaistatus);
  
  // Handle alerts
  // if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
  //   Serial.println("Alert: TWAI controller has become error passive.");
  // }
  // if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
  //   Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
  //   Serial.printf("Bus error count: %d\n", twaistatus.bus_error_count);
  // }
  if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
    // Serial.println("Alert: The RX queue is full causing a received frame to be lost.");
    // Serial.printf("RX buffered: %d\t", twaistatus.msgs_to_rx);
    // Serial.printf("RX missed: %d\t", twaistatus.rx_missed_count);
    // Serial.printf("RX overrun %d\n", twaistatus.rx_overrun_count);
  }

if(toggle==1)
{
  // Check if message is received
  twai_message_t message;

  //  while(twai_receive(&message,0) != ESP_OK)
  
   
  if (alerts_triggered & TWAI_ALERT_RX_DATA) // One or more messages received, handle all.
  {

  if (twai_receive(&message, 0) == ESP_OK && !(message.rtr))
  {
  switch(message.identifier)  //Read messages based on identifier
   {
     case(0X18C):
     { 
         uint8_t a=message.data[0];
         uint8_t b=message.data[1];
         uint8_t c=message.data[2];

         rpm=(((c<<16)|(b<<8)|a)>>5) & 65535;

         if(rpm>32768){
          rpm-=65536;
         }
        //  Serial.print("RPM : ");
        //  Serial.println(rpm);

         uint8_t d=message.data[3];
         uint8_t e=message.data[4];
         
         motor_temp = ((((e<<8)|d)>>6) & 255) - 40;
        //  Serial.print("Motor_Temp : ");
        //  Serial.println(motor_temp);
         
         uint8_t f=message.data[4];
         uint8_t g=message.data[5];

         controller_temp= ((((g<<8)|f)>>6) & 255) - 40;       
        //  Serial.print("Controller_Temp : ");
        //  Serial.println(controller_temp);
         break;
     }
      case(0X38C):
     {
         uint8_t a=message.data[3];
         uint8_t b=message.data[4];
         
         throttle = (((b<<8)|a)>>2) & 255;
        //  Serial.print("Throttle % : ");
        //  Serial.println(throttle);

         break;
     }
    case(0X6B0):
    {
    voltage = (message.data[0]<<8 | message.data[1])*0.1;
    packSoC = message.data[3]/2.0;
    PC=(message.data[4]<< 8 | message.data[5])*0.1;
    break;
    }

    case(0X6B1):
    {
    failsafe = (message.data[0]<<8 | message.data[1]);
    DTC1 = (message.data[2]<<8 | message.data[3]);
    DTC2 = (message.data[4]<<8 | message.data[5]);
    break;
    }

 case(0X6B2):
    {
    DCL=(message.data[0]<<8 | message.data[1]);
    temperatureHigh = (message.data[2]<< 8 | message.data[3])/2.0;
    PC=(message.data[4]<< 8 | message.data[5])*0.1;
    break;
    }

   }
  OLED1Display(packSoC);
  OLED2Display(voltage,temperatureHigh);
  // OLED3Display(RPM,0.0);    
  OLED4Display(DCL, failsafe, DTC1, DTC2,rpm,motor_temp,controller_temp);

  }
 Bargraph(rpm);
}
else
{
   oled_select(1);
   display.clearDisplay();
   delay(1);

   oled_select(2);
   display.clearDisplay();
   display.setTextSize(3);
   display.setCursor(0,10);
   display.print("CAN!");
   display.display();
   delay(1);

  //OLED3Display(RPM,0.0);   


 }
 
}
else if(toggle == 0)
{
   twai_message_t message;
    if (twai_receive(&message, 0) == ESP_OK && !(message.rtr)) 
    {
      
      // Serial.print(message.identifier);
      switch(message.identifier)
      {
        case(0xF5):
        {
        neutral=message.data[0];
        //prech=message.data[1];
        AIR1=message.data[1];
        AIR2=message.data[2];
        TS_COMP=message.data[3];
        ready=message.data[4];
        sdStatus=message.data[5];
        freq_out=message.data[6];
        reverse=message.data[7];
        break;
        }

        case(0xF6):
        {
        accLmt=message.data[0];
        brake=message.data[1];
       // file_stat=message.data[2];
        file_no=message.data[3];
        break;
        }
        case(0xF7):
        {
       // vali_file_stat = message4.data[0];
        vali_file_no = message.data[1];
        break;
        }
      }
     OLED1Display1( brake ,neutral, reverse);
     OLED2Display1( AIR1 ,AIR2, TS_COMP);
     OLED3Display1( ready ,sdStatus, freq_out);
     OLED4Display1( accLmt ,vali_file_no, file_no);

    
    }
}
 

}

void OLED_init()
{
  I2COLED.begin(I2C_SDA, I2C_SCL,100000);
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


void soc_intro()
{
  for(int i=118;i>=4;i--)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(i,0);
    display.println(".");
    //delay(1);
    display.display();
  }
  for(int j=2;j<53;j++)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(4,j);
    display.println(".");
    //delay(1);
    display.display();
  }
  for(int i=6;i<119;i++)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(i,52);
    display.println(".");
    //delay(1);
    display.display();
  }
   for(int j=52;j>=2;j--)
  {
    display.setTextSize(1);
   
    display.setCursor(118,j);
    display.println(".");
    //delay(1);
    display.display();
  }
  for(int k=20;k<33;k++)
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(119,k);
    display.println(".");
    display.setCursor(120,k);
    display.println(".");
    display.setCursor(121,k);
    display.println(".");
    display.setCursor(122,k);
    display.println(".");
    delay(1);  
    display.display();
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

void OLED1Display(float SoC)
{
  oled_select(1);
  display.clearDisplay();
  display.drawRect(6, 5, 116, 54, WHITE);
  display.fillRect(122, 25, 6, 15, WHITE);
  if(SoC==100)
  {
    display.setTextSize(5);
    display.setTextColor(WHITE);
    display.setCursor(20,14);
    display.print(SoC,0);
    display.display();  
  }
  else if(SoC>=10 && SoC<100)
  {
   
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(18,17);
    display.print(SoC,1);
   
    display.display();
  }
  else
  {
    display.setTextSize(5);
    display.setTextColor(WHITE);
    display.setCursor(22,14);
    display.print(SoC,1);
   
    display.display();
  }
  delay(1);
  display.display();
}

void OLED2Display(float voltage, int8_t temperatureHigh)
{  
  oled_select(2);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.print(voltage);
  display.setCursor(65,0);
  display.println("V");
  //display.display();

  display.setCursor(0,25);
  display.println(PC,1);
  display.setCursor(65,25);
  display.printf("A");
  //display.display();


  display.setCursor(0,50);
  display.println(temperatureHigh);
  display.setCursor(65,50);
  display.printf("%cC",247);

  display.display();
  delay(1);
}

void OLED3Display(float speed, float distance)
{
  oled_select(3);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(3);
  display.println(speed,1);
  //display.display();


  display.setCursor(80,5);
  display.setTextSize(2);
  display.println("kmph");
  //display.display();


  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,38);
  display.printf("%.1f",(distance/1000));
  display.setCursor(80,38);
  display.setTextSize(3);
  display.println("km");
 
  display.display();
  delay(1);
}

void OLED4Display(int SoH, uint8_t failsafe, uint8_t DTC1, uint8_t DTC2,int rpm, int mtemp,int mctemp)
{
  oled_select(4);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,2);
  display.printf("DCL:");
  display.println(SoH);
  display.printf("F:%x R:%d\n",failsafe,rpm);
  display.printf("TC1:%d M:%d\n",DTC1,mtemp);
  display.printf("TC2:%d C:%d\n",DTC2,mctemp);
  // display.printf("MRPM:%d\n",rpm);
  // display.printf("M_Temp:%d\n",mtemp);
  // display.printf("MC_Temp:%d\n",mctemp);



  display.display();
  delay(1);
}

void OLED1Display1(int x , int y , int z)
{
  oled_select(1);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("Brks:");

    display.setCursor(10,27);
    display.println("Ntrl:");

    display.setCursor(10,49);
    display.println("Rvrs:");

    display.setCursor(80, 5);
    display.println(x);

    display.setCursor(80, 27);
    display.println(y);

    display.setCursor(80, 49);
    display.println(z);

    display.display();
}

void OLED2Display1(int x , int y , int z)
{  
  oled_select(2);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("AIR1:");

    display.setCursor(10,27);
    display.println("AIR2:");

    display.setCursor(10,49);
    display.println("TCMP:");

    display.setCursor(80, 5);
    display.println(x);

    display.setCursor(80, 27);
    display.println(y);

    display.setCursor(80, 49);
    display.println(z);
    
    display.display();
}

void OLED3Display1(int x , int y , int z)
{
  oled_select(3);
  display.clearDisplay();

    //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("RTD :");

    display.setCursor(10,27);
    display.println("SDST:");

    display.setCursor(10,49);
    display.println("TSAL:");

    display.setCursor(80, 5);
    display.println(x);

    display.setCursor(80, 27);
    display.println(y);

    display.setCursor(80, 49);
    display.println(z);
    
    display.display();
}

void OLED4Display1(int x , int y , int z)
{
  oled_select(4);
  display.clearDisplay();

  //Display Text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    
    display.setCursor(10,5);
    display.println("ACLT:");

    display.setCursor(10,27);
    display.println("V_FN:");

    display.setCursor(10,49);
    display.println("F_No:");

    display.setCursor(80, 5);
    display.println(x);

    display.setCursor(80, 27);
    display.println(y);

    display.setCursor(80, 49);
    display.println(z);
    
    display.display();
}


void Bargraph(uint32_t dutyCycle){
  dutyCycle=map(dutyCycle,0,8500,0,100)*2.55;
 // dutyCycle = dutyCycle/0.1222;
  ledcWrite(ledChannel, dutyCycle);
}
