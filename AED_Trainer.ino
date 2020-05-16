 
/*Warning: This is an AED trainer, DO NOT ATTEMPT TO APPLY IT TO ANY MEDICAL USAGE */
/*This project used some parts of the scripts from [Angelo qiao](Angelo.qiao@dfrobot.com) and [Ken Shirriff] as refrences*/

#include "Arduino.h"  
#include "SoftwareSerial.h"                    
#include "DFRobotDFPlayerMini.h"        // 採用DFRobotDFPlayerMini程式庫  
#include "IRremote.h" //使用IRremote程式庫

SoftwareSerial mySoftwareSerial(2, 3);
//mySoftwareSerial(RX, TX),宣告軟體序列傳輸埠  

int RECV_PIN = 11;
long pre_time_pad = 0;
long pre_time_shock = 0;
long pre_time_buzzer_latency = 0;
long pre_time_beep = 0;
int led_state_pad = LOW; //貼上貼片LED燈燈號
int led_state_shock = LOW; //電擊LED燈燈號
int buzzer_state = LOW;
int flash_time = 500; //電擊鈕LED燈每500秒閃爍一次
int buzzer_time = 200; //蜂鳴器每600毫秒交替一次，每次響200ms沉默400ms，配合CPR每分鐘100下的頻率
long beep_time = 120000; //蜂鳴器只會運作2分鐘(120000ms);
int pad_led = 0; //貼上貼片LED燈燈號狀態指標，0與其他熄滅、1閃爍、2恆亮
bool pluged = false; //檢測教官是否已按下"不要觸碰病人，正在分析"
bool detected = false; //按鈕有沒有被按下過
bool gotoshock = false; //使用boolean控制是否進入要走電擊迴圈
bool butt = false;
bool cprbeep = false;
bool shock_led =false;
bool beeped = false; //開啟或關閉吵死人的蜂鳴器

IRrecv irrecv(RECV_PIN);

decode_results results;
  
DFRobotDFPlayerMini myDFPlayer;         //宣告MP3 Player  
void printDetail(uint8_t type, int value);  //印出詳情  
  
// the setup function runs once when you press reset or power the board  
void setup()   
{  
    Serial.begin(115200);            // 定義Serial Baud
    mySoftwareSerial.begin(9600);  // 定義mySoftwareSerial baud
  
    Serial.println(F("DFRobot DFPlayer Mini Demo"));   
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));  
    bool is_init_success = myDFPlayer.begin(mySoftwareSerial);  

    //設定LED
     pinMode(4, OUTPUT); //系統正常
     pinMode(5, OUTPUT); //保持鎮定尋求救援
     pinMode(6, OUTPUT); //貼上電極貼片
     pinMode(7, OUTPUT); //建議電擊按鈕閃爍
     //LED設定完成
     //設定按鈕
     pinMode(8, INPUT); //按鈕用的接腳 
     //按鈕設定完成
     //設定Busy輸入
     pinMode(9, INPUT); //Busy接腳用，偵測HIGH或LOW
     //Busy偵測接腳已指定
     //設定蜂鳴器
     pinMode(10, OUTPUT); //5V蜂鳴器接在Pin 10
     //蜂鳴器接腳已指定
     //燈號全暗
     digitalWrite(4, LOW);
     digitalWrite(5, LOW);
     digitalWrite(6, LOW);
     digitalWrite(7, LOW);
     //燈號已全暗
     
     irrecv.enableIRIn(); // Start the IR receiver
    
    if (is_init_success == false)    // 如果DFPlayer Mini回應不正確.  
    {  //Use softwareSerial to communicate with mp3.   
        Serial.println(F("Unable to begin:"));                      
        Serial.println(F("1.Please recheck the connection!"));  
        Serial.println(F("2.Please insert the SD card!"));  
        while (true);                                       
    }  
     
    Serial.println(F("DFPlayer Mini online."));
    // 如果DFPlayer Mini回應正確.印出"DFPlayer Mini online."  
    
     myDFPlayer.setTimeOut(500); // 設定通訊逾時為500ms  
  
     //----Set volume----  
     myDFPlayer.volume(25);     // 設定音量, 範圍0~30.   
  
     //----Set different EQ----  // 設定EQ(等化器 Equalizer)  
     myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);  
  
  
     myDFPlayer.enableDAC();     //Enable On-chip DAC  
     digitalWrite(4, HIGH); //系統指示燈亮起
     myDFPlayer.play(1); //系統正常
     delay(2500);
     Serial.println("sys normal played");
     digitalWrite(5, HIGH); //保持鎮定尋求救援燈亮起
     myDFPlayer.play(2); //保持鎮定尋求救援
     delay(2500);
     Serial.println("stay calm played");
     pad_led = 1;
     myDFPlayer.loop(3); //開始要求插電極
     Serial.println("Plug the pad and wait for instructions");
    
}  
  
