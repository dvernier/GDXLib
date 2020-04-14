//04132020 8p

//TESTING STATUS, WORKS WITH 
//ALL DEBUGS OFF YES.
//STATUS BUT NOT DISPLAY YES
//DISPLAY BUT NOT STATUS LEADS TO CRASH AFTER A COUPLE READINGS. CRASHES
#include "ArduinoBLE.h"
#include "GDXLib.h"
//#define TWO_LINE_DISPLAY //ADD FOR DISPLAY
#define C_F_VERSION //C and F temperature 
//#define CURIE_VERSION //to support Arduino 101, instead of Arduino BLE, also search for ###
//#define STATUS //to show battery status, RSSI, and other info
GDXLib GDX;
static char strUnits[16];
static char D1[17]="                ";//char strings used to print on display
static char D2[17]="                ";
  

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  delay(1000);//this is important!
  char strBuffer[64];

  #if defined TWO_LINE_DISPLAY
    CharDisplayInit();
    delay (200);
  #endif //TWO_LINE_DISPLAY
  
  #if defined C_F_VERSION
    strncpy(D1,"special version ",16);
    strncpy(D2,"C & F temp only ",16);
    Display(D1,D2); 
  #endif //C_F_VERSION

  strncpy(D1," Looking for ",16);
  strncpy(D2," GDX Sensor",16);
  Display(D1,D2); 
  //GDX.GoDirectBLE_Begin();//
  GDX.GoDirectBLE_Begin("GDX-ST 0P1000S9", 1, 500);
  //GDX.GoDirectBLE_Begin("GDX-FOR 072001P5", 1, 500);//note less than loop time
  //GDX.GoDirectBLE_Begin("GDX-MD 0B1027S0", 5, 1000);
  delay (1000);
 
  GDX.autoID();// this is the routine to get device info
  delay (1000);
  
  strncpy(D1,"Found: ",16);
  strncpy(D2, GDX.deviceName(),16);
  Display(D1,D2); 
  
  Serial.println(GDX.deviceName());
  //ONE OF THE THREE BELOW IS CAUSING A CRASH!!!!?????
  //Serial.print("ChannelName: ");Serial.println(GDX.channelNameX());
  //Serial.print("DeviceName: ");Serial.println(GDX.deviceName());
  //Serial.print("ChannelUnits: ");Serial.println(GDX.channelUnits());
  
  //strcpy(strUnits,GDX.channelUnits());
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  //sprintf(strUnits, "%s", GDX.channelUnits());
  //ConvertUTF8ToASCII(strUnits);
  //Serial.print("strUnits ");
  //Serial.print(strUnits);
  
  #if defined STATUS
    Serial.print("battery: ");Serial.print(GDX.batteryPercent());Serial.println(" %");
    Serial.print("RSSI ");Serial.println(GDX.RSSI()-256);//this is the way we are used to seeing it.!!!HACK
    Serial.print("ChargeState: ");Serial.print(GDX.chargeState());
    Serial.println(" (0=idle, 1= charging, 2= complete, 3= error)");
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

      strcpy(strUnits,GDX.channelUnits());
      Serial.println(strUnits);
      
  #endif STATUS
      
   #if defined TWO_LINE_DISPLAY
       Serial.print("battery: ");Serial.print(GDX.batteryPercent());Serial.println(" %");
    Serial.print("RSSI ");Serial.println(GDX.RSSI()-256);//this is the way we are used to seeing it.!!!HACK
    Serial.print("ChargeState: ");Serial.print(GDX.chargeState());
    Serial.println(" (0=idle, 1= charging, 2= complete, 3= error)");
    strcpy(strUnits,GDX.channelUnits());
    Serial.println(strUnits);
    // Cache the unit string and try to remap special UTF8
    // characters to ones that we can display.
    //sprintf(strUnits, "%s", GDX.channelUnits());
    //ConvertUTF8ToASCII(strUnits);
    //Serial.print("strUnits ");
 
    //CharDisplayPrintLine(1, GDX.channelNameX());
    //sprintf(strBuffer, "--- %s", GDX.channelUnits());
    //CharDisplayPrintLine(2, strBuffer);
    CharDisplayPrintLine(1, "battery percent:");
    sprintf(strBuffer, "%.1d", GDX.batteryPercent());
    CharDisplayPrintLine(2, strBuffer);
    delay(1000);
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
      CharDisplayPrintLine(1, "chargeState: ");
      CharDisplayPrintLine(2, strBuffer);
      delay(2000);
      CharDisplayPrintLine(1, "RSSI ");
      sprintf(strBuffer, "%.1d",(GDX.RSSI()-256));
      CharDisplayPrintLine(2, strBuffer);
      delay(2000);
      CharDisplayPrintLine(1, "ChannelName: ");
      CharDisplayPrintLine(2, GDX.channelNameX());
      CharDisplayPrintLine(1, "ChannelName: ");
      CharDisplayPrintLine(2, GDX.channelNameX());
   delay(2000);
   #endif //TWO_LINE_DISPLAY   

}
 void loop()
{
  float channelReading =GDX.readSensor();
  char strBuffer[64];
  strncpy(D1,"channelReading = ",16);
  sprintf(D2, "%.2f %s", channelReading,"Deg C");
  //strncpy(D2,"put a number here channelReading",16);
  Display(D1,D2); 
  delay(1000);
  //sprintf(strBuffer, "--- %s", strUnits);
  //sprintf(strBuffer, "%.2f %s", channelReading,GDX.channelUnits());
  //Serial.println(strBuffer);
  #if defined C_F_VERSION
    float tempF= channelReading*1.8+32;//convert C to F degrees  HACK
    strncpy(D1,"channelReading = ",16);
    strncpy(D2,"Fahrenheit",16);
    Display(D1,D2);   //sprintf(strBuffer, "%.2f %s", tempF,"deg F");
    delay(1000);//right now this would cause it to read half as often with this DEBUG on
  #endif //C_F_VERSION
}

void Display(const char* displayChars1,const char* displayChars2)
{
  Serial.println(displayChars1);
  Serial.println(displayChars2);
  #if defined TWO_LINE_DISPLAY
    if (strlen(displayChars1<17))
      CharDisplayPrintLine(1,displayChars1);
    if (strlen(displayChars2<17))
      CharDisplayPrintLine(2,DdisplayChars2);
    delay(1000);
  #endif //TWO_LINE_DISPLAY
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
