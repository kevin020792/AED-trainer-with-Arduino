 
/*Warning: This is an AED trainer, DO NOT ATTEMPT TO APPLY IT TO ANY MEDICAL USAGE */
/*This project used some parts of the scripts from [Angelo qiao](Angelo.qiao@dfrobot.com) and [Ken Shirriff] as refrences*/

#include "Arduino.h"  
#include "SoftwareSerial.h"                    
#include "DFRobotDFPlayerMini.h"  // Use DFRobotDFPlayerMini library 
#include "IRremote.h" //Use IRremote library

SoftwareSerial mySoftwareSerial(2, 3);
//mySoftwareSerial(RX, TX), declare software serial port  

int RECV_PIN = 11;
long pre_time_pad = 0;
long pre_time_shock = 0;
long pre_time_buzzer_latency = 0;
long pre_time_beep = 0;
int led_state_pad = LOW; //LED light signal for "apply the pad"
int led_state_shock = LOW; //LED light signal for "stand by to shock"
int buzzer_state = LOW; 
int flash_time = 500; //The LED light signal for "stand by to shock" will flash per 500ms
int buzzer_time = 200; //The buzzer will beep per 600ms, 200 HIGH and 400 LOW
long beep_time = 120000; //The buzzer will only work for 2 minutes (120000 ms)
int pad_led = 0; //The index for the LED signal "apply the pad", 0 and other: LOW; 1 for flash and 2 for always HIGH
bool pluged = false; //Check if the pad is attached
bool detected = false; //Check if the shock button is pressed
bool gotoshock = false; //Control the circle 
bool butt = false;
bool cprbeep = false; 
bool shock_led =false;
bool beeped = false; //Control the On/Off of the buzzer

IRrecv irrecv(RECV_PIN);

decode_results results;
  
DFRobotDFPlayerMini myDFPlayer;         //Declare MP3 Player  
void printDetail(uint8_t type, int value);  //Print the details of the DFPlayer Mini  
  
// the setup function runs once when you press reset or power the board  
void setup()   
{  
    Serial.begin(115200);          //Define Serial Baud
    mySoftwareSerial.begin(9600);  //Define mySoftwareSerial baud
  
    Serial.println(F("DFRobot DFPlayer Mini Demo"));   
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));  
    bool is_init_success = myDFPlayer.begin(mySoftwareSerial);  

    //Setup the pins for LEDs
     pinMode(4, OUTPUT); //System is booting up
     pinMode(5, OUTPUT); //Stay calm and call 911
     pinMode(6, OUTPUT); //Apply the pads and plug the electrodes
     pinMode(7, OUTPUT); //Shock recommended
     
     //Setup the pin to detect the button
     pinMode(8, INPUT); //Pin 8 for the shock button
    
     //Setup the pin to detect Busy on DFPlayer Mini
     pinMode(9, INPUT); //Pin 9 for the "Busy" pin on DFPlayer Mini
     
     //Setup the pin for buzzer
     pinMode(10, OUTPUT); //5V buzzer on Pin 10
     
     //Shut down all LED
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
     digitalWrite(6, LOW);
     digitalWrite(7, LOW);
     //All LED are LOW now
     
     irrecv.enableIRIn(); // Start the IR receiver
    
    if (is_init_success == false)    // If DFPlayer Mini is not available.  
    {  //Use softwareSerial to communicate with mp3.   
        Serial.println(F("Unable to begin:"));                      
        Serial.println(F("1.Please recheck the connection!"));  
        Serial.println(F("2.Please insert the SD card!"));  
        while (true);                                       
    }  
     
    Serial.println(F("DFPlayer Mini online."));
    // Print "DFPlayer Mini online." if it is available 
    
     myDFPlayer.setTimeOut(500); // 設定通訊逾時為500ms  
  
     //----Set volume----  
     myDFPlayer.volume(25);     // 設定音量, 範圍0~30.   
  
     //----Set different EQ----  // 設定EQ(等化器 Equalizer)  
     myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);  
  
  
     myDFPlayer.enableDAC();     //Enable On-chip DAC  
     digitalWrite(4, HIGH); //LED signal for system booting
     myDFPlayer.play(1); //Play "System is booting up"
     delay(2500);
     Serial.println("System booting up");
     digitalWrite(5, HIGH); //LED signal for "Stay calm and call for help"
     myDFPlayer.play(2); //Play "Stay calm and call 911"
     delay(2500);
     Serial.println("Stay calm and call 911");
     pad_led = 1;
     myDFPlayer.loop(3); //Loop "Plug the electrode and apply the pads to bare skin as instructions"
     Serial.println("Plug the electrode and apply the pads to bare skin as instructions");
    
}  
  
void(* resetFunc) (void) = 0; //Reset Arduino