void(* resetFunc) (void) = 0; //重啟指令集

void loop()   
{  
    if (myDFPlayer.available())  // 監視MP3有沒有回應  
    {                                          // 有的話印出詳情  
        printDetail(myDFPlayer.readType(), myDFPlayer.read());
        //Print the detail message from DFPlayer to handle different errors and states.  
    }  
      
    if (irrecv.decode(&results)) 
    {
      
       Serial.println(results.value); //教官按下"1"，學員已貼貼片
       if(results.value == 16724175)  //撥放不要觸碰病人，正在分析
       {  
        pad_led = 2;
        gotoshock = false;
        pluged = true;
        digitalWrite(10, LOW);
        Serial.println(results.value);  
        myDFPlayer.loop(4);  
        Serial.println("play 4");    
                
      } 
      else if (results.value == 16718055){ //教官按下"2"，撥放建議除顫
         Serial.println(results.value); 
        if (pluged == true){
         pad_led = 2; 
         beeped = false;
         digitalWrite(10, LOW);
         myDFPlayer.loop(5);  
         Serial.println("play 5_Suggest_Defib");
         gotoshock = true;  //啟動偵測按鈕迴圈
         detected = false;
         Serial.println("Go to the if loop for shock");
        }
      }
      else if (results.value ==16743045){ //教官按下"3"，撥放不建議除顫
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
      else if (results.value == 16732845) { //教官按下"9"，播搞笑
        gotoshock = false;
        cprbeep = false;
        digitalWrite(10, LOW);
        Serial.println(results.value);
        myDFPlayer.play(9); //撥放"皮諾可這個直接電死"
        Serial.println("play 9");
      }
      else if (results.value ==16754775){ //教官按下"+"，增加音量
        Serial.println(results.value);  
        myDFPlayer.volumeUp();
        Serial.println("volume up");                
      }
      else if (results.value ==16769055){ //教官按下"-"，降低音量
        Serial.println(results.value);  
        myDFPlayer.volumeDown();
        Serial.println("volume down");                
      }
      else if (results.value ==16748655){ //教官按下EQ，重新來過
        resetFunc(); //執行重啟指令
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
          myDFPlayer.play(7); //電擊完成，繼續CPR
          butt = false;
          cprbeep = true;  
        }
      
   }
    if (cprbeep == true){ //準備撥放CPR頻率聲
     if(digitalRead(9) == HIGH){
     // myDFPlayer.play(8); //2分鐘逼逼聲
     //cprbeep = false;
      buzzer_control();
      gotoshock = false;
      
     }
    }
    
   pad_led_control(); //貼上電擊貼片並插入電極的LED指示燈，1是閃爍2是恆亮
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
        //發出電擊警告音400ms
        digitalWrite(10, HIGH);
        delay(400);
        digitalWrite(10, LOW);
        //完成警示
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
      myDFPlayer.loop(4); //時間到重覆撥放"不要觸碰病人，正在分析"
    }
    else {
      long current_time4 = millis();
       if (current_time4 - pre_time_beep > buzzer_time ){
          pre_time_beep = current_time4;
           beeped = true;
        //判斷迴圈，如果Buzzer已經在叫，改成閉嘴400ms，如果沒有叫，那就叫200ms
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
 
