/*
 GDXLib Demo (v. 20201104, using the 0.88 )
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
char strBuffer[32]="unassigned";
char strUnits[32]="strUnits";

void setup(){
  Serial.begin(9600);
  delay(500);
  Serial.println("Searching for GDX Sensor");
  char strBuffer[32]="unassigned";
  char strUnits[32]="strUnits";
  //GDX.open();  //use this line for proximity pairing, or use a statement like one of the two below:
  //GDX.open("GDX*ST XXXXXXXX",1, 1000);//specify a type of GDX device, channel and period here 
  GDX.open("GDX-ACC 0H1019K1",1, 1000);//or specify a specific GDX device, channel and period here 
  
  Serial.print("Found: ");
  Serial.println (GDX.deviceName());
  
  Serial.print("channelName; ");
  Serial.println (GDX.channelName());

  Serial.print("channelUnits: ");
  Serial.println (GDX.channelUnits());

  Serial.print("channelNumber: ");
  Serial.println (GDX.channelNumber());

  Serial.print("samplePeriodInMilliseconds: ");
  Serial.println (GDX.samplePeriodInMilliseconds());
  
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
   
  GDX.start();
  delay(200);
  for(int row=1;row<11;row++){ //take 10 readings and then stop. 
     Serial.print(row);
     Serial.print(" ");
     float channelReading =GDX.readSensor();
     Serial.print(channelReading);
     Serial.print(" ");
     Serial.println(GDX.channelUnits());
   }//end of for
       
     GDX.stop();
     Serial.println("series of readings taken; stopping GDX data collection");
     GDX.close(); 
     
}//end of setup

void loop(){
  //add the code below if you want repeated readings:
  /*
  Serial.println("top of loop");
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.println(GDX.channelUnits());
  */
}
