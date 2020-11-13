/*
 GDXLib Demo (v. 20201112, using the 0.88 )
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 The lines with the major indents are for using a 2-line display connected
 with four I2C lines. 
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
#include <Servo.h> //include library functions fpr servo motor
Servo myservo; //create servo object to control a servo
int sensorPin=0; //initialize global variable for pin assignment to sensor

GDXLib GDX;
char strBuffer[32]="unassigned";
char strUnits[32]="strUnits";
   //indented lines are for 2-line display
   // this version uses I2C connection
   #include <Wire.h>
   #include <SerLCD.h> //Click here to get the library: http://librarymanager/All#SparkFun_SerLCD
   SerLCD lcd; // Initialize the library with default I2C address 0x72
   int loopCounter=0; // used to control the time the loop runs
   int loopsToGoThrough=200; // change to set how long the program runs
   
void setup(){
  Serial.begin(9600);
  delay(500);
  myservo.attach(5); //attach the servo object to Arduino pin 9 
  pinMode(9,OUTPUT); //setup the servo
  pinMode(6, OUTPUT); //setup DCU line for a servo motor 
  pinMode(7, OUTPUT); //setup DCU line for a servo motor
  pinMode(8, OUTPUT); //setup DCU line for a servo motor
  digitalWrite(6, LOW); //turn off DCU line
  digitalWrite(7, LOW); //turn off DCU line
  digitalWrite(8, LOW); //turn off DCU line
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
  //GDX.Begin();  //use this line for proximity pairing
      //or
  GDX.open("GDX*ACC XXXXXXXXX",1, 150);//specify device type, channel and period here 

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

  pinMode(2, OUTPUT); // output 
 
}//end of setup

void loop(){
  loopCounter++; //increment
  Serial.print(loopCounter);
  Serial.print("  ");
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.print(GDX.channelUnits());
  int servoSetting = map(channelReading, 10, 0, 0, 110); //scale outputy to angle measure (value between 0 and 179) 
  myservo.write(servoSetting); //set servo position according to scaled value 
  Serial.print(" ");
  Serial.println(servoSetting);
       // 2-LINE DISPLAY CODE
       lcd.clear(); //Clear 
       sprintf(strBuffer, "%.2f", channelReading);
       lcd.print(strBuffer);
       lcd.setCursor(0,1);// column, row
       lcd.print(GDX.channelUnits());
       //2-LINE DISPLAY CODE */
    if (loopCounter>loopsToGoThrough){
        Serial.print("Stopping program, and closing Bluetooth connection.");
        GDX.stop();
        GDX.close();
   }
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
