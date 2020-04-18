#include "ArduinoBLE.h"
#include "GDXLib.h"

#define TWO_LINE_DISPLAY //comment out for no DISPLAY
//#define C_F_VERSION //C and F temperature

GDXLib GDX;
static char strUnits[16];
int t=0; //loop counter

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  char strBuffer[64];//I changed to 64 

  #if defined TWO_LINE_DISPLAY
    CharDisplayInit();
    delay (2000);
  #endif //TWO_LINE_DISPLAY
  
  #if defined C_F_VERSION
    Serial.print  ("special version  ");
    Serial.println("C and F temp only");
    CharDisplayPrintLine(1, "special version ");
    CharDisplayPrintLine(2, "C & F temp only ");
    delay (2000);
  #endif //C_F_VERSION
  
  Serial.println(" Looking for"); 
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, "Looking for ");
  #endif //TWO_LINE_DISPLAY
  
  char sensorName[64]="             ";
  // char sensorName[64]="GDX-ST 0P1000S9";
  // char sensorName[64]="GDX-FOR 072001P5"
  //char sensorName[64]="GDX-ACC 0H101767";
       
  if (sensorName[5] == ' ') //if no specific sensor seleted
  {
    Serial.println("  any GDX sensor ");
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(2, "any GDX sensor");
      delay(2000);
    #endif //TWO_LINE_DISPLAY
    GDX.GoDirectBLE_Begin();
  }
  else
  {
    Serial.println(sensorName);
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(2, sensorName);
      delay(2000);
    #endif //TWO_LINE_DISPLAY
    GDX.GoDirectBLE_Begin(sensorName,1, 1000);//specify channel and period here also
  }
  
  GDX.autoID();// this is the routine to get device info
  Serial.print("Found: ");
  Serial.println(GDX.deviceName());
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, "Found ");
    CharDisplayPrintLine(2, GDX.deviceName());
    delay(1000);
  #endif //TWO_LINE_DISPLAY

  Serial.print("ChannelName: ");
  Serial.println(GDX.channelName());
  Serial.print("ChannelUnits: ");
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  sprintf(strUnits, "%s", GDX.channelUnits());
  ConvertUTF8ToASCII(strUnits);
  Serial.print("strUnits ");
  Serial.println(strUnits);
 
  Serial.println ("Data Table:");
}
 void loop()
{
  float channelReading =GDX.readSensor();
  char strBuffer[64];
  sprintf(strBuffer, "--- %s", strUnits);//?!!!!
  sprintf(strBuffer, "%.2f %s", channelReading,strUnits);
  //sprintf(strBuffer, "%.2f %s", channelReading,strUnits);//crash??!!!!!

  #if defined C_F_VERSION
      t++;
     float t2=t/2.0;//used to determine every other time through the loop
     if (t2==int(t/2))// every other time switch to F temperature
        {
          channelReading= channelReading*1.8+32;//convert C to F degrees  HACK
          //sprintf(strBuffer, "%.2f %s", channelReading,"deg F");// causes crash
          sprintf(strBuffer, "%.2f %s", channelReading,"deg F");   
        }
  #endif //C_F_VERSION
  Serial.println(GDX.channelName());
  Serial.println(strBuffer);
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, GDX.channelName());
    CharDisplayPrintLine(2,strBuffer);
  #endif // TWO_LINE_DISPLAY
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
