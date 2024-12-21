#include <TinyGPS++.h>
#include <HardwareSerial.h>


#include "SD.h"
#include "FS.h"
#include "driver/twai.h"
#include "SPI.h"


// Pins used to connect to CAN bus transceiver:
#define RX_PIN 21
#define TX_PIN 22

#define POLLING_RATE_MS 500
// Intervall:
#define TRANSMIT_RATE_MS 500
 
static bool driver_installed = false;


float voltage, packSoC;        //Variables to read data on CAN bus
uint8_t failsafe, DTC1, DTC2;  //failsafe mode , Diagonostic Trouble Code status
int8_t temperatureHigh;        //Temperature relay state and customized flag
float DCL = 0;
float PC = 0;

uint16_t motor_temp;
uint16_t controller_temp;

int file_stat=0,file_no=0, vali_file_stat=0,vali_file_no=0;

long state=0;

unsigned long micro=0,vali_micro=0;

//#include "time.h

HardwareSerial A7670C(2); 

String GOOGLE_SCRIPT_ID = "AKfycbyz6qO2kZHeO3GQlpBa9emIbf8gzn6SI_EMvq9CIxBEbr1orDvde36mJl520OFRvchSjA";


void setup()
{
  Serial.begin(115200);
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
delay(10000);
  A7670C.begin(115200, SERIAL_8N1, 17, 16); // RX, TX pins on ESP32 (16, 17), 115200 baud
  delay(500);
  A7670C.write("AT+CCID");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CREG?");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGATT=1");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGACT=1,1");             // Send Ctrl+Z to indicate end of message
  delay(500);
  A7670C.write("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"");             // Send Ctrl+Z to indicate end of message
  delay(500);

}


void loop()
{

   // Check if alert happened
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
  twai_message_t message4;
  // Check if message is received
  if (alerts_triggered & TWAI_ALERT_RX_DATA) {
    //Serial.println("Hello");
    // One or more messages received. Handle all.

    if (twai_receive(&message4, 0) == ESP_OK) {
      // String Data;
      switch (message4.identifier)  //Read messages based on identifier
      {
        case (0X6B0):
          {
            voltage = (message4.data[0] << 8 | message4.data[1]) * 0.1;
            //temperatureHigh = message.data[2];
            packSoC = message4.data[3] / 2.0;
            // DCL=(message.data[6]<<8 | message.data[7]);
            break;
          }

        case (0X6B1):
          {
            failsafe = (message4.data[0] << 8 | message4.data[1]);
            DTC1 = (message4.data[2] << 8 | message4.data[3]);
            DTC2 = (message4.data[4] << 8 | message4.data[5]);
            break;
          }

        case (0X6B2):
          {
            DCL = (message4.data[0] << 8 | message4.data[1]);
            temperatureHigh = (message4.data[2] << 8 | message4.data[3]) / 2.0;
            break;
          }
        case (0X18C):
          {

            uint8_t d = message4.data[3];
            uint8_t e = message4.data[4];

            motor_temp = ((((e << 8) | d) >> 6) & 255) - 40;
            // Serial.print("Motor_Temp : ");
            // Serial.println(motor_temp);

            uint8_t f = message4.data[4];
            uint8_t g = message4.data[5];

            controller_temp = ((((g << 8) | f) >> 6) & 255) - 40;
            // Serial.print("Controller_Temp : ");
            // Serial.println(controller_temp);
            break;
          }

        case(0xF6):
          {
          // accLmt=message.data[0];
          // brake=message.data[1];
          file_stat=message4.data[2];
          file_no=message4.data[3];
          micro = ((message4.data[4] << 24) | (message4.data[5] << 16) | (message4.data[6]  << 8) | (message4.data[7]));
          break;
          }
        
        case(0xF7) :
        {
          vali_file_stat=message4.data[0];
          vali_file_no=message4.data[1];
          vali_micro = ((message4.data[2] << 24) | (message4.data[3] << 16) | (message4.data[4]  << 8) | (message4.data[5])); 
          break;
        }
          
   }
  }
 }

 SendHTTP(file_stat,file_no, packSoC, voltage, DCL, temperatureHigh, motor_temp, controller_temp, failsafe, DTC1, DTC2, state,vali_file_stat, vali_file_no, micro,vali_micro);
}

void SendHTTP(int file_stat, int file_no, float packSoC, float voltage, float DCL, int8_t temperatureHigh, uint16_t motor_temp, uint16_t controller_temp, uint8_t failsafe, uint8_t DTC1, uint8_t DTC2, long state,int Vali_file_stat, int Vali_File,unsigned long micro,unsigned long Vali_micro)
{
  String http_str = "AT+HTTPPARA=\"URL\",\"https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"&sd_micros="+micro+"&file_status=" + file_stat+"&file_number="+file_no+"&vali_micros="+Vali_micro+"&vali_File_stat="+Vali_file_stat+"&vali_File="+Vali_File+"&state_of_charge="+packSoC+"&pack_voltage="+voltage+"&dis_curr_limit="+DCL+"&acc_temperature="+temperatureHigh+"&m_temp="+motor_temp+"&mc_temp="+controller_temp+"&fs="+failsafe+"&dtc1="+DTC1+"&dtc2="+DTC2+"&state1="+state+"\"\r\n";
  A7670C.print("AT+HTTPINIT\r\n");                   // Set the module to SMS mode
  delay(10);
  A7670C.print(http_str);   // Recipient's phone number (include country code)
  delay(10);
  A7670C.print("AT+HTTPACTION=0\r\n");   // Text to send
  delay(10);
  A7670C.write("AT+HTTPTERM\r\n");             // Send Ctrl+Z to indicate end of message
  delay(10);
}


