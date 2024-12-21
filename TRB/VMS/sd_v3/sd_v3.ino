#include "SD.h"
#include "FS.h"
#include "driver/twai.h"
#include "SPI.h"

// int toggle=10 ;

// Pins used to connect to CAN bus transceiver:
#define RX_PIN 21
#define TX_PIN 22

#define POLLING_RATE_MS 500
// Intervall:
#define TRANSMIT_RATE_MS 500
 
static bool driver_installed = false;


//int prechPin =36 ;
int AIR1Pin = 39;
int AIR2Pin = 34;
int TS_CompPin = 15;
int RTD = 25;
int sdPin = 27;
int freq_outPin = 14;
int neutralPin = 33;
int reversePin = 13;
int accLmtPin = 26;
int brakePin = 32;

int LV_Current = 4;
int LV_Voltage = 2;

float voltage, packSoC;        //Variables to read data on CAN bus
uint8_t failsafe, DTC1, DTC2;  //failsafe mode , Diagonostic Trouble Code status
int8_t temperatureHigh;        //Temperature relay state and customized flag
float DCL = 0;
float PC = 0;

int file_status = 0;

int64_t rpm;
uint16_t motor_temp;
uint16_t controller_temp;
int16_t torque;
int16_t throttle;
int16_t motor_current;
uint16_t motor_voltage;

//int flag_BMS=0;

//initialising file name
int tempb = 0;

const float ACS712_Sensitivity = 0.185;

String FileName = "/dump";


//function to write into file (newfile)

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    file_status = 0;
  } 
  if(!file.print(message))
  {
    file_status=1;
  }
  else {
    file_status = 2;
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void file_num_check(void) {
  /* File dataFile; int tempb = 0; //variables should declare first globally. */
  while (SD.exists("/d" + String(tempb + 1) + ".txt")) {
    tempb++;
    Serial.println(tempb);
    //   lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
    //   lcd.print(tempb);
  }
  // lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
  //  lcd.print("Filecheck done !");
  Serial.println("Filecheck done ! ");
}

 void send_message() {
  //int prech = analogRead(prechPin);
  int AIR1 = analogRead(AIR1Pin);
  int AIR2 = analogRead(AIR2Pin);
  int TS_COMP = analogRead(TS_CompPin);
  int RTD1 = analogRead(RTD);
  int sdStatus = analogRead(sdPin);
  int freq_out = analogRead(freq_outPin);
  int neutral = analogRead(neutralPin);
  int reverse = analogRead(reversePin);
  int accLmt = analogRead(accLmtPin);
  int brake = analogRead(brakePin);

  AIR1 = map(AIR1, 0, 4095, 0, 255);
  AIR2 = map(AIR2, 0, 4095, 0, 255);
  TS_COMP = map(TS_COMP, 0, 4095, 0, 255);
  RTD1 = map(RTD1, 0, 4095, 0, 255);
  sdStatus = map(sdStatus, 0, 4095, 0, 255);
  freq_out = map(freq_out, 0, 4095, 0, 255);
  neutral = map(neutral, 0, 4095, 0, 255);
  reverse = map(reverse, 0, 4095, 0, 255);
  accLmt = map(accLmt, 0, 4095, 0, 255);
  brake = map(brake, 0, 4095, 0, 255);

  // Configure message to transmit
  twai_message_t message;
  
  message.data_length_code = 8;
  message.identifier = 0xF5;
  message.extd = 0;
  message.rtr = 0;


  message.data[0] = neutral;
  message.data[1] = AIR1;
  message.data[2] = AIR2;
  message.data[3] = TS_COMP;
  message.data[4] = RTD;
  message.data[5] = sdStatus;
  message.data[6] = freq_out;
  message.data[7] = reverse;

  // Queue message for transmission
  if (twai_transmit(&message, pdMS_TO_TICKS(TRANSMIT_RATE_MS)) == ESP_OK) {
    Serial.print("Message queued for transmission\n");
  } else {
    Serial.print("Failed to queue message for transmission\n");
  }
  delay(2);

  twai_message_t message1;
unsigned long micro= micros();
  message1.identifier = 0xF6;
  message1.data_length_code = 8;

  message1.extd = 0;
  message1.rtr = 0;

  message1.data[0] = accLmt;
  message1.data[1] = brake;
  message1.data[2] = file_status;
  message1.data[3] = tempb;
  message1.data[4]=  (micro>>24) & 0xFF;
  message1.data[5] = (micro>> 16) & 0xFF;
  message1.data[6] = (micro>> 8) & 0xFF;
  message1.data[7] = (micro) & 0xFF;
 
  Serial.print(tempb);
  // Serial.print("\n");
  // Serial.print(message1.data[1]);
  // Serial.print("a\n");

  if (twai_transmit(&message1, pdMS_TO_TICKS(TRANSMIT_RATE_MS)) == ESP_OK) {
    Serial.print("Message queued for transmission\n");
  } else {
    Serial.print("Failed to queue message for transmission\n");
  }
}

void setup() {
  // Start Serial:
  Serial.begin(115200);

  pinMode(AIR1Pin, INPUT);
  pinMode(AIR2Pin, INPUT);
  pinMode(TS_CompPin, INPUT);
  pinMode(RTD, INPUT);
  pinMode(sdPin, INPUT);
  pinMode(freq_outPin, INPUT);
  pinMode(neutralPin, INPUT);
  pinMode(reversePin, INPUT);
  pinMode(accLmtPin, INPUT);
  pinMode(brakePin, INPUT);


  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    //return;
  }

  file_num_check();
  tempb++;
  FileName = "/d" + String(tempb) + ".txt";
  writeFile(SD, FileName.c_str(), "Micros,Voltage,Temperature,SOC,PC,DCL,Failsafe,DTC1,DTC2,LV_Current,State,M_RPM,M_Temp,MC_Temp,M_Torque,M_Curr,M_Vol\n");  //print this string first into the file

  // Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)TX_PIN, (gpio_num_t)RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();  //Look in the api-reference for other speed sets.
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("Driver installed");
    driver_installed = true;
  } else {
    Serial.println("Failed to install driver");
    driver_installed = false;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) {
    Serial.println("Driver started");
    driver_installed = true;
  }

  else {
    Serial.println("Failed to start driver");
    driver_installed = false;
  }

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;

  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
    Serial.println("CAN Alerts reconfigured");
  } else {
    Serial.println("Failed to reconfigure alerts");
    driver_installed = false;
  }

  // TWAI driver is now successfully installed and started
  //  driver_installed = true;
}

