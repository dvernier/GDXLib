#include "ArduinoBLE.h"
#include "GDXLib.h"

GDXLib GDX;

void setup()
{
  //this version using GDX-ACC
  // note the ao sample period for GDX-ACC=500ms
  Serial.begin(9600);
  delay(500);
  Serial.println("LOOKING FOR GDX-ACC 0H101767");
   //delay (1000);
  GDX.GoDirectBLE_Begin("GDX-ACC 0H101767",1, 200);//specify channel and period here also
  Serial.print("Found: ");
  //delay (1000);
  GDX.autoID();// this is the routine to get device info
  delay (200);
}
 void loop()
{
  Serial.print("top of loop: ");
  float channelReading =GDX.readSensor();

  Serial.print("channelReading: ");
  Serial.println(channelReading);
  delay (1000);
  
}
