  //Code for reading and recording timestampes for FDX-A rfid tags
  
  //Based in part on RFID cat door instructable, by landmanr
  /* http://www.instructables.com/id/RFID-cat-door/ */
  
  
  
  /*This code hijacks the primary serial port (on pins 0 and 1) normally used to communicate with the computer
  and uses it to communicate with a second rfid module through that serial port. All connections to the first serial port (pins 0 and 1)
  must be removed when uploading, then replaced before running the script from wall power*/
    
  //Include libraries
  #include <SoftwareSerial.h>
  #include "RTClib.h"
  #include <SD.h>
  #include <Wire.h>
  
  
  int val;
  const int ledPin = 13;
  
  //Set up serial connection to rfid module
  SoftwareSerial mySerial(2, 3);
  
  RTC_DS1307 RTC;
  int incomingByte = 0; 
  int led_state=0;
  
  
  //Initial setup loop
  void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    pinMode(ledPin, OUTPUT);
    
    //Check to see if the SD card is present and wired up correctly
     if (!SD.begin(10)) {
      Serial.println("initialization failed!");
      return;
    }
    
    pinMode(10,OUTPUT);
    RTC.begin();
    Wire.begin();
    
    //Uncomment line below if you want to reset the clock. Upload the code, then uncomment and upload again
    //before running the script "untethered" i.e. from wall power
    
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  void loop() 
  {
     byte i = 0;
    byte val = 0;
    byte code[6];
    byte checksum = 0;
    byte bytesread = 0;
    byte tempbyte = 0;
    int result = 0;
  
  
  
  
  
    //Loop for "Inner" antenna
    if(Serial.available()) 
    {
                result=1;
                //Serial.println(mySerial.read());
                if((val = Serial.read()) == 0x02) {// check for header 
                                bytesread = 0; 
                          
                                while (bytesread < 12) {// read 10 digit code + 2 digit checksum
                                  if(Serial.available()) { 
                                    val = Serial.read();
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
                                  //Serial.print("5-byte code: ");
                                                    for (i=0; i<5; i++) {
                                                      if (code[i] < 16) Serial.print("0");
                                                      //Serial.print(code[i], HEX);
                                                     // Serial.print(" ");
                                                    }
                                  
                                  
                            ///Section to write to SD Card
                            File RFIDlog = SD.open("RFIDlog.csv", FILE_WRITE);
                          
                                          // if the file is available, write to it:
                                          if (RFIDlog) {
                                            for (i=0; i<5; i++) {
                                                  if (code[i] < 16) Serial.print("0");
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
                                            RFIDlog.print(",");
                                            RFIDlog.print("Inner");
                                            RFIDlog.println("");
                                            RFIDlog.close();
                                            
                                        
                                                       }  
                                                
                                  
                                              }
                                  
                                  
                                  
                  }
                  bytesread = 0;
      }
      
      
      
      
      
      
      
      
   //Loop for "Outer" antenna
      if(mySerial.available()) 
    {
                  result=1;
                  //Serial.println(mySerial.read());
                  if((val = mySerial.read()) == 0x02) {// check for header 
                    bytesread = 0; 
                          
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
                                  //Serial.print("5-byte code: ");
                                  for (i=0; i<5; i++) {
                                    if (code[i] < 16) mySerial.print("0");
                                    //Serial.print(code[i], HEX);
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
                                          RFIDlog.print(",");
                                          RFIDlog.print("Outer");
                                          RFIDlog.println("");
                                          RFIDlog.close();
                                          
                                      
                                        }  
                                  
                                  
                                      }
                                  
                                  
                                  
                      }
                    bytesread = 0;
      }
     
     
     
     
  }
  
  
  
    
    
   
