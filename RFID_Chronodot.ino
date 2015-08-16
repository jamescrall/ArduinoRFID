//Based on RFID cat door dude's code


#include <SoftwareSerial.h>
#include <SD.h>
#include <Wire.h>
#include "Chronodot.h"

int val;
const int ledPin = 13;
const int RF_RECEIVE_pin=2; 
const int RF_TRANSMIT_pin=3; 
SoftwareSerial mySerial(RF_RECEIVE_pin, RF_TRANSMIT_pin);
Chronodot RTC;
int incomingByte = 0; 
int led_state=0;



void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("RFID test!");
  pinMode(ledPin, OUTPUT);
   if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  Serial.println("Time ready!");
  Serial.println("Serial Ready");
  pinMode(10,OUTPUT);
  RTC.begin();
  Wire.begin();
 // RTC.adjust(DateTime(__DATE__, __TIME__));
}

void loop() 
{
  
    if(RFID()==1)
    {
        digitalWrite(ledPin, HIGH);
    }
      else
    {
        digitalWrite(ledPin, LOW);
    }
}

int RFID() 
  {
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;
  int result = 0;

  if(mySerial.available()) {
    result=1;
    //Serial.println(mySerial.read());
    if((val = mySerial.read()) == 0x02) {// check for header 
      bytesread = 0; 
      Serial.println("I read it");
      while (bytesread < 12) {// read 10 digit code + 2 digit checksum
        if(mySerial.available()) { 
          val = mySerial.read();
          //Serial.println(val,HEX);
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { // if header or stop bytes 
            break; // stop reading
          }

          // Do Ascii/Hex conversion:
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          // Every two hex-digits, add byte to code:
          if (bytesread & 1 == 1) {
            // make some space for this hex-digit by
            // shifting the previous hex-digit with 4 bits to the left:
            code[bytesread >> 1] = (val | (tempbyte << 4));

            if (bytesread >> 1 != 5) {// If we're at the checksum byte,
              checksum ^= code[bytesread >> 1]; // Calculate the checksum... (XOR)
            };
          } else {
            tempbyte = val; // Store the first hex digit first...
          };
          bytesread++; // ready to read next digit
        } 
      } 

      // Output to Serial:

      if (bytesread == 12) { // if 12 digit read is complete
        Serial.print("5-byte code: ");
        for (i=0; i<5; i++) {
          if (code[i] < 16) mySerial.print("0");
          Serial.print(code[i], HEX);
         // Serial.print(" ");
        }
        
        
  ///Section to write to SD Card
  File RFIDlog = SD.open("RFIDlog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (RFIDlog) {
    for (i=0; i<5; i++) {
          if (code[i] < 16) mySerial.print("0");
          RFIDlog.print(code[i], HEX);
         // Serial.print(" ");
        }
    RFIDlog.print(",");
    DateTime now = RTC.now();
    RFIDlog.print(now.year());
    RFIDlog.print(",");
    RFIDlog.print(now.month());
    RFIDlog.print(",");
    RFIDlog.print(now.day());
    RFIDlog.print(",");
    RFIDlog.print(now.hour());
    RFIDlog.print(",");
    RFIDlog.print(now.minute());
    RFIDlog.print(",");
    RFIDlog.print(now.second());
    RFIDlog.println("");
    //RFIDlog.print(",");
    //RFIDlog.print(now.minute(), DEC);
    //RFIDlog.print(",");
    //RFIDlog.println(now.second(), DEC);
    RFIDlog.close();
    
    //Serial.println(l);
    //Serial.print(",");
    //Serial.print(now.minute(), DEC);
    //Serial.print(",");
    //Serial.println(now.second(), DEC);

    // print to the serial port too:
    //Serial.println(l);
    //Serial.println("success!");
  }  
        
        
        }
        
        
        
        Serial.println();
        
                //Extract time, add to output
        DateTime now = RTC.now();
        Serial.print("time: ");
        Serial.print(now.year());
        Serial.print("/");
        Serial.print(now.month());
        Serial.print("/");
        Serial.print(now.day());
        Serial.print(" ");
        Serial.print(now.hour());
        Serial.print(":");
        Serial.print(now.minute());
        Serial.print(":");
        Serial.print(now.second());
        Serial.println();
        Serial.print("Checksum: ");
        Serial.print(code[5], HEX);
        Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
        if(code[5] == checksum)
        result=1;
        Serial.println();
      }
      bytesread = 0;
    }
  }
  //return result;
 

