#include "ArduinoBLE.h"
#include "GDXLib.h"
#define TWO_LINE_DISPLAY_TX //comment out for no DISPLAY on Arduino Nano 33 BLE
//#define TWO_LINE_DISPLAY_I2C //comment out for no DISPLAY I2C
//#define FOUR_CHARACTER_DISPLAY_DIG1 //comment out for no DISPLAY on Digital 1 Shield connector
#define STATUS //to display battery status, RSSI, and other info THIS SEEMS TO BE THE CRASHER RIGHT NOW!
//#define C_F_VERSION //C and F temperature
//#define LEDS

#if defined FOUR_CHARACTER_DISPLAY_DIG1
  #include <SoftwareSerial.h> //library used in printing to display
  // These are the Arduino pins required to create a software serial
  //  instance. We'll actually only use the TX pin.
  const int softwareTx = 5;//note that this is now set for the DIG 1 port
  const int softwareRx = 3;
  SoftwareSerial s7s(softwareRx, softwareTx);
#endif //FOUR_CHARACTER_DISPLAY_DIG1

#if defined TWO_LINE_DISPLAY_I2C
   #include <Wire.h>
   #define DISPLAY_ADDRESS1 0x72 //This is the default address of the OpenLCD
#endif //TWO_LINE_DISPLAY_I2C

