/*
 GDXLib Demo (v. 20201112, using the 0.88 )
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 This program controls three LED, sort of like a traffic light, based on 
 sound level. It supports and I2C 2-line display.
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
char strBuffer[32]="unassigned";
char strUnits[32]="strUnits";
   //indented lines are for 2-line display
   // this version uses I2C connection
   #include <Wire.h>
   #include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
   SerLCD lcd; // Initialize the library with default I2C address 0x72

void setup(){
  Serial.begin(9600);
  delay(500);
  pinMode(2, OUTPUT); // output 
  pinMode(3, OUTPUT); // output 
  pinMode(4, OUTPUT); // output 
  pinMode(5, OUTPUT); // output 
      // 2-LINE DISPLAY CODE
      Wire.begin();
      lcd.begin(Wire); //Set up the LCD for I2C communication
      lcd.setBacklight(255, 255, 255); //Set backlight to bright white
      lcd.setContrast(5); //Set contrast. Lower to 0 for higher contrast.
      lcd.clear(); //Clear the display - this moves the cursor to home position as well
      lcd.print("Searching for");
      lcd.setCursor(0,1);// column, row
      lcd.print("GDX Sensor");
      delay(1000);
      //2-LINE DISPLAY CODE*/
  GDX.open("GDX*SND XXXXXXXX",1, 1000);//or specify device, channel and period here 

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
      lcd.clear();
      lcd.print("Found: ");
      lcd.setCursor(0,1);// column, row
      lcd.print(GDX.deviceName());
      delay(2000);
      
      lcd.clear();
      ConvertUTF8ToASCII(GDX.channelName());//this is to handle special characters on LCD display
      lcd.print(GDX.channelName());
      lcd.setCursor(0,1);// column, row
      ConvertUTF8ToASCII(GDX.channelUnits());//this is to handle special characters on LCD display
      lcd.print(GDX.channelUnits());
      delay(2000);
            
      lcd.clear();
      sprintf(strBuffer,"%s %d %s %d","batt:",GDX.batteryPercent(),"CS:",GDX.chargeState());
      lcd.print(strBuffer);
      sprintf(strBuffer,"%s %d", "channelNumber",GDX.channelNumber());
      lcd.setCursor(0,1);// column, row
      lcd.print(strBuffer);
      delay(2000);
     
      lcd.clear();
      sprintf(strBuffer,"%s %d", "   RSSI:   ",GDX.RSSI());
      lcd.print(strBuffer);
      lcd.setCursor(0,1);// column, row
      lcd.print("threshold = -44 ");//!!!NOTE SET IN .CPP CODE
      delay(2000);
      //2-LINE DISPLAY CODE*/ 
  GDX.start();
}//end of setup

void loop(){
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.println(GDX.channelUnits());
  if (channelReading<70) digitalWrite (2,HIGH);
  else digitalWrite(2, LOW);
  if (channelReading>70) digitalWrite (3,HIGH);
    else digitalWrite(3, LOW);
  if (channelReading>90) digitalWrite (4,HIGH);
    else digitalWrite(4, LOW);
  digitalWrite(5, LOW);

       // 2-LINE DISPLAY CODE
       lcd.clear(); //Clear 
       sprintf(strBuffer, "%.2f", channelReading);
       lcd.print(strBuffer);
       lcd.setCursor(0,1);// column, row
       lcd.print(GDX.channelUnits());
       //2-LINE DISPLAY CODE */
}

void ConvertUTF8ToASCII(char* s)//this is to handle special characters on LCD display
{
  if (s[0] =='O'||s[1] ==226) {
    strcpy(s,"O2 Gas");//exception
    Serial.println("O2 Gas exception");
  } 
  if (s[1] =='C'||s[2] ==226) {
    strcpy(s,"CO2 Gas");//exception
    Serial.println("CO2 Gas exception");
  }
  unsigned int k = 0;
  unsigned int len = strlen(s);
  byte c;
  for (unsigned int i = 0; i < len; i++)
  {
    c = s[i];
    if (c == 0xC2)//Latin capital letter A with circumflex, decimal 194
    { 
      i++; // skip to the next character
      c = s[i];
      if      (c == 0xB5) c = 'u';  // micro
      else if (c == 0xB0) c = 0xDF; // degrees (specific for 16x2 LCD character set)
      else if (c == 0xB2) c = '2';  // squared (superscript 2)
      else if (c == 0xB3) c = '3';  // cubed (superscript 3)
    }
    s[k] = c;
    k++;
  }
  s[k] = 0;
}