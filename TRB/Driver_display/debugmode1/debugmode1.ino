
const int toggleSwitch_pin = 2;
const int neutral_Pin = 3;
const int acclmt_pin = 5; 
const int brakeswitch_pin = 16;
const int start_pin = 17;
const int kill1_pin = 21;
const int kill2_pin = 23;
const int lv_bat_pin = 32;

bool lv_bat_status
bool debugMode_status = false;
bool toggleSwitch_status = false;
bool neutral_Status = false;
bool brakeswitch_status = false;
bool start_pin = false;
bool kill1_status = false;
bool kill2_status = false;

void setup() 
{
  pinMode(toggleSwitch_pin, INPUT_PULLUP);
  pinMode(neutral_Pin, INPUT_PULLUP);
  pinMode(acc_lmt_pin, INPUT_PULLUP);
  
  Serial.begin(9600);

void startup_seq()
{
    int toggleSwitch_status = digitalRead(toggleSwitch_pin);    //Read toggle pin
  
  if (toggleSwitch_status == HIGH) 
  {
    debugMode_status = true;
    Serial.println("Debug mode activated");
    
    // debugging mode code
    while(debugMode_status == true)
    {
      int neutral_Status = analogRead(neutral_Pin);
      if(2.4 < neutral_Status <= 3.3)
      {
        neutral_Status=true;
        while(neutral_Status==true)
        {
          int acclmt_status = analogRead(acclmt_pin);
          if(2.4 < acclmt_status <= 3.3)
          {
             acclmt_status=true;
             Serial.println("!!! acceleration pedal");
          }
          else
          {
            acclmt_status=false;
            while(acclmt_status==false)
            {
               int brakeswitch_status = analogRead(brakeswitch_pin);
               if(2.4 < brakeswitch_status <= 3.3)
               {
                  brakeswitch_status=true;
                  while(brakeswitch_status==true)
                  {
                    int start_status = analogRead(start_pin);
                    if(2.4 < start_status <= 3.3)
                    {
                      start_status=true;
                      while(start_status==true)
                      {
                        int kill1_status = analogRead(kill1_pin);
                        int kill2_status = analogRead(kill2_pin);
                        if((2.4 < kill1_status <= 3.3) || (2.4 < kill2_status <= 3.3))
                        {
                          kill1_status , kill2_status == true;
                          Serial.println("lesss gooooooo mfs");
                          //call next debug function here
                        }
                        else
                        {
                          Serial.println("!!! start");
                        }
                      }
                    }
                    else
                    {
                      Serial.println("!!! start");
                    }
                  }
               }
               else
               {
                  Serial.println("!!! brakes");
               }
            }
          }
        }
      }
      else
      {
        Serial.println("!!!neutral");
      }
    }
  } 
  else 
  {
    if (debugMode)
    {
      debugMode = false;
      Serial.println("Debug mode deactivated");
    }
    // normal code without debugging mode
  } 
  //delay to avoid rapid toggling
  delay(100);
}
void lv_battery()
{
  int lv_bat_status = analogRead(lv_bat_pin);
  if(3.3 < lv_bat_pin < 2.7 )
  {
    Serial.println("lv battery check");
    startup_seq();
  }
  else
  {
    Serial.println("charge the battery");
  }
}

void loop(){}
