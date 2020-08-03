/*
 GDXLib Demo (v. 20200728, using the 0.82 cpp code)
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 The lines with the major indents are for using a 2-line display connected
 with three serial lines. Leave the line #define  TWO_LINE_DISPLAY_TX commented
 out unless you have such a display. And right now adding this is causing crashes
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
#define TWO_LINE_DISPLAY_TX //comment out for no DISPLAY on Arduino Nano 33 BLE

void setup(){
  Serial.begin(9600);
  delay(500);
  Serial.println("Searching for");
  Serial.println("GDX Sensor");
  char strBuffer[32]="unassigned";
  #if defined TWO_LINE_DISPLAY_TX
      CharDisplayInit();
      CharDisplayPrintLine(1,"Searching for");
      CharDisplayPrintLine(2,"GDX sensor");
      delay (2000);
  #endif //TWO_LINE_DISPLAY_TX  

  //GDX.Begin();  //use this line for proximity pairing
    //or
  GDX.Begin("GDX-ACC 0H1019K1",1, 500);//or specify device, channel and period here 
  Serial.print("Found: ");
  Serial.println(GDX.deviceName());
  Serial.print("deviceName; ");
  Serial.println(GDX.deviceName());
  Serial.print("channelName; ");
  Serial.println(GDX.channelName());
  Serial.print(" strlen(GDX.channelName() ");
  Serial.println(strlen(GDX.channelName()));
  
  Serial.print("channelUnits: ");
  Serial.println(GDX.channelUnits());
  Serial.print("Battery status: ");
  //Serial.print(GDX.batteryPercent());
  Serial.println("%, ");
  Serial.print("ChargeState: ");
  //Serial.print(GDX.chargeState());
  Serial.println(" (0=idle, 1=charging, 2=charging complete, 3=error)");
  Serial.print("Bluetooth signal strength (RSSI): "); 
  //Serial.println(GDX.RSSI());
  #if defined TWO_LINE_DISPLAY_TX
      CharDisplayPrintLine(1,"Found: ");
      //CharDisplayPrintLine(2,GDX.deviceName());
      delay(1000);
      //CharDisplayPrintLine(1,GDX.channelName());
      //CharDisplayPrintLine(1,GDX.channelUnits());
      delay(1000);
      CharDisplayPrintLine(1, "battery PC: ");
      //sprintf(strBuffer, "%d", GDX.batteryPercent());
      //CharDisplayPrintLine(2, strBuffer);//
      delay(1000);
      CharDisplayPrintLine(1, "charge state: ");
      //sprintf(strBuffer, "%d", GDX.chargeState());
      //CharDisplayPrintLine(2, strBuffer);//
      delay(1000);
      CharDisplayPrintLine(1, "RSSI: ");
      //sprintf(strBuffer, "%d", GDX.RSSI());
      //CharDisplayPrintLine(2, strBuffer);//
      delay(1000);
  #endif //TWO_LINE_DISPLAY_TX  
        
  for(int row=1;row<100;row++){
     Serial.print(row);
     Serial.print(" ");
     float channelReading =GDX.readSensor();
     Serial.print(channelReading);
     Serial.print(" ");
     //Serial.println(GDX.channelUnits());
     Serial.println(" ");
     #if defined TWO_LINE_DISPLAY_TX
           //char strBuffer[32];
           //sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
           //CharDisplayPrintLine(1,GDX.channelName());
           //CharDisplayPrintLine(2,GDX.channelUnits());
           //CharDisplayPrintLine(2, strBuffer);//
       #endif //TWO_LINE_DISPLAY_TX  
       delay(500);   
       }//end of for
     GDX.Stop();
       Serial.println("series of readings taken; stopping GDX data collection");
}//end of setup

void loop(){
 /*
  //char strBuffer[32];
  Serial.println("top of loop");
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.println(" units go here");
  #if defined TWO_LINE_DISPLAY_TX
     //sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
     CharDisplayPrintLine(1,GDX.channelName());
     //CharDisplayPrintLine(2,strBuffer);
  #endif //TWO_LINE_DISPLAY_TX  //Serial.println(GDX.channelUnits());
  delay (500);
  */
}

        // Both the functions below are used for TWO_LINE_DISPLAY with serial connection,
        // and are not needed if you are using an Arduino Uno WiFi and the display on connected to
        // the Vernier Arduino shield.
        
        void CharDisplayPrintLine(int line, const char* strText){
          uint8_t lineCode = 128;
          if (line == 2) lineCode = 192;
        
          // Force a field width of 16.
          // Left justified with space padding on the right.
          char strBuffer[64];
          sprintf(strBuffer, "%-16.15s",  strText);
        
          //For debug -- prints what goes to the display
          Serial.print("[");
          Serial.print(strBuffer);
          Serial.print(" strBuffer ");
          Serial.println("]");
          Serial.println(strlen(strBuffer));
          Serial.println("]");
          
          Serial1.write((uint8_t)254); 
          Serial1.write(lineCode);  
          Serial1.write(strBuffer, 15);
        }
      
        void CharDisplayInit(){
          Serial1.begin(9600);
          delay(500);
        
          //Clear the screen
          Serial1.write((uint8_t)254);
          Serial1.write((uint8_t)1);
        
          // Move cursor to beginning of line1
          Serial1.write(254); 
          Serial1.write(128);  
        }
          void ClearScreen(){
        
          //Clear the screen
          Serial1.write((uint8_t)254);
          Serial1.write((uint8_t)1);
        
          // Move cursor to beginning of line1
          Serial1.write(254); 
          Serial1.write(128);  
        }
        
 
