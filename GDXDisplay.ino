//041520207a
// I am making a new branch called Master2

#include "ArduinoBLE.h"
#include "GDXLib.h"
//#define TWO_LINE_DISPLAY //ADD FOR DISPLAY
//#define C_F_VERSION //C and F temperature 
//#define CURIE_VERSION //to support Arduino 101, instead of Arduino BLE, also search for ###
#define STATUS //to show battery status, RSSI, and other info
GDXLib GDX;
static char strUnits[32];
static char D1[16]="             ";//char strings used to print on display
static char D2[16]="             ";
int t=0; //loop counter

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  delay(1000);//this may be important!
  char strBuffer[64];

  #if defined TWO_LINE_DISPLAY
    CharDisplayInit();
    delay (200);
  #endif //TWO_LINE_DISPLAY
  
  #if defined C_F_VERSION
    strcpy(D1,"special version");
    strcpy(D2,"C & F temp only");
    CharDisplayPrintLine(1,D1); 
    CharDisplayPrintLine(2,D2); 
  #endif //C_F_VERSION

  strcpy(D1," Looking for ");
  strcpy(D2," GDX Sensor");
  CharDisplayPrintLine(1,D1); 
  GDX.GoDirectBLE_Begin();//
  //GDX.GoDirectBLE_Begin("GDX-ST 0P1000S9", 1, 500);
  //GDX.GoDirectBLE_Begin("GDX-FOR 072001P5", 1, 500);//note less than loop time
  //GDX.GoDirectBLE_Begin("GDX-MD 0B1027S0", 5, 1000);
  delay (1000);
 
  GDX.autoID();// this is the routine to get device info
  delay (1000);
  
  strcpy(D1,"Found: ");
  strncpy(D2, GDX.deviceName(),15);
  
  CharDisplayPrintLine(1,D1); 
  CharDisplayPrintLine(2,D2); 
  
  strncpy(D1,"ChannelName: ",15);
  strncpy(D2, GDX.channelName(),15);
  CharDisplayPrintLine(1,D1); 
  CharDisplayPrintLine(2,D2); 

  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  Serial.print("@@@ GDX.channelUnits()");
  Serial.println(GDX.channelUnits());
  strncpy(strUnits, GDX.channelUnits(),15);
  //Serial.print("strUnits before");
  //Serial.println(strUnits);
  //ConvertUTF8ToASCII(strUnits);
  Serial.print("@@@strUnits after");
  Serial.println(strUnits);
  
  strcpy(D1,"ChannelUnits: ");
  strncpy(D2, strUnits,15);
  CharDisplayPrintLine(1,D1);
  CharDisplayPrintLine(2,D2);  
  
  #if defined STATUS
      strcpy(D1,"RSSI: ");
      sprintf(D2, "%.d %s", GDX.RSSI()-256,"percent");
      CharDisplayPrintLine(1,D1);
      CharDisplayPrintLine(2,D2);    
      strcpy(D1,"battery: ");
      sprintf(D2, "%.d %s",GDX.batteryPercent(),"percent");
      CharDisplayPrintLine(1,D1);
      CharDisplayPrintLine(2,D2); 
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
      strcpy(D1,"charge status: ");
      strcpy(D2, strBuffer);
      CharDisplayPrintLine(1,D1);
      CharDisplayPrintLine(2,D2); 
   #endif STATUS
}

 void loop()
{
  float channelReading =GDX.readSensor();
  Serial.print("@@@ D1 at start of loop: ");
  Serial.println(D1);//it comes in with left over string
 
  strncpy(D1,GDX.channelName(),16);
  Serial.print("@@@D1 after strcpy: ");
  Serial.println(D1);
  Serial.print("sizeof(D1) after strcpy: ");
  Serial.println(sizeof(D1));
 
  sprintf(D2, "%.2f %s", channelReading,strUnits);// using strUnits or GDX.channelUnits() seems to be a problem
  //sprintf(D2, "%.2f %s", channelReading,strUnits);// 
  //sprintf(D2, "%.2f", channelReading);// using strUnits seems to be a problem
      //strcpy(D1,"RSSI: ");
      //sprintf(D2, "%.d %s", GDX.RSSI()-256,"percent");
      //CharDisplayPrintLine(1,D1);   
      //strcpy(D1,"battery: ");
      //sprintf(D2, "%.d %s",GDX.batteryPercent(),"percent"); //!!!!!even this causes crashes
      //sprintf(D2, "%.d",GDX.batteryPercent()); //!!!!!even this causes crashes with no display
      //sprintf(D2, "%.2f",channelReading); //!!!!!even this causes crashes with no display
  
  #if defined C_F_VERSION
      t++;
     float t2=t/2.0;//used to determine every other time through the loopif (t2==int(t/2))// every other time switch to F temperature
        {
          channelReading= channelReading*1.8+32;//convert C to F degrees  HACK
          sprintf(D2, "%.2f %s", channelReading,"Deg F");// change number and units to F
        }
  #endif //C_F_VERSION
  Serial.print("sizeof(D1) ");
  Serial.println(sizeof(D1));
  Serial.print("sizeof(D2) ");
  Serial.println(sizeof(D2));
  CharDisplayPrintLine(1,D1);
  CharDisplayPrintLine(2,D2);
  delay(1000);//right now this would cause it to read half as often with this DEBUG on
}

void CharDisplayPrintLine(int line, const char* strText)
{
   Serial.println(strText);
   Serial.print("sizeof(strText): ");
   Serial.println(sizeof(strText));
   #if defined TWO_LINE_DISPLAY
     uint8_t lineCode = 128;
     if (line == 2) lineCode = 192;
     // Force a field width of 16.
     // Left justified with space padding on the right.
     char strBuffer[32];
     sprintf(strBuffer, "%-16.16s",  strText);
     Serial1.write((uint8_t)254); 
     Serial1.write(lineCode);  
     Serial1.write(strBuffer, 16);
     delay(1000);
   #endif //TWO_LINE_DISPLAY
}

void CharDisplayInit()
{
  Serial1.begin(9600);
  delay(500);
  
  // Clear the screen
  Serial1.write((uint8_t)254);
  Serial1.write((uint8_t)1);

  // Move cursor to beginning of line1
  Serial1.write(254); 
  Serial1.write(128);  
}

void SegmentDisplayInit()
{
  // Initialze the display.
  // It is on the dedicated hardware serial port on the Arduino101.
  // This is the predefined Serial1 object.
  Serial1.begin(9600);
  Serial1.write(0x76); // Clear
  Serial1.write("----", 4);  
}

void SegmentDisplayPrintFloat(float number)
{
    char tempString[16];
  
    uint8_t dot = 0x00;
    signed long value;
    signed long value100 = number * 100;
    signed long value1000 = number * 1000;

    if (number < 0)
    {
      if      (value100 > -1000)  dot = 0x2;    
      else if (value100 > -10000) dot = 0x4;
      value = value100;
    }
    else
    {
      if      (value1000 < 10000)   dot = 0x01;
      else if (value1000 < 100000)  dot = 0x02;
      else if (value1000 < 1000000) dot = 0x04;
      value = value1000;
    }
      
    sprintf(tempString, "%04ld", value); 
    tempString[4] = 0;
    
    Serial1.write((uint8_t)0x79); // Ensure cursor position at first digit
    Serial1.write((uint8_t)0x00);    
    Serial1.write(tempString, 4); 
    Serial1.write((uint8_t)0x77); // Set decimal place
    Serial1.write(dot);    
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
