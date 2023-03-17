#include <EEPROM.h>
#include <ESP32CAN.h>
#include "RTClib.h"
#include <CAN_config.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

const char* ssid     = "moto";
const char* password = "athul1234567";

#define FIREBASE_HOST "https://sterilizer.firebaseio.com/"
#define FIREBASE_AUTH "pnUwQVHcBf5AJ0fObO7d5oVDmnYIS87E1VQgiNQT" 

#define CONNECTION_TIMEOUT 10

FirebaseData firebaseDataUv;
FirebaseJson json;

String UvPath = "/CANnetwork/uv";

bool uvLight = false;
int falseflag;

void checkUvStateFromDb(String uv){
  if(uv == "ON"){
    uvLight = true;
  }
  else if(uv == "OFF"){
    uvLight = false;
  }
  else if(uv == "OFF1"){
    uvLight = false;
  }
}

void putDataToDb(String location,String dataToWrite){
  Firebase.setString(firebaseDataUv, location,  dataToWrite);
}

CAN_device_t CAN_cfg;
RTC_DS1307 DS1307_RTC;
int Hour, Minute, total_minutes=0, ref_time=0, d, e;

void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  pinMode(15,OUTPUT);
  digitalWrite(15,LOW);
  if (!DS1307_RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while(1);
  }
  EEPROM.begin(512);
  delay(10);

  byte a = EEPROM.read(0);

  byte hr = EEPROM.read(1);

  byte mn = EEPROM.read(2);

  ref_time=hr*60 + mn;

 if(a==0){
  CAN_frame_t rx_frame;
  rx_frame.FIR.B.FF = CAN_frame_std;
  rx_frame.MsgID = 1;
  rx_frame.FIR.B.DLC = 2;
  rx_frame.data.u8[1] = 'b' ;
  ESP32Can.CANWriteFrame(&rx_frame);
  online();
  }
 else{
  CAN_cfg.speed=CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
  ESP32Can.CANInit(); 
  normal();
  }
 
}

void normal(){
  CAN_frame_t rx_frame;
  rx_frame.FIR.B.FF = CAN_frame_std;
  rx_frame.MsgID = 1;
  rx_frame.FIR.B.DLC = 2;
  rx_frame.data.u8[1] = 'a' ;
  ESP32Can.CANWriteFrame(&rx_frame);
  while(1){
    DateTime now = DS1307_RTC.now(); 
    
    Hour=now.hour();
    Serial.print(Hour);
    Serial.print(':');
 
    Minute=now.minute();
    Serial.println(Minute);
     
    total_minutes=Hour*60 + Minute;  
    Serial.println(total_minutes);
    if(total_minutes-ref_time>=1){
      digitalWrite(2,HIGH);
    }
    if (total_minutes-ref_time>=2){
      EEPROM.write(0,0);
      EEPROM.commit();     
      ESP.restart();
    }
    delay(1000);
  }
}

void online(){

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    timeout_counter++;
    if(timeout_counter >= CONNECTION_TIMEOUT*5){
     ESP.restart();
    }
    delay(500);
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Serial.println("------------------------------------");
  Serial.println("Connected to Firebase...");
  
  if (!Firebase.beginStream(firebaseDataUv, UvPath))
  {
    falseflag = 1;
  }

  Firebase.setReadTimeout(firebaseDataUv, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseDataUv, "tiny");
  digitalWrite(15, HIGH); 
  while(1){

    if (!Firebase.readStream(firebaseDataUv)){
      falseflag = 2;
    }

    if (firebaseDataUv.streamTimeout()){
      falseflag = 3;
    }

    if (firebaseDataUv.streamAvailable()){
      checkUvStateFromDb(firebaseDataUv.stringData());
    }
    
    if(uvLight){
     digitalWrite(2, HIGH);
     
     EEPROM.write(0,1);
     EEPROM.commit();

     putDataToDb(UvPath, "OFF");
     
     DateTime now = DS1307_RTC.now(); 
     d=now.hour();
     e=now.minute();
     
     EEPROM.write(1,d);
     EEPROM.commit();

     EEPROM.write(2,e);
     EEPROM.commit();
     ESP.restart(); 
    }
    
  }
  
}

void loop(){
  
}
