#include "driver/twai.h"//include libraries and drivers
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <EEPROM.h>

#define address 0//address for EEPROM
#define EEPROM_SIZE 4//EEPROM Size


#define I2C_SDA 5//I2C Pins
#define I2C_SCL 4  
#define i2c_address 0x3c

TwoWire I2COLED = TwoWire(0);

//Select lines for multiplexer 4052
#define S1 25
#define S2 26

// Pins used to connect to CAN bus transceiver: !!!Don't Change
#define RX_PIN 22
#define TX_PIN 21

// Intervall:
#define POLLING_RATE_MS 1000

#define OLED_RESET -1 //OLED reset
Adafruit_SH1106G display(128, 64, &I2COLED, OLED_RESET);//OLED object creation

static bool driver_installed = false; //flag for CAn driver installation
uint32_t CAN_ID = 0x050;//Device CAN ID

float totalDistanceCovered=0; // total distance covered by vehicle

#define slot 8 //No. of slots on rotating disk
#define RPMPin 35

float voltage, packAh, packSoC, packSoH;//Variables to read data on CAN bus
uint8_t failsafe,DTC1, DTC2;//failsafe mode , Diagonostic Trouble Code status
int8_t temperatureHigh,relays;//Temperature relay state and customized flag
float DCL=0;
float PC=0;
int flag_BMS=0;

volatile long RPMN1count=0;
double RPMN1=0;
volatile boolean flag_rpm=0;
volatile unsigned long RPMN1period;
double speed;


//int digipin=17 ;
// int analogpin=16 ;


// int readyPIN=27;
// int brakePIN=28;
// int accLmtPIN=31;
// int neutralPIN=30;

// bool brake ; 
// bool accLmt ;
// bool neutral ;
// bool ready ;


void OLED_init();  //initialise oleds
void oled_select(int id);   //select oled to work on it


//all the intros
void soc_intro(); //shape of cell
void line_intro(); //line in the middle

void OLED1Display(float SoC);     //on oled bottom left
void OLED2Display(float voltage, int8_t temperatureHigh); //on oled top left
void OLED3Display(float speed, float distance2);   //on oled top right
void OLED4Display(int SoH, uint8_t failsafe, uint8_t DTC1, uint8_t DTC);  //on oled bottom right



void setup() {
  Serial.begin(115200);

  //pinMode(digipin,INPUT);

  // pinMode(readyPIN,INPUT);
  // pinMode(brakePIN,INPUT);
  // pinMode(accLmtPIN,INPUT);
  // pinMode(neutralPIN,INPUT);
  pinMode(RPMPin, INPUT);  //Declaring RPM pin as input 
  
  //attachInterrupt(digitalPinToInterrupt(35), v_isr, FALLING);

  attachInterrupt(35, N1, FALLING);

  //pins for multiplexer (output)
  pinMode(S1,OUTPUT);  
  pinMode(S2,OUTPUT);
  
  

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
    Serial.println("Driver installed");
    driver_installed = true;
  }

  else 
  {
    Serial.println("Failed to install driver");
    driver_installed = false;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) 
  {
    Serial.println("Driver started");
    driver_installed = true;
  } 
  else 
  {
    Serial.println("Failed to start driver");
    driver_installed = false;
  }

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
  
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK)
  {
    Serial.println("CAN Alerts reconfigured");
  }
  else
  {
    Serial.println("Failed to reconfigure alerts");
    driver_installed = false;
  }

  Serial.println("Setup completed");

}

