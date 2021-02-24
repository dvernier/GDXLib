/*
 GDX Mask Demo (v. 210211, using the 0.9.2 library code)
 This version supports a I2C 2-line display.
 Use a GDX-ACC for control.
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
#define DISPLAY //comment out for no DISPLAY 
   //indented lines are for 2-line display
   // this version uses I2C connection
   #include <Wire.h>
   #include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
   SerLCD lcd; // Initialize the library with default I2C address 0x72
char *phrase1[] = {"1234567890123456",
                   "This is display ", 
                   "fun project for ", 
                   "     Yes        ",
                   "Thank You       ", 
                   "Hello, my name  ", 
                   "How are you     ",
                   "     No         ",
                   "Keep wearing    ",
                    };

char *phrase2[] = {"1234567890123456",
                   "mask, it is a   ", 
                   "students to do  ", 
                   "  ++++++++++    ",
                   "  very much!    ", 
                   "(Your Name Here)", 
                   "doing today?    ",
                   "thank you       ",
                   "  your mask!    ",
                    };
void setup(){
  Serial.begin(9600);
  delay(500);
  Serial.println("Searching for");
  Serial.println("GDX Sensor");
  #if defined DISPLAY
  // 2-LINE DISPLAY CODE
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
  #endif //DISPLAY

  GDX.open("GDX*ACC XXXXXXXX",11, 800);//or specify device, type channel and period here 
    //or
  //GDX.open("GDX-ACC 0H101767",11, 800);//specify specific device, channel and period here

  // 2-LINE DISPLAY CODE
      lcd.clear();
      lcd.print("Searching for: ");
      lcd.setCursor(0,1);// column, row
      lcd.print("GDX-ACC SENSOR");
      delay(2000);
  
  Serial.println("Found: ");
  Serial.print("deviceName: ");
  Serial.print (GDX.orderCode());
  Serial.print (" ");
  Serial.println (GDX.serialNumber());
  
  Serial.print("channelName:");
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
  Serial.print(GDX.samplePeriodInMilliseconds());
  Serial.println(" milliseconds");
   
  GDX.start();
  
}//end of setup

void loop()
{
  char strBuffer[32];
  int i=0;
  Serial.println("buffer declared ");
  float channelReading=0;
  channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.println(channelReading);
  if (abs(channelReading)>157.5) i=1;
  else if(channelReading>112.5) i=2;
  else if(channelReading>67.5) i=3;
    else if (channelReading>22.5) i=4;
    else if (channelReading>-22.5) i=5;
    else if (channelReading>-67.5) i=6;
    else if (channelReading>-112.5) i=7;
    else  i=8;
 
  Serial.println("i ");
  Serial.println(i);
  Serial.println("phrase");
  Serial.println(phrase1[i]);
  Serial.println(phrase2[i]);
  
  #if defined DISPLAY
       // 2-LINE DISPLAY CODE
       lcd.clear(); //Clear 
       lcd.print(phrase1[i]);
       lcd.print(phrase2[i]);
       //2-LINE DISPLAY CODE */
  #endif //DISPLAY
  delay (800);
}//end of loop
