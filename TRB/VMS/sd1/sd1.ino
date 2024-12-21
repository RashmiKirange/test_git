#include "FS.h"
#include "SD.h"
#include "SPI.h"

File file;
//File file1;
int i=0;
String din;
String a,b,c,d;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
if(!SD.begin()){
  Serial.println("SD failed");
  }
  else {
     while(SD.exists("D"+String(i)+".txt")){
      Serial.println(i);
      i=i+1;
  }
  
  Serial.println("no file found");
  Serial.print("SD intialised\n");
  }
  
/*if (!SD.exists("/count.txt")){
    Serial.println("count.txt doesn't exist on SD Card");
    } 
else{
    Serial.println("count.txt exists on SD Card");
    SD.remove("/count.txt");
}*/


 
} 



void loop() {
  // put your main code here, to run repeatedly:
  
 /*file=SD.open("/count.txt",FILE_WRITE);
    din=String("a")+","+String("b")+","+String("c")+","+String("d");
  file.println(String(din));
  file=SD.open("/count.txt",FILE_READ);
  Serial.println("/count.txt");
  file.close();*/


}
