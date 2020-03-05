#include <ArduinoBLE.h>
#include "GDXLib.h"
GDXLib GDX;

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  //GDX.GoDirectBLE_Begin();//
  GDX.GoDirectBLE_Begin("GDX-TMP 0F1038J5", 1, 1000);
  delay (1000);
  GDX.autoID();// this is the routine to get device info
  Serial.println ("Data Table:");
  // Initialize the character display
 
}
 void loop()
{
  float channelReading =GDX.readSensor();
  //char strBuffer[64];
  //char units[18];

  Serial.print("channelReading = ");
  Serial.println(channelReading);
  //GDX.getChannelUnits(units, 18);//Jenny's method of getting a string
  //Serial.println(units);//"units returned Jenny's C way: ");
  Serial.print("RSSI ");
  //Serial.println(GDX.getRSSI());
  Serial.print("battery percent: ");
  Serial.println(GDX.batteryPercent());
  Serial.print("chargeState: ");
  Serial.print(GDX.chargeState());
  Serial.println(" 0 =idle, 1= charging, 2= complete, 3= error");
  Serial.print("channelName: ");
  Serial.println(GDX.channelNameX());
  Serial.print("deviceName: ");
  Serial.println(GDX.deviceName());
  Serial.print("channelUnits: ");
  Serial.println(GDX.channelUnits());
  Serial.println();
  delay(1000);
}