void loop() {
  //int toggle_check=digitalRead(toggle);

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
            PC = (message4.data[4] << 8 | message4.data[5]) * 0.1;
            // Serial.println(voltage);
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
            PC = (message4.data[4] << 8 | message4.data[5]) * 0.1;

            break;
          }
        case (0X18C):
          {
            uint8_t a = message4.data[0];
            uint8_t b = message4.data[1];
            uint8_t c = message4.data[2];

            rpm = (((c << 16) | (b << 8) | a) >> 5) & 65535;

            if (rpm > 32768) {
              rpm -= 65536;
            }
            //  Serial.print("Motor_Temp : ");
            //  Serial.println(rpm);

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
          //break;
        //}
        case (0X28C):
          {
            uint8_t a = message4.data[5];
            uint8_t b = message4.data[6];

            torque = ((a | (b << 8)) & 1023);

            if (torque > 512) {
              torque -= 1024;
            }
            torque = torque * 0.10;
            // Serial.print("Torque : ");
            // Serial.println(torque);
         
         break;
     }
      case(0X38C):
     {
         uint8_t a=message4.data[3];
         uint8_t b=message4.data[4];
         
         throttle = (((b<<8)|a)>>2) & 255;
        //  Serial.print("Throttle % : ");
        //  Serial.println(throttle);

         uint8_t c=message4.data[6];
         uint8_t d=message4.data[7];
         
         motor_current = (((d<<8)|c)>>6) & 1023;
         if(motor_current>512){
          motor_current-=1024;
         }
        //  Serial.print("Motor_Current : ");
        //  Serial.println(motor_current);

         uint8_t x=message4.data[5];
         uint8_t y=message4.data[6];

         motor_voltage=((((y<<8)|x)>>4) & 1023)*0.10;
        //  Serial.print("Motor_Voltage : ");
        //  Serial.println(motor_voltage);
  
         break;
     }

   }
  }
}  

  int AIR1 = digitalRead(AIR1Pin);
  int AIR2 = digitalRead(AIR2Pin);
  int TS_COMP = digitalRead(TS_CompPin);
  int ready = digitalRead(RTD);
  int sdStatus = digitalRead(sdPin);
  int freq_out = digitalRead(freq_outPin);
  int neutral = digitalRead(neutralPin);
  int reverse = digitalRead(reversePin);
  int accLmt = digitalRead(accLmtPin);
  int brake = digitalRead(brakePin);

  int adc = analogRead(LV_Current);
  //Serial.println(sensorValue);

  float voltage1 = ((adc*3.3)/4095.0)/**((3.3+6.8)/6.8)*/+0.22;
  //float voltage = ((adc*3.3)/4095.0)*((47+13.6)/13.6);
  float current = (voltage-2.5)/0.066;

 
   int state = AIR1 << 9 | AIR2 << 8 | TS_COMP << 7 | ready << 6 | sdStatus << 5 | freq_out << 4 | neutral << 3 | reverse << 2 | accLmt << 1 | brake;
   String Data = String(micros()) + "," + String(voltage) + "," + String(temperatureHigh) + "," + String(packSoC) + "," + String(PC) + "," + String(DCL) + "," + String(failsafe) + "," + String(DTC1) + "," + String(DTC2) + "," + String(current) + "," + String(state) + "," + String(rpm) + "," + String(motor_temp) + "," + String(controller_temp) + "," + String(torque) + "," + String(motor_current) + "," + String(motor_voltage)+"\n";
    appendFile(SD, FileName.c_str(), Data.c_str());
    Serial.println(Data);
    Serial.println("");
    Data = "";
    send_message();
    delay(1);
}