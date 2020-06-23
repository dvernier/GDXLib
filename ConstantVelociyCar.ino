/*This is a simple demo program for using GDX sensors with the the Arduino Uno WiFi Rev 2.
The lines with the major indents are for using a 2-line display connected via
 I2C. Get rid of them if you do not have a display set up that way.

   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)

timeLimit
distanceLimit
MotorSpeedStart
deltaMotorSpeed)
*/
#define WITHCART
//#define TESTING
#if defined WITHCART
   #include "GDXLib.h"
   GDXLib GDX;
#endif //
#include "ArduinoBLE.h"
   #include <Wire.h>
   #define DISPLAY_ADDRESS1 0x72 //This is the default address of the OpenLCD

//this version assumes the motor is on A on the Motor Shield A connector
int PWMPin =3; // this sets the right pin for PWM
int Brake =9; // this sets the brake status
int Direction =12; // this sets the motor direction
int Mode=1;//default red
int MotorSpeed=130;// Motor speed varies from 0 to 255
float speed;
int MotorTime; //time in seconds
float MotorCurrent;
int Step=0;
const int red = 2; //LEDs to indicate mode
const int Green = 6;
const int Blue = 4;
const int buzzer = 5;//buzzer
const int ShieldLED=13; //LED on shield 
float distanceLimit=-1;//default to 1 meter
float startingPosition=0;
float t =0;
float d =0;
float timeLimit=40;//default 40s
float deltaTime=0.2;
int  steps=0;
int stepsToSecond=0;
bool endOfRun=0;//set this to 1 when the run is over
   
void setup(){
  //all very indented lines here are used for TWO_LINE_DISPLAY via I2C
      Wire.begin(); //Join the bus as master
      delay(200);
  Serial.begin(9600);
  Serial.println("ConstantVelocityCar");
      //By default .begin() will set I2C SCL to Standard Speed mode of 100kHz
      //Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz
      //Send the reset command to the display - this forces   66the cursor to return to the beginning of the display
      Wire.beginTransmission(DISPLAY_ADDRESS1);
      Wire.write('|'); //Put LCD into setting mode
      Wire.write('-'); //Send clear display command
      Wire.endTransmission();
      /*delay (2000);
      Wire.beginTransmission(DISPLAY_ADDRESS1);
      Wire.write(254); // command character
      Wire.write(128); // move to line 1, position 0,
      Wire.print("ready to start  "); //Send to the display
      Wire.print(t); //Send to the display  
      Wire.write(254); // command character
      Wire.write(192); // move to line 2
      Wire.print(" now"); //Send to the display
      Wire.endTransmission();
      delay (2000);
      */
  delay(200);

  //buzzer and LED test:
  digitalWrite(buzzer, HIGH);//buzzer on
  digitalWrite(red, HIGH);  //LED on
  delay (200);
  digitalWrite(buzzer, LOW);//buzzer off,
  digitalWrite(red, LOW); //LED off
      
  analogWrite(PWMPin,0); 
  digitalWrite(Brake,1); //brake on
  #if defined WITHCART
    Serial.println("Searching for GDX sensor...");
    delay(500);
    GDX.GoDirectBLE_Begin("GDX-CART-G 0V100001",1, 200);//or specify device, channel and period here 
    delay(1000);
    GDX.autoID();  
    delay(500);
    Serial.print("Found: ");
    Serial.println(GDX.deviceName());
    Serial.println(GDX.channelName());
    Serial.println(GDX.channelUnits());
  #endif //WITHCART
  pinMode(ShieldLED, OUTPUT); // timing LED
  pinMode(5, OUTPUT); // buzzer 
  pinMode(red, OUTPUT); // Set LED lines for output
  //pinMode(Green, OUTPUT); // Set LED lines for output
  pinMode(Blue, OUTPUT); 
  
  pinMode(buzzer, INPUT); // sound on/off switch
  analogWrite(PWMPin, 0); //motor off

  i2cSendValue(Blue);  
  delay (1000);
  i2cSendValue(Brake);    

  //if(analogRead(A5)<.1)//choose the mode with a resistor LATER
     //void MoveTo (10, float distanceLimit, 110, 0);// move back
     //void MoveTo (10, float distanceLimit, 90, 10);//accellerate forward
     //void MoveTo (float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   
   MoveTo(10, 0.3, 130, 0,0);// move forward
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart forward at MotorSpeed from the start for for distanceLimit meters OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */
   delay(1000);
   MoveTo(10, 0.3, 220, 0,0);// move forward
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart forward at MotorSpeed from the start for for distanceLimit meters OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */
   delay(1000);
   MoveTo(10, 0.3, 130, 1,0);// move backward
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart backward at MotorSpeed from the start for for distanceLimit meters OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */
   delay(1000);
   MoveTo(10, 0.3, 220, 1,0);// move backward
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart backward at MotorSpeed from the start for for distanceLimit meters OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */
   delay(1000);
   MoveTo(10, 0.6, 90, 0,5);// move forward with acceleration
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart backward at MotorSpeed from the star for for distanceLimit OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */
   delay(1000);
   MoveTo(10, 0.6, 90, 1,5);// move backward with acceleration
   //(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Direction,int deltaMotorSpeed)
   /*this code will drive the cart backward at MotorSpeed from the start for for distanceLimit OR timeLimit seconds.
   It also flashes an LED every second (timing may be a bit off, but consistant).
    */ 
}//end of setup

void loop(){
}   //end of loop

