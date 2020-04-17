#include "ArduinoBLE.h"
#include "GDXLib.h"

GDXLib GDX;
//static char strUnits[16];

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
 // char strBuffer[32];//I changed to 64 in despiration
  
  /* Initialize the digital output lines
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  */
  for (int i = 2; i < 5; i++) // cycle through the LEDs
      {
        digitalWrite(i, HIGH);
        delay (200);
        digitalWrite(i, LOW);
        delay (200);
      }          
  
  Serial.println(" Looking for"); 
  
  //choose one line of the type below:
  //char sensorName[64]="             ";//for detecting the nearest GDX sensor
  // char sensorName[64]="GDX-ST 0P1000S9";
  char sensorName[64]="GDX-FOR 072001P5";
  //char sensorName[64]="GDX-ACC 0H101767";
       
  
  {
    Serial.println(sensorName);
    GDX.GoDirectBLE_Begin("GDX-FOR 072001P5",1, 1000);//specify channel and period here also
  }
  


  /*
  Serial.println(GDX.channelName());
  Serial.print("ChannelUnits: ");


  Serial.print("battery: ");
  Serial.print(GDX.batteryPercent());
  Serial.println(" %");
  
  Serial.print ("chargeStatus: ");
  Serial.println(" 0 =idle, 1= charging, 2= complete, 3= error");
  Serial.println (GDX.chargeState());
  Serial.println ("Data Table:");
  */
}
 void loop()
{

  float channelReading =GDX.readSensor();
  //char strBuffer[64];
  //sprintf(strBuffer, "%.2f %s", channelReading,strUnits);
  //Serial.println(GDX.channelName());
  Serial.println(channelReading);
  //float maxsignal =50;
   /*for (int dPin = 2; dPin<5;dPin++)
      {  //turn off all LEDs
         digitalWrite(dPin, LOW);
      }
         
    if (channelReading >maxsignal*1/20 )//Check to see if above threshold 2
       {
        digitalWrite(2, HIGH);
       }
    if (channelReading >maxsignal*2/20 )//Check to see if above threshold 4
       {
        digitalWrite(3, HIGH);
       }
    if (channelReading >maxsignal*3/20 )//Check to see if above threshold 6
       {
        digitalWrite(4, HIGH);
       }
    if (channelReading >maxsignal*4/20 )//Check to see if above threshold 8
       {
        digitalWrite(5, HIGH);
       }
    if (channelReading >maxsignal*5/20  )//Check to see if above threshold 10
       {
        digitalWrite(6, HIGH);
       }
    if (channelReading >maxsignal*6/20  )//Check to see if above threshold 12
       {
        digitalWrite(7, HIGH);
       }
    if (channelReading >maxsignal*7/20  )//Check to see if above threshold 12
       {
        digitalWrite(8, HIGH);
       }
    if (channelReading >maxsignal*8/20  )//Check to see if above threshold 12
       {
        digitalWrite(9, HIGH);
       }
    if (channelReading >maxsignal*9/20  )//Check to see if above threshold 14
       {
         for (int jj = 2; jj<7;jj++) // flash LEDs for max force
           {
            digitalWrite(9, HIGH);
            delay (200);
         digitalWrite(9, LOW);
            delay (200);
           }
          
        }
         */
  delay(100);

}
