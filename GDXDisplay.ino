/*
 GDXLib Demo (v. 20200809, using the 0.83 with then old ReadSensor cpp code)
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

void setup(){
  Serial.begin(9600);
  delay(500);
  Serial.println("Searching for");
  Serial.println("GDX Sensor");
  char strBuffer[32]="unassigned";
      // 2-LINE DISPLAY CODE
      CharDisplayInit();
      CharDisplayPrintLine(1,"Searching for");
      CharDisplayPrintLine(2,"GDX sensor");
      delay (2000);
      //2-LINE DISPLAY CODE*/
  GDX.Begin();  //use this line for proximity pairing
      //or
  //GDX.Begin("GDX-ACC 0H1019K1",1, 200);//or specify device, channel and period here 
  //GDX.Begin("GDX-RB 0K2000F4",2, 200);
  Serial.print("Found: ");
  Serial.println (GDX.deviceName());
  
  Serial.print("channelName; ");
  Serial.println (GDX.channelName());

  Serial.print("channelUnits: ");
  Serial.println (GDX.channelUnits());
  
  Serial.print("Battery status(%): ");
  Serial.println (GDX.batteryPercent());
  
  Serial.print("ChargeState: ");
  Serial.print (GDX.chargeState());
  Serial.println(" (0=idle, 1=charging, 2=charging complete, 3=error)");
  
  Serial.print("Bluetooth signal strength (RSSI): "); 
  Serial.println (GDX.RSSI());
  
  Serial.print("channelName; ");
  Serial.println (GDX.channelName());

  Serial.print("channelUnits: ");
  Serial.println (GDX.channelUnits());
  
  Serial.print("Battery status(%): ");
  Serial.println (GDX.batteryPercent());
  
  Serial.print("ChargeState: ");
  Serial.print (GDX.chargeState());
  Serial.println(" (0=idle, 1=charging, 2=charging complete, 3=error)");
  
  Serial.print("Bluetooth signal strength (RSSI): "); 
  Serial.println (GDX.RSSI());
      // 2-LINE DISPLAY CODE
      CharDisplayPrintLine(1,"Found: ");
      CharDisplayPrintLine (2,GDX.deviceName());
      delay(1000);
      
      CharDisplayPrintLine (1,GDX.channelName());
      CharDisplayPrintLine (2,GDX.channelUnits());
      delay(1000);
             
      CharDisplayPrintLine(1, "battery PC: ");
      sprintf(strBuffer, "%d", GDX.batteryPercent());
      CharDisplayPrintLine(2, strBuffer);
      delay(100);
               
      CharDisplayPrintLine(1, "charge state: ");
      sprintf(strBuffer, "%d", GDX.chargeState());
      CharDisplayPrintLine(2, strBuffer);
      delay(100);
      
      CharDisplayPrintLine(1, "RSSI: ");
      sprintf(strBuffer, "%d", GDX.RSSI());
      CharDisplayPrintLine(2, strBuffer);
      delay(100);
     //2-LINE DISPLAY CODE 
        
  for(int row=1;row<11;row++){
     Serial.print(row);
     Serial.print(" ");
     //float channelReading =row*row;//fake readings
     float channelReading =GDX.readSensor();
     Serial.print(channelReading);
     Serial.print(" ");
     Serial.println (GDX.channelUnits());
     // 2-LINE DISPLAY CODE
           //char strBuffer[32];
           sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
           CharDisplayPrintLine (1,GDX.channelName());
           CharDisplayPrintLine (2,GDX.channelUnits());
           CharDisplayPrintLine(2, strBuffer);
       //2-LINE DISPLAY CODE */
      // delay(500);   
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
  Serial.println(GDX.channelUnits());
  
  // 2-LINE DISPLAY CODE
  //   //sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
  //   CharDisplayPrintLine (1,GDX.channelName());
  //   CharDisplayPrintLine(2,strBuffer);
  //2-LINE DISPLAY CODE
  
  Serial.println(GDX.channelUnits());
  delay (500);
  */
}

        // Both the functions below are used for TWO_LINE_DISPLAY with serial connection,
        // and are not needed if you are using an Arduino Uno WiFi and the display on connected to
        // the Vernier Arduino shield.
        
        void CharDisplayPrintLine(int line, const char* strText){
          uint8_t lineCode = 128;
          delay(100);//!!!
          if (line == 2) lineCode = 192;
        
          // Force a field width of 16.
          // Left justified with space padding on the right.
          char strBuffer[32];
          sprintf(strBuffer, "%-16.15s",  strText);
        
          //For debug -- prints what goes to the display
          //Serial.print(" strBuffer ");
          //Serial.print("[");
          //Serial.print(strBuffer);
          //Serial.println("]");
          //Serial.println(strlen(strBuffer));
          
          Serial1.write((uint8_t)254); 
          delay(100);//!!!
          Serial1.write(lineCode);  
          delay(100);//!!!
          Serial1.write(strBuffer, 15);
        }
      
        void CharDisplayInit(){
          Serial1.begin(9600);
          delay(500);
        
          //Clear the screen
          Serial1.write((uint8_t)254);
          Serial1.write((uint8_t)1);
                  delay(100);//!!!
          // Move cursor to beginning of line1
          Serial1.write(254); 
          Serial1.write(128);  
        }
          void ClearScreen(){
        
          //Clear the screen
          Serial1.write((uint8_t)254);
                    delay(100);//!!!
          Serial1.write((uint8_t)1);
                  delay(100);//!!!
          // Move cursor to beginning of line1
          Serial1.write(254); 
                    delay(100);//!!!
          Serial1.write(128);  
        }
        
 