void loop()   
{  
    if (myDFPlayer.available())  // Monitoring the DFPlayer Mini 
    {                   
        printDetail(myDFPlayer.readType(), myDFPlayer.read());
        //Print the detail message from DFPlayer to handle different errors and states.  
    }  
      
    if (irrecv.decode(&results)) 
    {
      
       Serial.println(results.value); 
       if(results.value == 16724175)  //Press "1" to play "Evaluating heart rhythm, do not touch the patient"
       {  
        pad_led = 2;
        gotoshock = false;
        pluged = true; //This part must be run first to run the other demonstrations.
        digitalWrite(10, LOW);
        Serial.println(results.value);  
        myDFPlayer.loop(4);  
        Serial.println("play 4");    
                
      } 
      else if (results.value == 16718055){ //Press "2" to play "Shock recommended, press the shock button to deliver shocks"
         Serial.println(results.value); 
        if (pluged == true){
         pad_led = 2; 
         beeped = false;
         digitalWrite(10, LOW);
         myDFPlayer.loop(5);  
         Serial.println("play 5_Suggest_Defib");
         gotoshock = true;  //Engage the loop to detect whether the button is pressed or not
         detected = false;
         Serial.println("Go to the if loop for shock");
        }
      }
      else if (results.value ==16743045){ //Press "3" to play "No shock advised, please continue"
         Serial.println(results.value);  
        if(pluged == true){
          gotoshock = false;
          pad_led = 2;
          digitalWrite(10, LOW);
          myDFPlayer.play(6);  
          Serial.println("play 6_No_Defib_CPR"); 
          cprbeep = true;
          beeped = false;
        }
                
      }
      else if (results.value ==16754775){ //Press "+" to increase the volume
        Serial.println(results.value);  
        myDFPlayer.volumeUp();
        Serial.println("volume up");                
      }
      else if (results.value ==16769055){ //Press "-" to lower the volume
        Serial.println(results.value);  
        myDFPlayer.volumeDown();
        Serial.println("volume down");                
      }
      else if (results.value ==16748655){ //Press "EQ" button to reset the system
        resetFunc(); //Reset Arduino
      }
      else {
      
      }
       irrecv.resume(); // Receive the next value
     }   
     
    if (gotoshock == true){
    Serial.println("stand by to shock");
       if (detected == false){
          readpin8val();
          
          }
     
       if(butt == true){
          Serial.println("Shocked, Goto CPR");
          detected = true;
          myDFPlayer.play(7); //"Shock delivered, continue CPR"
          butt = false;
          cprbeep = true;  
        }
      
   }
    if (cprbeep == true){ //Prepare to make the buzzer simulate the compress frequency
     if(digitalRead(9) == HIGH){
      buzzer_control();
      gotoshock = false;
      
     }
    }
    
   pad_led_control(); //
   shock_led_control();
   
}    
        
      
  
  
void printDetail(uint8_t type, int value)   
{  
    switch (type) {  
    case TimeOut:  
        Serial.println(F("Time Out!"));  
        break;  
    case WrongStack:  
        Serial.println(F("Stack Wrong!"));  
        break;  
    case DFPlayerCardInserted:  
        Serial.println(F("Card Inserted!"));  
        break;  
    case DFPlayerCardRemoved:  
        Serial.println(F("Card Removed!"));  
        break;  
    case DFPlayerCardOnline:  
        Serial.println(F("Card Online!"));  
        break;  
    case DFPlayerPlayFinished:  
        Serial.print(F("Number:"));  
        Serial.print(value);  
        Serial.println(F(" Play Finished!"));  
        break;  
    case DFPlayerError:  
        Serial.print(F("DFPlayerError:"));  
        switch (value) {  
        case Busy:  
            Serial.println(F("Card not found"));  
            break;  
        case Sleeping:  
            Serial.println(F("Sleeping"));  
            break;  
        case SerialWrongStack:  
            Serial.println(F("Get Wrong Stack"));  
            break;  
        case CheckSumNotMatch:  
            Serial.println(F("Check Sum Not Match"));  
            break;  
        case FileIndexOut:  
            Serial.println(F("File Index Out of Bound"));  
            break;  
        case FileMismatch:  
            Serial.println(F("Cannot Find File"));  
            break;  
        case Advertise:  
            Serial.println(F("In Advertise"));  
            break;  
        default:  
            break;  
        }  
        break;  
    default:  
        break;  
    }  
}  

void readpin8val(){
  if (digitalRead(8)==HIGH){
        butt = true;
        Serial.println("電擊鈕被按下");
        //Warning signals for 400ms
        digitalWrite(10, HIGH);
        delay(400);
        digitalWrite(10, LOW);
        //Warning finished
       }
       else {
        butt = false;
       }
       
}

void pad_led_control(){
  if (pad_led ==1){
    int current_time = millis();
    if (current_time - pre_time_pad > flash_time){
      led_state_pad = !led_state_pad;
       pre_time_pad = current_time;
      }
    digitalWrite(6, led_state_pad);
   
  }
  else if (pad_led ==2){
    digitalWrite(6, HIGH);
  }
  else {
    digitalWrite(6, LOW);
  }
}

void shock_led_control() {
  if (gotoshock == true){
   
    if (detected == false){
    int current_time2 = millis();
       if (current_time2 - pre_time_shock > flash_time){
       led_state_shock = !led_state_shock;
       pre_time_shock = current_time2;
      }
       digitalWrite(7, led_state_shock);
      
      }
     else{
      digitalWrite(7,LOW);
     }
  }
}

void buzzer_control(){
    long current_time3 = millis();
    if (beeped == false) { //Give a snapshot of current time for timer
      pre_time_buzzer_latency = current_time3;
    }
    
    
    if (current_time3 - pre_time_buzzer_latency > beep_time) {
      Serial.println(current_time3);
      pre_time_buzzer_latency = current_time3;
      buzzer_state = LOW;
      cprbeep = false;
      digitalWrite(10, LOW);
      myDFPlayer.loop(4); //2 minutes up, loop "Evaluating heart rhythm, do not touch the patient"
    }
    else {
      long current_time4 = millis();
       if (current_time4 - pre_time_beep > buzzer_time ){
          pre_time_beep = current_time4;
           beeped = true;
        //If buzzer is beeping, silent it for 400 ms, else, beep for 200 ms
          if (buzzer_state == HIGH) {
           buzzer_state = LOW;
           buzzer_time = 400;
          }
          else {
           buzzer_state = HIGH;
           buzzer_time = 200;
          }
        digitalWrite(10, buzzer_state);
        Serial.print("Buzzer state now:");
        Serial.println(buzzer_state);
       }

   
    }

 }
 
