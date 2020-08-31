//GDXLib DemoSimple (v. 20200819, using the 0.85 GDXLib)
#include "ArduinoBLE.h"
#include "GDXLib.h"
GDXLib GDX;

void setup(){
  Serial.begin(9600);
  delay(500);
  GDX.Begin();  //use this line for proximity pairing
      //or
  //GDX.Begin("GDX-ACC 0H1019K1",1, 1000);//or specify device, channel and period here 

  Serial.print("Found: ");
  Serial.println (GDX.deviceName());
  Serial.println (GDX.channelName());

  GDX.start();

  for(int row=1;row<11;row++){
     Serial.print(row);
     Serial.print(" ");
     float channelReading =GDX.readSensor();
     Serial.print(channelReading);
     Serial.print(" ");
     Serial.println(GDX.channelUnits());
   }//end of for
   GDX.stop();
}//end of setup

void loop(){
}