GDXLib GDX;
static char strUnits[16];
int t=0; //loop counter
int period = 1000; //time between readings in ms, default 1000ms, change it, but do not go too fast!
int channel =1;//channel to be read, default is 1

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  while (!Serial); // Wait for serial monitor to open
  char strBuffer[32];
  
  #if defined LEDS
    //  Initialize the digital output lines, if used
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(6,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(8,OUTPUT);
    pinMode(9,OUTPUT);
    for (int i = 2; i < 10; i++) // cycle through the LEDs
      {
        digitalWrite(i, HIGH);
        delay (100);
        digitalWrite(i, LOW);
        delay (100);
      }          
  #endif //LEDS
 
 #if defined C_F_VERSION
    Serial.print  ("special version  ");
    Serial.println("C and F temp only");
    
    #if defined TWO_LINE_DISPLAY_TX
      CharDisplayPrintLine(1, "special version");
      CharDisplayPrintLine(2, "C & F temp only");
      delay (2000);
    #endif //TWO_LINE_DISPLAY
  #endif //C_F_VERSION
  
  Serial.print("Looking for "); 

  #if defined TWO_LINE_DISPLAY_TX
      CharDisplayInit();
      CharDisplayPrintLine(1,"Searching for");
      CharDisplayPrintLine(2,"GDX sensor");
  #endif //TWO_LINE_DISPLAY_TX

  #if defined FOUR_CHARACTER_DISPLAY_DIG1
      s7s.begin(9600);
      // Clear the display, and then turn on all segments and decimals
      clearDisplay();  // Clears display, resets cursor
      setDecimals(0b000000);  // Turn on all decimals, colon, apos
      s7s.print("DEG.");  // Displays Deg and then Cels on display
      delay (2000);
      s7s.print("CELS"); 
      setBrightness(127);  // Medium brightness
      delay(1500);
      setBrightness(255);  // High brightness
      delay(1500);
      // Clear the display before jumping into loop
      clearDisplay();  
  #endif //FOUR_CHARACTER_DISPLAY_DIG1

  #if defined TWO_LINE_DISPLAY_I2C
      // all very indented lines here are used for TWO_LINE_DISPLAY via I2C
      Wire.begin(); //Join the bus as master
      //By default .begin() will set I2C SCL to Standard Speed mode of 100kHz
      //Wire.setClock(400000); //Optional - set I2C SCL to High Speed Mode of 400kHz
      //Send the reset command to the display - this forces the cursor to return to the beginning of the display
      Wire.beginTransmission(DISPLAY_ADDRESS1);
      Wire.write('|'); //Put LCD into setting mode
      Wire.write('-'); //Send clear display command
      delay(1000);
      Wire.write('hello'); //Send clear display command
            delay(1000);
      Wire.endTransmission();
      delay (777);
      //adjust contrast:
      Wire.write('|'); //Put LCD into setting mode
      Wire.write(24); //Send clear display command
      Wire.write(50); //Send contrast//seemed good in testing
      Wire.endTransmission();
#endif //TWO_LINE_DISPLAY_I2C
  
  //set things up in the steps below
  //char sensorName[64]="             ";//for proximity pairing  !!!            111
  //char sensorName[32]="GDX-ST 0P1000S9";
  //char sensorName[32]="GDX-TMP 0F1038J5";
  char sensorName[32]="GDX-FOR 072001D0";
  //char sensorName[32]="GDX-ACC 0H1019K1";//&&
  int period = 1000; //time between readings   
  if (period<100)
        period = 100; //do not allow faster sampling!!!
    
  if (sensorName[1] == ' ') //if no specific sensor seleted (I used 2nd character here)
  {
    Serial.println("any GDX sensor ");
    
    #if defined TWO_LINE_DISPLAY_TX
      CharDisplayPrintLine(2, "any GDX sensor");
    #endif //TWO_LINE_DISPLAY
    
    delay(2000);
    
    GDX.Begin();
  }
  else
  {
    Serial.println(sensorName);
    
    #if defined TWO_LINE_DISPLAY_TX
      CharDisplayPrintLine(2, sensorName);
      delay(2000);
    #endif //TWO_LINE_DISPLAY_TX
   
    
    GDX.Begin(sensorName,channel, period);//specify channel and period here also
  }
  delay(1000);

  Serial.println("Found: ");
  delay(1000);
  Serial.print("deviceName ");
  Serial.println(GDX.deviceName());
  
  #if defined TWO_LINE_DISPLAY_TX
    CharDisplayPrintLine(1, "Found           ");
    CharDisplayPrintLine(2, GDX.deviceName());
    delay(2000);
  #endif //TWO_LINE_DISPLAY_TX

  Serial.print("ChannelName: ");
  Serial.println(GDX.channelName());
  Serial.print("ChannelUnits: ");
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  //THIS SEEMS TO BE A PROBLEM sprintf(strUnits, "%s", GDX.channelUnits());
  //ConvertUTF8ToASCII(strUnits);
  Serial.print("strUnits ");
  Serial.println(GDX.channelUnits());
  Serial.print("batteryPercent ");
  Serial.println(GDX.batteryPercent());
  Serial.print("chargeState: ");
  switch(GDX.chargeState())
    {
      case 0:
        strcpy(strBuffer,"not connected");
        break;
      case 1:
        strcpy(strBuffer,"charging");
        break;
      case 2:
        strcpy(strBuffer,"complete");
        break;
      case 3:
        strcpy(strBuffer,"error");
        break;
    }
    Serial.println(strBuffer);
  
  #if defined STATUS //THERE USED TO BE PROBLEMS LURKING HERE IN THE STATUS
      CharDisplayPrintLine(1, "ChargeState: ");
      CharDisplayPrintLine(2, strBuffer);// left over from use in the switch above
      delay(2000);   
      
      CharDisplayPrintLine(1, "battery PC: ");
      sprintf(strBuffer, "%d", GDX.batteryPercent());
      CharDisplayPrintLine(2, strBuffer);//
      delay(2000);
      
      CharDisplayPrintLine(1, "RSSI: ");
      sprintf(strBuffer, "%d", GDX.RSSI());
      CharDisplayPrintLine(2, strBuffer);//
      delay(2000);
      
 #endif //STATUS  
  
  Serial.println ("Data Table:");
}
 void loop()
{
  t++;
  float channelReading =GDX.readSensor();
  char strBuffer[32];//!!!!!!!!!!
  sprintf(strBuffer, "%.2f %s", channelReading,strUnits);

  #if defined C_F_VERSION
     float t2=t/2.0;//used to determine every other time through the loop
     if (t2==int(t/2))// every other time switch to F temperature
        {
          channelReading= channelReading*1.8+32;//convert C to F degrees  HACK
          //sprintf(strBuffer, "%.2f %s", channelReading,"deg F");// causes crash
          sprintf(strBuffer, "%.2f %s", channelReading,"deg F");   
        }//end of if
   #endif //C_F_VERSION
  
  
  
  Serial.print(GDX.channelName());
  Serial.print(" channelReading ");
  Serial.print(": ");
  Serial.print(strBuffer);
  Serial.print(" ");
  Serial.println(channelReading);
  
  #if defined TWO_LINE_DISPLAY_TX
    CharDisplayPrintLine(1, GDX.channelName());
    CharDisplayPrintLine(2,strBuffer);
  #endif // TWO_LINE_DISPLAY


  
   #if defined FOUR_CHARACTER_DISPLAY_DIG1
      // Magical sprintf creates a string for us to send to the s7s.
      //  The %4d option creates a 4-digit integer.
      sprintf(strBuffer, "%4d", channelReading);
      // This will output the tempString to the S7S
      s7s.print(strBuffer);
      setDecimals(0b0000010);  // Sets digit 3 decimal on
      Serial.print("strBuffer");
      Serial.println(strBuffer);
  #endif // FOUR_CHARACTER_DISPLAY_DIG1
  
  #if defined TWO_LINE_DISPLAY_I2C
        i2cSendFloat(channelReading);
  #endif // TWO_LINE_DISPLAY_I2C

  #if defined LEDS
   for (int dPin = 2; dPin<10;dPin++)
      {  //turn off all LEDs
         digitalWrite(dPin, LOW);
      }    
       float maxsignal =100;//!!! CHANGE DEPENDING ON SENSOR
       
       if (channelReading >(maxsignal*1/10 ))//Check to see if above threshold 1
       {
        digitalWrite(2, HIGH);
       }
    if (channelReading >maxsignal*2/10 )//Check to see if above threshold 2
       {
        digitalWrite(3, HIGH);
       }
    if (channelReading >maxsignal*3/10 )//Check to see if above threshold 3
       {
        digitalWrite(4, HIGH);
       }
    if (channelReading >maxsignal*4/10 )//Check to see if above threshold 4
       {
        digitalWrite(5, HIGH);
       }
    if (channelReading >maxsignal*5/10  )//Check to see if above threshold 5
       {
        digitalWrite(6, HIGH);
       }
    if (channelReading >maxsignal*6/10  )//Check to see if above threshold 6
       {
        digitalWrite(7, HIGH);
       }
    if (channelReading >maxsignal*7/10  )//Check to see if above threshold 7
       {
        digitalWrite(8, HIGH);
       }
    if (channelReading >maxsignal*8/10  )//Check to see if above threshold 8
       {
        digitalWrite(9, HIGH);
       }
    if (channelReading >maxsignal*9/10  )//Check to see if above threshold 9
       {
         for (int jj = 2; jj<7;jj++) // flash LEDs for max force
           {
            digitalWrite(9, HIGH);
            delay (200);
         digitalWrite(9, LOW);
            delay (200);
           }//end of for
       } // end of 9/10

  #endif //LEDS
  delay(period);//!!!in my tests, this will adjust the loop repeats at about the same time as the sample period
  // this would need to be adjusted.
  //sensors.
}//end of loop

