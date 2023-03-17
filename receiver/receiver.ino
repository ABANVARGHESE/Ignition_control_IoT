#include <SPI.h>
#include "mcp2515_can.h"

const int motorPin1 = A3; 
const int motorPin2 = A4; 
const int enablePin = A2; 

const int pin = 4;


const int spiCSPin = 10;
mcp2515_can CAN(spiCSPin);

void setup(){   
    pinMode(4 ,OUTPUT);
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(enablePin, OUTPUT);
    digitalWrite(pin,LOW);
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)){
     Serial.println("CAN BUS Init Failed");
     delay(100);
    }
    Serial.println("CAN BUS  Init OK!");
    
}


void loop()
{

    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        CAN.readMsgBuf(&len, buf);

        unsigned long canId = CAN.getCanId();

        Serial.println("-----------------------------");
        Serial.print("Data from ID: 0x");
        Serial.println(canId, HEX);

        for(int i = 0; i<len; i++)
        {   
            Serial.print(buf[i]);
            Serial.print("\t");
           
            if(buf[0]==49 && buf[1]==97)
            {digitalWrite(motorPin1, HIGH);
             digitalWrite(motorPin2, LOW);
             analogWrite(enablePin, 255);
             digitalWrite(4,HIGH);
            }
            else if(buf[0]==50 && (buf[1]==98 || buf[1]==97))
            {
             analogWrite(enablePin, 0);
             digitalWrite(4,LOW);
            }
        }
        Serial.println();
    }







  
//    unsigned char len = 0;
//    unsigned char buf[8];
//
//    while(CAN_MSGAVAIL == CAN.checkReceive())
//    {
//        CAN.readMsgBuf(&len, buf);
//
//        unsigned long canId = CAN.getCanId();
//
//        Serial.println("-----------------------------");
//        break;
//        Serial.print("Data from ID: 0x");
//        Serial.println(canId, HEX);
//
//
//    }
//
//
//      for(int i = 0; i<len; i++)
//       {
//            Serial.print(buf[0]);
//            Serial.print("\t");
//            if(i==0)
//            {
//                if(buf[0]=='1'){
//                 digitalWrite(motorPin1, HIGH);
//                 digitalWrite(motorPin2, LOW);
//                 analogWrite(enablePin, 255); 
//                }
//                else if(buf[0]=='2'){
//                 digitalWrite(enablePin, LOW);
//                }
//            
//            }
//           
//       }
//        Serial.println();
 }       
