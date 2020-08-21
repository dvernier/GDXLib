/*
 GDXLib Demo (v. 20200819, using the 0.85 )
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 The lines with the major indents are for using a 2-line display connected
 with three serial lines. Leave the line #define  TWO_LINE_DISPLAY_TX commented
 out unless you have such a display.
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
  char strBuffer[32]="unassigned";
  char strUnits[32]="strUnits";

void setup(){
  Serial.begin(9600);
  delay(500);
  Serial.println("Searching for");
  Serial.println("GDX Sensor");
  char strBuffer[32]="unassigned";
  char strUnits[32]="strUnits";
      // 2-LINE DISPLAY CODE
      CharDisplayInit();
      CharDisplayPrintLine(1,"Searching for");
      CharDisplayPrintLine(2,"GDX sensor");
      delay (2000);
      //2-LINE DISPLAY CODE*/
  GDX.Begin();  //use this line for proximity pairing
      //or
  //GDX.Begin("GDX-ACC 0H1019K1",1, 1000);//or specify device, channel and period here 
  //GDX.Begin("GDX-RB 0K2000F4",2, 500);
  //GDX.Begin("GDX-SND 0T1001L7",2, 500);
  Serial.print("Found: ");
  Serial.println (GDX.deviceName());
  
  Serial.print("channelName; ");
  Serial.println (GDX.channelName());

  Serial.print("channelUnits: ");
  Serial.println (GDX.channelUnits());

  Serial.print("channelNumber: ");
  Serial.println (GDX.channelNumber());
  
  Serial.print("Battery status(%): ");
  Serial.println (GDX.batteryPercent());
  
  Serial.print("ChargeState: ");
  Serial.print (GDX.chargeState());
  Serial.println(" (0=idle, 1=charging, 2=charging complete, 3=error)");
  
  Serial.print("Bluetooth signal strength (RSSI): "); 
  Serial.println (GDX.RSSI());
  Serial.println ("threshold = -44 ");//!!!NOTE SET IN .CPP CODE
  
  Serial.print("samplePeriod: ");
  Serial.println (GDX.samplePeriodInMilliseconds());
  
      // 2-LINE DISPLAY CODE
      CharDisplayPrintLine(1,"Found: ");
      CharDisplayPrintLine (2,GDX.deviceName());
      delay(2000);
      
      CharDisplayPrintLine (1,GDX.channelName());
      CharDisplayPrintLine (2,GDX.channelUnits());
      delay(2000);
            
      sprintf(strBuffer,"%s %d %s %d","batt:",GDX.batteryPercent(),"CS:",GDX.chargeState());
      CharDisplayPrintLine(1, strBuffer);
      sprintf(strBuffer,"%s %d", "channelNumber",GDX.channelNumber());
      CharDisplayPrintLine(2, strBuffer);
      delay(2000);
     
      sprintf(strBuffer,"%s %d", "   RSSI:   ",GDX.RSSI());
      CharDisplayPrintLine (1,strBuffer);
      CharDisplayPrintLine (2,"threshold = -44 ");//!!!NOTE SET IN .CPP CODE
      delay(2000);
      //2-LINE DISPLAY CODE*/ 
      
  GDX.start();
  delay(200);
  for(int row=1;row<11;row++){
     Serial.print(row);
     Serial.print(" ");
     float channelReading =GDX.readSensor();
     Serial.print(channelReading);
     Serial.print(" ");
     Serial.println(GDX.channelUnits());
     // 2-LINE DISPLAY CODE
           //char strBuffer[32];
           sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
           CharDisplayPrintLine (1,GDX.channelName());
           CharDisplayPrintLine (2,GDX.channelUnits());
           CharDisplayPrintLine(2, strBuffer);
       //2-LINE DISPLAY CODE */
 
       }//end of for
     GDX.stop();
     Serial.println("series of readings taken; stopping GDX data collection");
     CharDisplayPrintLine (1,"finished with   ");
     CharDisplayPrintLine (2,"sensor readings ");
     delay(1000); 
     GDX.close(); 
     delay(1000); 
}//end of setup

void loop(){
  /*char strBuffer[32];
  Serial.println("top of loop");
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.println(GDX.channelUnits());
  
      // 2-LINE DISPLAY CODE
      sprintf(strBuffer, "%.2f %s", channelReading, GDX.channelUnits());
      CharDisplayPrintLine (1,GDX.channelName());
      CharDisplayPrintLine(2,strBuffer);
      //2-LINE DISPLAY CODE
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
          //char strBuffer[32];
          sprintf(strBuffer, "%-16.15s",  strText);
        
          //For debug -- prints what goes to the display
          //Serial.print(" strBuffer ");
          //Serial.print("[");
          //Serial.print(strBuffer);
          //Serial.println("]");
          //Serial.println(strlen(strBuffer));
          
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
        
void ConvertUTF8ToASCII(char* s)
{ 
  unsigned int k = 0;
  unsigned int len = strlen(s);
  byte c;

  for (unsigned int i = 0; i < len; i++)
  {
    c = s[i]; 
    Serial.print("in convert, len = "); 
    Serial.print(len); 
    Serial.print(" i = ");        
    Serial.print(i); 
    Serial.print(" c = ");  
    Serial.println(c);        
    if (c == 0xC2)// if the character is weird
    {
        //move some code like this to .cpp
        Serial.println("WIERD CHARACTER FOUND");
        Serial.println(GDX.channelName()[7]);
        if (GDX.channelName()[0]=='T')
        Serial.print("TEMPERATURE");
      if (GDX.channelName()[7]=='a')
        Serial.print("ACCELERATION");
      if (GDX.channelName()[0]=='C')
        Serial.print("CONDUCTIVITY"); 
      //other wierd ones are CO2 and O2       
       i++; // skip to the next character, and check it
      c = s[i];            
      if      (c == 0xB5) c = 'u';  // micro
      else if (c == 0xB0) c = 0xDF; // degrees (specific for 16x2 LCD character set)
      else if (c == 0xB2) c = '2';  // squared          
    }//END OF IF
    
    s[k] = c;
    k++;
}
 
  s[k] = 0;
}
 
