#include "ArduinoBLE.h"
#include "GDXLib.h"

GDXLib GDX;

void setup()
{
  //this version is for proximity pairing
  Serial.begin(9600);
  delay(500);

  //GDX.GoDirectBLE_Begin();//find nearest
  GDX.GoDirectBLE_Begin("GDX-FOR 072001P5",2, 500);//or specify device, channel and period here 
  delay (100);
  GDX.autoID();// this is the routine to get device info
  Serial.print("Found: ");
  Serial.println(GDX.deviceName());
  delay (100);
  Serial.println("SETUP DONE");
}
 void loop()
{
  //Serial.println("TOP OF LOOP");
  float channelReading =GDX.readSensor();
  Serial.print("                   channelReading: ");
  Serial.println(channelReading);
  delay (500);
  
}