void MoveTo(float timeLimit, float distanceLimit, int MotorSpeedStart, bool Dir, int deltaMotorSpeed){ 
  digitalWrite( Blue,0); //brake off
  t=0;//start time at zero
  MotorSpeed=MotorSpeedStart;
  digitalWrite(Direction,Dir);//direction control
  digitalWrite(Brake,LOW);//brake control
  #if defined WITHCART
    startingPosition=GDX.readSensor();//if not read, it is 0
  #endif 
  Serial.print("        NEW RUN -");
  Serial.print(" startingPosition ");
  Serial.println(startingPosition);
  Serial.print(" deltaTime: ");  
  Serial.print(deltaTime);
  Serial.print(" timeLimit: ");  
  Serial.print(timeLimit);
  Serial.print(" distanceLimit: ");  
  Serial.print(distanceLimit);
  Serial.print(" MotorSpeedStart");
  Serial.print(MotorSpeedStart);
  Serial.print(" Dir: ");
  Serial.print(Dir);  
  Serial.print(" deltaMotorSpeed: ");
  Serial.println(deltaMotorSpeed);
    i2cSendValue(Blue);  
  do{
     Serial.println("top of do");
     if (stepsToSecond == 4){
       digitalWrite(red,HIGH);
       stepsToSecond=0;
     }//end of if
     else{
       digitalWrite(red,LOW);
     }//end of else
     float previousPosition=d;
     #if defined WITHCART
       d=startingPosition-GDX.readSensor();
       //float Position=GDX.readSensor();
     #else
       d=d=startingPosition-t*.15;
     #endif
     float speed=d-previousPosition; 
     MotorCurrent =analogRead(A1); 
     #if defined WITHCART
        analogWrite(PWMPin,MotorSpeed); //motor speed adjust
     #endif //  
     //delay(10);//we really do not need a delay, because the loop is so slow.
     steps++;
     stepsToSecond++;
     t=t+deltaTime;
     MotorSpeed= MotorSpeed+deltaMotorSpeed; //change speed in modes with acceleration
     Serial.println("call numbers");
     Serial.print("t: "); 
     Serial.print(t);       
     Serial.print(" d: ");
     Serial.print(d);
     Serial.print("  MotorSpeed: ");
     Serial.print(MotorSpeed);   
     Serial.print("  speed: ");
     Serial.print(speed); 
     Serial.print(", MotorCurrent: ");  
     Serial.println(analogRead(A1)); 
        //lines here are used for TWO_LINE_DISPLAY_on DIG2 with Arduino Uno WiFi Rev2
        Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
        Wire.write('|'); //Put LCD into setting mode
        Wire.write('-'); //Send clear display command
            i2cSendValue(Blue);  
        Wire.write(254); // command character
        Wire.write(128); // move to line 1, position 2,
        Wire.print("t:");
        Wire.write(254); // command character
        Wire.write(130); // move to line 1, position 2,
        Wire.print(t); //Send to the 
           i2cSendValue(Blue);  
        Wire.write(254); // command character
        Wire.write(136); // move to line 1, position 8
        Wire.print("d:");
        Wire.write(254); // command character
        Wire.write(138); // move to line 1, position 11,
        Wire.print(d); //Send to the display
        
        Wire.write(254); // command character
        Wire.write(192); // move to line 2, position 0
        Wire.print("v:");
        Wire.write(254); // command character
        Wire.write(194); // move to line 2, position 2
        Wire.print(speed); //Send to the display
        Wire.write(254); // command character
        Wire.write(200 ); // move to line 1, position 0,
        Wire.print("m:");
        Wire.write(254); // command character
        Wire.write(203); // move to line 2, right side
        Wire.print(MotorSpeed);
        Wire.endTransmission(); //Stop I2C transmission
  }//end of do
  while ((d<distanceLimit) && (t<timeLimit)&&d>(-1*distanceLimit));
  MotorSpeed=0;
  analogWrite(PWMPin,MotorSpeed); //motor speed adjust
  digitalWrite(Brake,HIGH);//brake control
  Serial.print("off, ");
  Serial.print("  MotorSpeed: ");
  Serial.println(MotorSpeed);
  digitalWrite(buzzer, HIGH);//buzzer on
  delay(200);
  digitalWrite(buzzer, LOW);//buzzer off
  }//end of function


 /*? 
 void i2cSendFloat(float numbers)
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1


        Wire.write(254); // command character
        Wire.write(192); // move to line 2, position 0,
        i2cSendFloat(MotorSpeed); //Send to the display
        Wire.write(254); // command character
        Wire.write(199); // move to line 2, rght side
        i2cSendFloat(speed); //Send to the display

       //lines here are used for TWO_LINE_DISPLAY_on DIG2 with Arduino Uno WiFi Rev2
 
  
  //Wire.write('|'); //Put LCD into setting mode
  //Wire.write('-'); //Send clear display command
  //Wire.print(" Channel Reading ");
  Wire.print(numbers);
  //Wire.print(GDX.channelUnits());
  //sprintf(strBuffer, "%2f", number);
  //Serial.print("Number          ");
  //Serial.println(number);
  //Serial.print(" channelUnits ");
  //Serial.print(GDX.channelUnits());
  //Serial.print(" strBuffer ");
  //Serial.println(strBuffer);
  //Wire.print(strBuffer);
  Wire.endTransmission(); //Stop I2C transmission
}*/
 void i2cSendValue(int value)
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1

  Wire.write('|'); //Put LCD into setting mode
  Wire.write('-'); //Send clear display command

  Wire.print("Cycles: ");
  Wire.print(value);

  Wire.endTransmission(); //Stop I2C transmission
}
 void i2cSendFloat(float number)
{
  char strBuffer[32];
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  Wire.write('|'); //Put LCD into setting mode
  Wire.write('-'); //Send clear display command
  Wire.print(number);

  Wire.endTransmission(); //Stop I2C transmission
}
