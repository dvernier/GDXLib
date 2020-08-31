/*Arduino  Mask program for I2C
 A lines with the major indents are for using a 2-line display
 with an I2C connection. Leave commented out what you do not need. 
 */

#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
char phrase1[16];//16 long
char phrase2[16];
char strBuffer[32]="unassigned";
char strUnits[32]="strUnits";
   //indented lines are for 2-line display
   // this version uses I2C connection
   #include <Wire.h>
   #include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
   SerLCD lcd; // Initialize the library with default I2C address 0x72

void setup(){
  Serial.begin(9600);
  delay(1000);
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
  Serial.println("Arduino Talking Mask Program");
  Serial.println("Venier S&T");
  delay(2000);
  Serial.println("Searching for GDX sensor...");
  //GDX.Begin();  //use this line for proximity pairing
  //or
  GDX.Begin("GDX-ACC 0H1019K1",1, 500);//or specify device, channel and period here 

  delay(1000);
  Serial.print("Found: ");
  Serial.println(GDX.deviceName());
  Serial.println(GDX.channelName());
  Serial.println(GDX.channelUnits());
    // 2-LINE DISPLAY CODE
      lcd.setCursor(0,0);//top-left corner
      lcd.print("Found: ");
      lcd.setCursor(0,1);// column, row
      lcd.print(GDX.deviceName());
      delay(2000);
      
      lcd.clear();
      lcd.print(GDX.channelName());
      lcd.setCursor(0,1);// column, row
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
}

void loop(){
  char strBuffer[32];
  Serial.print("buffer declared ");
  float channelReading=0;
  channelReading =GDX.readSensor();
  Serial.println(GDX.channelUnits());
  Serial.print("channelReading: ");
  Serial.println(channelReading);
   int i=3;
    if (channelReading>4)
      i=1;
   if (channelReading<-4)
      i=2;
   if (channelReading>7)
      i=5;
   if (channelReading<-7)
      i=4;
    Serial.print("i ");
    Serial.println(i);
    switch(i){
    case 1:
      strcpy(phrase1,"   Hello, My    ");
      strcpy(phrase2,"  Name is Dave  ");
      break;
    case 2:
      strcpy(phrase1,"   Thank You    ");
      strcpy(phrase2," For Your Help! ");
      break;
     case 3:
      strcpy(phrase1,"Good Bye, keep  ");
      strcpy(phrase2,"wearing a mask! ");
      break;
     case 4:
      strcpy(phrase1,"   No           ");
      strcpy(phrase2," Thank You      ");
      break;
     case 5:
      strcpy(phrase1,"   Yes          ");
      strcpy(phrase2,"                ");
      break;
    }
 
  Serial.println(phrase1);
  Serial.println(phrase2);
       // 2-LINE DISPLAY CODE
       lcd.clear(); //Clear 
       sprintf(strBuffer, "%.2f", channelReading);
       lcd.print(phrase1);
       lcd.setCursor(0,1);// column, row
       lcd.print(phrase2);
       //2-LINE DISPLAY CODE */
  delay (1000);
  
}

      