void loop() {
  //bool ready = digitalRead(prechPIN);
  // brake = digitalRead(brake);
  // accLmt = digitalRead(accLmtPIN);
  // neutral = digitalRead(neutralPIN);
  // ready = digitalRead(readyPIN);
  //if driver not installed
  // if (!driver_installed) {  
  //   //serial.printf("Driver not installed");
  // }

  
  uint32_t alerts_triggered;
  twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(POLLING_RATE_MS));
  twai_status_info_t twaistatus;
  twai_get_status_info(&twaistatus);

  
  // Handle alerts
  if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
    Serial.println("Alert: TWAI controller has become error passive.");
  }
  if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
    Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
    Serial.printf("Bus error count: %d\n", twaistatus.bus_error_count);
  }
  if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
    Serial.println("Alert: The RX queue is full causing a received frame to be lost.");
    Serial.printf("RX buffered: %d\t", twaistatus.msgs_to_rx);
    Serial.printf("RX missed: %d\t", twaistatus.rx_missed_count);
    Serial.printf("RX overrun %d\n", twaistatus.rx_overrun_count);
  }

  // Check if message is received
  twai_message_t message;

  //  while(twai_receive(&message,0) != ESP_OK)
  
   
  if (alerts_triggered & TWAI_ALERT_RX_DATA) // One or more messages received, handle all.
  {

  while (twai_receive(&message, 0) == ESP_OK)
  {

  switch(message.identifier)  //Read messages based on identifier
   {
    case(0X6B0):
    {
    voltage = (message.data[0]<<8 | message.data[1])*0.01;
    temperatureHigh = message.data[2];
    packSoC = message.data[3]/2.0;
    DCL=(message.data[6]<<8 | message.data[7]);
    PC=(message.data[4]<< 8 | message.data[5])*0.1;
    // Data= "Voltage "+String(voltage)+ " Temperature "+String(temperatureHigh)+ "SoC "+String(packSoC)+" Current "+String(PC)+" DCL "+String(DCL)+"\n";


    //packSoH = message.data[4]/2;
    /*
    Serial.print( voltage);
    Serial.print("\t");
    Serial.print( temperatureHigh);
    Serial.print("\t");
    Serial.print( packSoC);
    Serial.print("\t");
    Serial.print(packSoH);
    Serial.println("\t");
    */
    break;
    }

    case(0X6B1):
    {
    failsafe = (message.data[0]<<8 | message.data[1]);
    DTC1 = (message.data[2]<<8 | message.data[3]);
    DTC2 = (message.data[4]<<8 | message.data[5]);
    // Data= " FS "+String(failsafe)+" DTC1 "+String(DTC1)+" DTC2 "+String(DTC2)+"\n";
    /*
    Serial.printf("%b",failsafe);
    Serial.print("\t");
    Serial.printf("%b", DTC1);
    Serial.print("\t");
    Serial.printf("%b",DTC2);
    Serial.println("\t");
    */
    break;
    }

    case(0X6B2):
    {
    flag_BMS = message.data[0];
    relays = message.data[2];
    packAh = (message.data[4]<< 8 | message.data[5])*0.01;
    /*
    Serial.print(flag_BMS, BIN);
    Serial.print("\t");
    Serial.print(relays, BIN);
    Serial.print("\t");
    Serial.print(packAh);
    Serial.println("\t");
    */
   // Data = "Relay State "+String(relays)+" Pack Ah "+String(packAh);
    break;
    }
    
    case(0X6B3):
    {
      DCL=(message.data[2]<<8 | message.data[3]);
      PC=(message.data[4]<< 8 | message.data[5])*0.1;
      break;      
    }
   }
   OLED1Display(packSoC);
   OLED2Display(voltage,temperatureHigh);

 if(flag_rpm)
  {
    noInterrupts();
    RPMN1=(100000000/(RPMN1period))*7.5;   //8 signals per rotation, 1/6/8=3.5
    interrupts();
    Serial.println(RPMN1/100.0);
    
    Serial.println(RPMN1period);    
    speed=RPMN1*1.83*18.0/5.0/60.0;     
    Serial.println(speed/100.0); 
    //EEPROM.write(address, totalDistanceCovered);
    //EEPROM.commit();
    flag_rpm=0;
  }

  if((micros()-RPMN1count)>1000000) speed=0.0 ;
  OLED3Display(speed/100.0,totalDistanceCovered);   
  
  //Serial.print(ready);   
  OLED4Display(DCL, failsafe, DTC1, DTC2);
  }
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


   if(flag_rpm)
  {
    noInterrupts();
    RPMN1=(100000000/(RPMN1period))*7.5;   //8 signals per rotation, 1/6/8=3.5
    interrupts();
    Serial.println(RPMN1/100.0);
    
    Serial.println(RPMN1period);    
    speed=RPMN1*1.83*18.0/5.0/60.0;     
    Serial.println(speed/100.0); 
    //EEPROM.write(address, totalDistanceCovered);
    //EEPROM.commit();
    flag_rpm=0;
  }

  if((micros()-RPMN1count)>1000000) speed=0.0 ;
  OLED3Display(speed/100.0,totalDistanceCovered);   

  // Serial.println(ready);

  
  // if(ready==1)
  // {
  //   start_sequence_fail();
  //   Serial.print(ready);
 
  // }
 }
}

void N1()
{
  
  RPMN1period=micros()-RPMN1count;
  RPMN1count=micros();
  totalDistanceCovered+=0.228551;
  flag_rpm=1;
}



static void handle_rx_message(twai_message_t& message) {
  // Process received message
  if (!(message.rtr))
  {
    Serial.printf("CAN ID == %X\n",message.identifier);
    for (int i = 0; i < message.data_length_code; i++) {
      Serial.printf(" %d = %02x,", i, message.data[i]);
    }
    Serial.println("");
  }
//  else Serial.println ("CAN ID NOT MATCH"); 
 
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

void OLED3Display(double speed, float distance)
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

void OLED4Display(int SoH, uint8_t failsafe, uint8_t DTC1, uint8_t DTC2)
{
  oled_select(4);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,1);
  display.printf("DCL:");
  display.println(SoH);
  display.printf("FS:%x\n",failsafe);
  display.printf("DTC1:%d\n",DTC1);
  display.printf("DTC2:%d\n",DTC2);
  display.display();
  delay(1);
}

// void start_sequence_fail()
// {
//     oled_select(4);
//     display.clearDisplay();
//     display.setTextSize(2);

//     if(brake==0)
//     {     
//       display.setCursor(0,1);
//       display.printf("Brakes");
//     }
  
//     if(accLmt==1)
//     {
//       display.setCursor(0,21);
//       display.printf("Accelerator");
//     }
//     if(neutral==0)
//     {
//       display.setCursor(0,41);
//       display.printf("Neutral");
//     }

//     display.display();
//     delay(1);
// }
