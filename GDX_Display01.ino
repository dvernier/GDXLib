 #include <ArduinoBLE.h>
//test
//should these be global?????
static int state = -1;
static char strUnits[16];

/*from lib:
#define D2PIO_MAX_ORDERCODE_LENGTH 16
#define D2PIO_MAX_SERIALNUMBER_LENGTH 16
#define D2PIO_BLE_ADDRESS_LENGTH 6
#define D2PIO_MAX_DEVICENAME_LENGTH 32
#define D2PIO_MAX_DEVICEDESCRIPTION_LENGTH 64
#define D2PIO_MAX_NUM_BYTES_IN_SENSOR_DESCRIPTION 60
#define D2PIO_MAX_NUM_BYTES_IN_SENSOR_UNIT 32
 */
char orderCode[9];// 16 bytes used in lib
char serialNumber[9];// 16 bytes used in lib
char deviceNam[16];// 32 bytes
char channelUnits[16];// 32 bytes
char channelName[30];// 60 bytes
unsigned long availableChannels;
byte scanRSSI;
char strBuffer[64];
char strFW1[16];
char strFW2[16];
byte batteryPercent;
byte batteryLevel;
float sensorReading;
//char sensorName[]="GDX-     "; // we will use this to display later 
char sensorName[]="     ";
//leave the name blank to have the program search for nearest GDX sensor
// and connect.

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600);
  delay(100);
  CharDisplayInit(); //initilize the graphic display
  Serial.println("Vernier GDX");
  Serial.println("Sensor display");
  CharDisplayPrintLine(1, "Vernier GDX");
  CharDisplayPrintLine(2, "Sensor display");
  delay(1000);
  Serial.println(" Looking for");
  CharDisplayPrintLine(1, "Looking for");

  if (sensorName[5]==' ')//if no specific sensor seleted
      {
            Serial.print("sensorName[5] ");
            Serial.println(sensorName[5]);
            delay(5000);
      Serial.println("any GDX sensor");
      CharDisplayPrintLine(2, "any GDX sensor");
      GoDirectBLE_Begin();
      }
  else
      {
      Serial.println(sensorName);
      CharDisplayPrintLine(2, "specified");
      GoDirectBLE_Begin("GDX-ACC 0H101767", 1, 1000);
      }
  delay(2000);


  //this is from kevin's loop:
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  sprintf(strUnits, "%s", GoDirectBLE_GetChannelUnits());
  ConvertUTF8ToASCII(strUnits);
  
  Serial.print("ScanRSSI: ");
  Serial.println(GoDirectBLE_GetScanRSSI());
  CharDisplayPrintBarGraph(2, GoDirectBLE_GetScanRSSI());
  delay(2000);
  
  //these have been set up in the library code:
  Serial.print("Order Code::");
  Serial.println(orderCode);
  Serial.print("Serial Number::");
  Serial.println(serialNumber);
  Serial.print("deviceName:: ");
  Serial.println (deviceNam);
  Serial.print("channelUnits:: ");
  Serial.println (channelUnits);
  Serial.print("channelName:: ");
  Serial.println (channelName);
  Serial.print("batteryPercent:: ");
  Serial.println (batteryPercent);
  Serial.print("batteryLevel:: ");
  Serial.println (batteryLevel);
  Serial.print("availableChannels:: ");
  Serial.println (availableChannels);
  
  Serial.println("FW1:  FW2:  BAT:");
  CharDisplayPrintLine(1, "FW1:  FW2:  BAT:");
  
  GoDirectBLE_GetStatus(strFW1, strFW2, batteryLevel);
  sprintf(&strBuffer[0], "%-6s",  strFW1);
  sprintf(&strBuffer[6], "%-6s",  strFW2);
  sprintf(&strBuffer[12], "%d%%", batteryLevel);
  Serial.println(strBuffer);
  CharDisplayPrintLine(2, strBuffer);
  delay(1000);

  Serial.print("Channel Name: ");
  Serial.println(GoDirectBLE_GetChannelName());
  CharDisplayPrintLine(1, GoDirectBLE_GetChannelName());
  
  Serial.print("ChannelUnits:" );
  Serial.println(GoDirectBLE_GetChannelUnits());
  CharDisplayPrintLine(2, GoDirectBLE_GetChannelUnits());
  delay(2000);

