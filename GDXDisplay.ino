#include <ArduinoBLE.h>
static char strUnits[16];
#include "GDXLib.h"
GDXLib GDX;
void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  delay(10000);//HACK
  GDX.GoDirectBLE_Begin();//
  //GDX.GoDirectBLE_Begin("GDX-TMP 0F1038J5", 1, 1000);
  Serial.print("RSSI ");
  Serial.println(GDX.getNumber());
  delay(2000);
 Serial.println ("Data Table:");
}
 void loop()
{
  float channelReading =GDX.readSensor();//
  Serial.print("channelReading = ");
  Serial.println(channelReading);
  delay(1000);
}
