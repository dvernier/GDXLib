//0403 10a
#include <ArduinoBLE.h>
#include "GDXLib.h"
#define DEBUG1//ADD FOR DISPLAY
GDXLib GDX;

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  char strBuffer[64];
  char units[18];
  #if defined DEBUG1
    CharDisplayInit();
    delay (200);
    CharDisplayPrintLine(1, "Looking for ");
    CharDisplayPrintLine(2, "GDX Sensor");
  #endif //DEBUG1
  
  //GDX.GoDirectBLE_Begin();//
  //GDX.GoDirectBLE_Begin("GDX-TMP 0F1038J5", 1, 1000);
  GDX.GoDirectBLE_Begin("GDX-MD 0B1027S0", 5, 1000);
  
  delay (1000);
  GDX.autoID();// this is the routine to get device info
  #if defined DEBUG1
    CharDisplayPrintLine(1, "Found ");
    CharDisplayPrintLine(2, GDX.deviceName());
    delay(2000);
  //GDX.getChannelUnits(units, 18);//Jenny's method of getting a string
  //Serial.println(units);//"units returned Jenny's C way: ");
  Serial.print("RSSI ");
  //Serial.println(GDX.getRSSI());//why does this fail????????????
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
  strcpy(units,GDX.channelUnits());
  Serial.print(units);
  Serial.println(atoi(units));
  if (atoi(units)<1)
    strcpy(units,"degrees");
  /*CharDisplayPrintLine(1, "float");
  float x=1234.56;
  char y[10]="hello";
  Serial.println(x);
  Serial.println(y);
  //sprintf(strBuffer, "%.2f", x);//this worked to display a float
  sprintf(strBuffer, "%.2f %s", x,y);//this worked to display a float and string
  Serial.println(strBuffer);
  CharDisplayPrintLine(2,strBuffer);
  delay(2000);
  CharDisplayPrintLine(1, "integer");
  int xx=1234;
  Serial.println(xx);
  //sprintf(strBuffer, "%.ld", xx);//this worked to display an int
  sprintf(strBuffer, "%.ld %s", xx,y);//this worked to display an int and a string
  Serial.println(strBuffer);
  CharDisplayPrintLine(2,strBuffer);
  delay(1000);
  */
  #endif //DEBUG1
  Serial.println ("Data Table:");
}
 void loop()
{
  float channelReading =GDX.readSensor();
  char strBuffer[64];
  char units[18];
  Serial.print("channelReading = ");
  Serial.println(channelReading);
  #if defined DEBUG1
    CharDisplayPrintLine(1, GDX.channelNameX());
    sprintf(strBuffer, "%.2f %s", channelReading,units);//this worked to display a float and string
    Serial.println(strBuffer);
    CharDisplayPrintLine(2,strBuffer);
    delay(2000);
  #endif //DEBUG1
  delay(1000);
  //GDX.GoDirectBLE_End();//sometime!!!!!!!!
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
