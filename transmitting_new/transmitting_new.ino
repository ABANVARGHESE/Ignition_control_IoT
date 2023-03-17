#include <SPI.h>
#include "mcp2515_can.h"

const int spiCSPin = 10;
const int buttonPin = 2;  
int buttonState = 0;   
int c = 0 ;
int esp_led = 4;
mcp2515_can CAN(spiCSPin);
unsigned char stmp[1];

void setup()
{
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    c++;
    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS Shield Init OK!");
}

    
void loop()
{   
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) 
  {
    while (digitalRead(buttonPin) == HIGH);
    { 
     Serial.println(c);
     c++;
     if(c > 2){
      c=1;
     }
     if(c == 2){
      digitalWrite(4, HIGH);
      stmp[0]='1';
      CAN.sendMsgBuf( 0x43 , 0 , 0 , 1 , stmp );
     }
     else if(c == 1){
      digitalWrite(4, LOW);
      stmp[0]='2';
      CAN.sendMsgBuf( 0x43 , 0 , 0 , 1 , stmp );
     }
    }
  }  
  delay(100);
}
