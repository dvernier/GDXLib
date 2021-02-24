/*
 GDXLib Demo (v. 210211, using the 0.9.2 )
 This is a simple demo program for using GDX sensors on any Arduino 
 which supports the Arduino BLE library. This includes the Nano33 BLE,
 Arduino Nano33 Sense, and MKR WiFi 1010, and Arduino Uno WiFi Rev2.
 The lines with the major indents are for using a 2-line display connected
 with four I2C lines. 
 */
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;
char strBuffer[32]="unassigned";
int loopCounter=0; // used to control the time the loop runs
int loopsToGoThrough=100; // change to set how long the program runs
   
void setup(){
  Serial.begin(9600);
  delay(500);
  int loopCounter=0; // used to control the time the loop runs
  pinMode(2, OUTPUT); // output 
  pinMode(3, OUTPUT); // output 
  pinMode(4, OUTPUT); // output 
  pinMode(5, OUTPUT); // output 
  
  GDX.open();  //use this line for proximity pairing
      //or
  //GDX.open("GDX-FOR 072001D0",1, 500);//or specify specific device, channel and period here 
     //or
  //GDX.open("GDX*ACC XXXXXXXX",1, 1000);//or specify device type, channel and period here 

 Serial.print("Found: ");
  Serial.print (GDX.orderCode());
  Serial.print (" ");
  Serial.println (GDX.serialNumber());
  
  Serial.print("channelName: ");
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
  
  GDX.start();
}//end of setup

void loop(){
  loopCounter++; //increment
  Serial.print(loopCounter);
  Serial.print("  ");
  float channelReading =GDX.readSensor();
  Serial.print("channelReading: ");
  Serial.print(channelReading);
  Serial.println(GDX.channelUnits());
  if (channelReading>2) digitalWrite (2,HIGH);
  else digitalWrite(2, LOW);
  if (channelReading>3) digitalWrite (3,HIGH);
    else digitalWrite(3, LOW);
  if (channelReading>4) digitalWrite (4,HIGH);
    else digitalWrite(4, LOW);
  if (channelReading>5) digitalWrite (5,HIGH);
    else digitalWrite(5, LOW);

 if (loopCounter>loopsToGoThrough){
    Serial.print("Stopping program, and closing Bluetooth connection.");
    GDX.stop();
    GDX.close();
   }
 }
