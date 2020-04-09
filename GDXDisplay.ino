//04092020 8a
#include "ArduinoBLE.h"
#include "GDXLib.h"
#define TWO-LINE DISPLAY//ADD FOR DISPLAY
//#define C&F VERSION//C and F temperature
//#define CURIE VERSION//to support Arduino 101, instead of Arduino BLE, also search for ###
#define DISPLAY STATUS//to display battery status, RSSI, and other info
GDXLib GDX;

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  char strBuffer[65];//I changed to 65 in despiration
  char units[18];
  #if defined TWO-LINE DISPLAY
    CharDisplayInit();
    delay (200);
  #endif //TWO-LINE DISPLAY
  #if defined C&F VERSION
    Serial.print  ("special version  ");
    Serial.println("C and F temp only");
    CharDisplayPrintLine(1, "special version ");
    CharDisplayPrintLine(2, "C & F temp only ");
    delay (3000);
  #endif //C&F VERSION
  #if defined TWO-LINE DISPLAY
    CharDisplayPrintLine(1, "Looking for ");
    CharDisplayPrintLine(2, "GDX Sensor");
    #endif //TWO-LINE DISPLAY
  //GDX.GoDirectBLE_Begin();//
  GDX.GoDirectBLE_Begin("GDX-ST 0P1000S9", 1, 1000);
  //GDX.GoDirectBLE_Begin("GDX-MD 0B1027S0", 5, 1000);
  delay (1000);
  GDX.autoID();// this is the routine to get device info
  Serial.print("Found: ");
  Serial.println(GDX.deviceName());
  #if defined TWO-LINE DISPLAY
    CharDisplayPrintLine(1, "Found ");
    CharDisplayPrintLine(2, GDX.deviceName());
    delay(2000);
  #endif //C&F VERSION
  Serial.print("RSSI ");
  Serial.println(GDX.RSSI());
  Serial.print("battery percent: ");
  Serial.println(GDX.batteryPercent());
  Serial.print("chargeState: ");
  Serial.print(GDX.chargeState());
  Serial.println(" 0 =idle, 1= charging, 2= complete, 3= error");
  Serial.print("ChannelName: ");
  Serial.println(GDX.channelNameX());
  Serial.print("deviceName: ");
  Serial.println(GDX.deviceName());
  Serial.print("channelUnits: ");
  strcpy(units,GDX.channelUnits());
  Serial.print(units);
  Serial.println(atoi(units));
  if (atoi(units)<1)
    strcpy(units,"degrees");
  #if defined DISPLAY STATUS
    CharDisplayPrintLine(1, "battery percent:");
    sprintf(strBuffer, "%.d", GDX.batteryPercent());
    CharDisplayPrintLine(2, strBuffer);
    delay(1000);
    #define D2PIO_CHARGER_STATE_IDLE     0 
    switch(GDX.chargeState()
    {
      case 0:
        strBuffer="not connected"
        break;
      case 1:
        strBuffer="charging"
        break;
      case 2:
        strBuffer="complete"
        break;
      case 3:
        strBuffer="error"
        break;
      }
    CharDisplayPrintLine(1, "chargeState: ");
    CharDisplayPrintLine(2, strBuffer);
    delay(2000);
    CharDisplayPrintLine(1, "RSSI ");
    sprintf(strBuffer, "%.d",(GDX.RSSI()));
    CharDisplayPrintLine(2, strBuffer);
    delay(2000);
    CharDisplayPrintLine(1, "ChannelName: ");
    CharDisplayPrintLine(2, GDX.channelNameX());
   #endif //DISPLAY STATUS
  Serial.println ("Data Table:");
}
 void loop()
{
  float channelReading =GDX.readSensor();
  char strBuffer[64];
  char units[18];
  Serial.print("channelReading = ");
  Serial.println(channelReading);
  #if defined TWO-LINE DISPLAY
    CharDisplayPrintLine(1, GDX.channelNameX());
    sprintf(strBuffer, "%.2f %s", channelReading,units);//this worked to display a float and string
    #if defined C&F VERSION
      strcpy(units,"degrees C");////hack !!!!!!!
      sprintf(strBuffer, "%.2f %s", channelReading,units);//this worked to display a float and string
    #endif //C&F VERSION
    Serial.println(strBuffer);
    CharDisplayPrintLine(2,strBuffer);
    delay(2000);
    #if defined C&F VERSION
      float tempF= channelReading*1.8+32;//convert C to F degrees  HACK
      strcpy(units,"degrees F");////hack !!!!!!!
      sprintf(strBuffer, "%.2f %s", tempF,units);//this worked to display a float and string
      Serial.println(strBuffer);
      CharDisplayPrintLine(2,strBuffer);
      delay(2000);
    #endif //C&F VERSION
  #endif //TWO-LINE DISPLAY
  
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
