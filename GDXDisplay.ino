#include <ArduinoBLE.h>
#include "GDXLib.h"
GDXLib GDX;
#define DEBUG1//ADD FOR DISPLAY
void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);

  #if defined DEBUG1
    CharDisplayInit();
    delay (200);
    CharDisplayPrintLine(1, "Looking for ");
    CharDisplayPrintLine(2, "GDX Sensor");
    delay(2000);
  #endif //DEBUG1
 
  delay(100);//HACK DELAY
  GDX.GoDirectBLE_Begin();//
  //GDX.GoDirectBLE_Begin("GDX-TMP 0F1038J5", 1, 1000);
 
  GDX.autoID();// this is the routine to do the autoID
  delay(2000);//BIG DELAY
  Serial.println ("Data Table:");
  // Initialize the character display
 
}
 void loop()
{
  float channelReading =GDX.readSensor();//
  char strBuffer[64];
  char units[18];

  Serial.print("channelReading = ");
  Serial.println(channelReading);

  GDX.getChannelUnits(units, 18);//Jenny's method of getting a string
  Serial.print("units returned Jenny's C way: ");
  Serial.println(units);
  Serial.print("RSSI ");
  Serial.println(GDX.getRSSI());
  Serial.print("battery percent: ");
  Serial.println(GDX.getBatteryStatus());
  Serial.print("channelName: ");
  Serial.println(GDX.channelNameX());
  Serial.print("channelUnits: ");
  Serial.println(GDX.channelUnits());
  #if defined DEBUG1
    sprintf(strBuffer, "%.2f", channelReading);
    CharDisplayPrintLine(1, strBuffer);
  //  strcpy(strBuffer,channelUnits());
    CharDisplayPrintLine(2, strBuffer);
  #endif //DEBUG1
  delay(1000);
}


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

void CharDisplayPrintBarGraph(int line, byte value)
{
  // For the 16x2 display with the A00 ROM code
  // the 0xFF character is a full pixel block.
  char strBuffer[32];
  sprintf(strBuffer, "%.*s", value, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  CharDisplayPrintLine(line, strBuffer);
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