GoDirectBLE_Measure() ;
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  sprintf(strUnits, "%s", GoDirectBLE_GetChannelUnits());
  ConvertUTF8ToASCII(strUnits);
  
  // Print a default measurement line.
  // This is nice for sensors that don't start displaying right away.
  sprintf(strBuffer, "--- %s", strUnits);
  delay(2000);
 //seems to start building the string
  //this is from kevin's loop:

      if (GoDirectBLE_DisplayChannelAsInteger())
      {
        sprintf(strBuffer, "%ld %s", (int32_t)GoDirectBLE_GetMeasurement(), strUnits);
      }
      else
      {
        sprintf(strBuffer, "%.2f %s", GoDirectBLE_GetMeasurement(), strUnits);
       
      }
      Serial.print(strBuffer);
      CharDisplayPrintLine(2, strBuffer);
      Serial.print (" ");
      Serial.print(strBuffer[0]);
      Serial.print (" ");
      sensorReading = atof(strBuffer);
      Serial.println(sensorReading);
           
      if (GoDirectBLE_DisplayChannelAsInteger())
      {
        sprintf(strBuffer, "%ld %s", (int32_t)GoDirectBLE_GetMeasurement(), strUnits);
      }
      else
      {
        sprintf(strBuffer, "%.1f %s", GoDirectBLE_GetMeasurement(), strUnits);
        //DLV hack to round temp to 1 digits to the right of the decimal. !!!DLV hack to change rounding
      }
      Serial.println(strBuffer);
      CharDisplayPrintLine(2, strBuffer);

      delay(100);
      Serial.println (" repeating...");
      if (GoDirectBLE_DisplayChannelAsInteger())
      {
        sprintf(strBuffer, "%ld %s", (int32_t)GoDirectBLE_GetMeasurement(), strUnits);
      }
      else
      {
        sprintf(strBuffer, "%.1f %s", GoDirectBLE_GetMeasurement(), strUnits);
        //DLV hack to round temp to 1 digits to the right of the decimal. !!!DLV hack to change rounding
      }
      Serial.println(strBuffer);
      CharDisplayPrintLine(2, strBuffer);
      
      delay(100);
      Serial.println (" repeating...");
      if (GoDirectBLE_DisplayChannelAsInteger())
      {
        sprintf(strBuffer, "%ld %s", (int32_t)GoDirectBLE_GetMeasurement(), strUnits);
      }
      else
      {
        sprintf(strBuffer, "%.1f %s", GoDirectBLE_GetMeasurement(), strUnits);
        //DLV hack to round temp to 1 digits to the right of the decimal. !!!DLV hack to change rounding
      }
      Serial.println(strBuffer);
      CharDisplayPrintLine(2, strBuffer);
Serial.println("Data Table:");
Serial.println("#          reading");
}

void loop()
{
   GoDirectBLE_Read();
      if (GoDirectBLE_DisplayChannelAsInteger())
      {
        sprintf(strBuffer, "%ld %s", (int32_t)GoDirectBLE_GetMeasurement(), strUnits);
      }
      else
      {
        sprintf(strBuffer, "%.2f %s", GoDirectBLE_GetMeasurement(), strUnits);
       
      }
      Serial.print("printing buffer rounded to 2 digits; ");
      Serial.println(strBuffer);
      CharDisplayPrintLine(2, strBuffer);
      Serial.print (" ");
      Serial.print(strBuffer[0]);
      Serial.print (" ");
      sensorReading = atof(strBuffer);
      Serial.print("printing sensor reading as a float; ");
      Serial.println(sensorReading);
      delay(500);
       /* I think I can detect the following angles:  -90, -60, -30, 0, 30, 60, 90.
      case = map(sensorReading,-90, 90, 50, 0,7);
      Serial.print("case ");
      Serial.print (case);
      switch (case) {
    case 1:
      //do something when var equals 1
      break;
    case 2:
      //do something when var equals 2
      break;
    case 3:
      //do something when var equals 1
      break;
    case 4:
      //do something when var equals 2
      break;
    case 5:
      //do something when var equals 1
      break;
    case 6:
      //do something when var equals 2
      break;
    case 7:
      //do something when var equals 1
      break;
    case 8:
      //do something when var equals 2
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
} 
  ////////////////////

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