void CharDisplayPrintLine(int line, const char* strText)
{
  uint8_t lineCode = 128;
  if (line == 2) lineCode = 192;

  // Force a field width of 16.
  // Left justified with space padding on the right.
  char strBuffer[32];
  sprintf(strBuffer, "%-16.16s",  strText);

  // For debug -- prints what goes to the display
  //Serial.print("[");
  //Serial.print(strBuffer);
  //Serial.println("]");
  
  Serial1.write((uint8_t)254); 
  Serial1.write(lineCode);  
  Serial1.write(strBuffer, 16);
}

void CharDisplayInit()
{
  Serial1.begin(9600);
  delay(500);
  
  // Clear the screen
  //Serial1.write((uint8_t)254);
 // Serial1.write((uint8_t)1);

  // Move cursor to beginning of line1
  Serial1.write(254); 
  Serial1.write(128);  
}


//=============================================================================
// ConvertUTF8ToASCII() Function
// Very limited -- only supports a few translations
//=============================================================================
void ConvertUTF8ToASCII(char* s)
{ 
  unsigned int k = 0;
  unsigned int len = strlen(s);
  byte c;

  for (unsigned int i = 0; i < len; i++)
  {
    c = s[i];    
    if (c == 0xC2)
    {
      i++; // skip to the next character
      c = s[i];            
      if      (c == 0xB5) c = 'u';  // micro
      else if (c == 0xB0) c = 0xDF; // degrees (specific for 16x2 LCD character set)
      else if (c == 0xB2) c = '2';  // squared          
    }
    
    s[k] = c;
    k++;
  }
  s[k] = 0;
}

  #if defined TWO_LINE_DISPLAY_I2C
  void i2cSendFloat(float number)
{
  char strBuffer[32];
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  Wire.write('|'); //Put LCD into setting mode
  Wire.write('-'); //Send clear display command

  //Wire.print(t);
  //Wire.print(" ");
  Wire.print(number);
  //sprintf(strBuffer, "%2f", number);

  //Serial.print("Number          ");
  //Serial.println(number);
  //Serial.print(" strBuffer ");
  //Serial.println(strBuffer);

  Wire.endTransmission(); //Stop I2C transmission
}
  #endif // TWO_LINE_DISPLAY_I2C
