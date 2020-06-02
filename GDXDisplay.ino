#include "ArduinoBLE.h"
#include "GDXLib.h"
//#define TWO_LINE_DISPLAY //comment out for no DISPLAY on Arduino Nano 33 BLE
//#define TWO_LINE_DISPLAY_DIG2 //comment out for no DISPLAY on Digital 2 Shield connector
//#define FOUR_CHARACTER_DISPLAY_DIG1 //comment out for no DISPLAY on Digital 1 Shield connector
//#define STATUS //to display battery status, RSSI, and other info THIS SEEMS TO BE THE CRASHER RIGHT NOW!
//#define C_F_VERSION //C and F temperature
//#define LEDS
//#define BLE_SENSORS //Use Built-in Arduino Nano33 BLE sensors
//#define BLE_SENSE_TH//Use Built-in Arduino Nano33 Sense (temp and humidity)
//#define BLE_SENSE_P//Use Built-in Arduino Nano33 Sense (pressure)
//#define BLE_SENSE_APDS9960//Use Built-in Arduino Nano33 Sense (RGB, IR, proximity, gestures,etc)

#if defined BLE_SENSORS
  #include <Arduino_LSM9DS1.h> //for reading sensors in Arduino Nano33 BLE
#endif //BLE_SENSE

#if defined BLE_SENSE_TH
  #include <Arduino_HTS221.h>//for reading temp and humidity sensors in Arduino Nano33 Sense
#endif //BLE_SENSE_TH

#if defined BLE_SENSE_P
  #include <Arduino_LPS22HB.h> //for reading pressure sensor in Arduino Nano33 Sense
#endif //BLE_SENSE_P

#if defined BLE_SENSE_APDS9960
  #include <Arduino_APDS9960.h>
  int proximity = 0;//I think these are global variables
  int r = 0, g = 0, b = 0;
  unsigned long lastUpdate = 0;
#endif //BLE_SENSE_APDS9960

#if defined TWO_LINE_DISPLAY_DIG2
  #include <SoftwareSerial.h> //library used in printing to display
  SoftwareSerial mySerial(3,9); //for display, pin 9 = TX, pin 3 = RX (unused)
#endif //TWO_LINE_DISPLAY_DIG2

#if defined FOUR_CHARACTER_DISPLAY_DIG1
  #include <SoftwareSerial.h> //library used in printing to display
  // These are the Arduino pins required to create a software serial
  //  instance. We'll actually only use the TX pin.
  const int softwareTx = 5;//note that this is now set for the DIG 1 port
  const int softwareRx = 3;
  SoftwareSerial s7s(softwareRx, softwareTx);
#endif //FOUR_CHARACTER_DISPLAY_DIG1

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

  #if defined BLE_SENSORS
    if (!IMU.begin()) //for built-in sensors in the Arduino Nano 33BLE
      {
        Serial.println("Failed to initialize IMU!");
        while (1);
      }
  #endif //BLE_SENSORS
  
  #if defined BLE_SENSE_TH
   if (!HTS.begin()) {
     Serial.println("Failed to initialize humidity temperature sensor!");
     while (1);
     }
 #endif //BLE_SENSE_TH

 #if defined BLE_SENSE_P
  if (!BARO.begin()) {
    Serial.println("Failed to initialize pressure sensor!");
    while (1);
  }
 #endif //BLE_SENSE_P
 
 #if defined BLE_SENSE_APDS9960
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor.");
    while (true); // Stop forever
  }

 #endif //BLE_SENSE_APDS9960

 #if defined TWO_LINE_DISPLAY
    CharDisplayInit();
    delay (2000);
    //DisplayTest();//!!!to test 2-line display
    //DisplayTest4();//!!!to test 4-line display (not set up)
 #endif //TWO_LINE_DISPLAY

 
 #if defined C_F_VERSION
    Serial.print  ("special version  ");
    Serial.println("C and F temp only");
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(1, "special version");
      CharDisplayPrintLine(2, "C & F temp only");
      delay (2000);
    #endif //TWO_LINE_DISPLAY
  #endif //C_F_VERSION
  
  Serial.print("Looking for "); 
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, "Looking for ");
  #endif //TWO_LINE_DISPLAY

  #if defined TWO_LINE_DISPLAY_DIG2
    mySerial.begin(9600); // for sending characters to display
    delay(500); // wait for display to boot up
    mySerial.write(124); // adjust backlight brightness of display
    mySerial.write(150); //max=157, 150=73%, 130=40%,128=off
    delay(500); // wait for display to start
    mySerial.write(254); // command character
    mySerial.write(128); // move to line 1, position 0,
    mySerial.print("Looking for ");
  #endif //TWO_LINE_DISPLAY_DIG2

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
  
  //set things up in the steps below
  //char sensorName[64]="             ";//for proximity pairing
  //char sensorName[32]="GDX-ST 0P1000S9";
  //char sensorName[32]="GDX-TMP 0F1038J5";
  //char sensorName[32]="GDX-FOR 072001P5";
  char sensorName[32]="GDX-ACC 0H1019K1";//&&
  int period = 1000; //time between readings   
  if (period<400)
        period = 400; //do not allow faster sampling
    
  if (sensorName[1] == ' ') //if no specific sensor seleted (I used 2nd character here)
  {
    Serial.println("any GDX sensor ");
    
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(2, "any GDX sensor");
    #endif //TWO_LINE_DISPLAY

    #if defined TWO_LINE_DISPLAY_DIG2
      mySerial.write(254); // cursor to start of bottom line
      mySerial.write(192);
      mySerial.print( "any GDX sensor");
    #endif //TWO_LINE_DISPLAY_DIG2
    delay(2000);
    
    GDX.GoDirectBLE_Begin();
  }
  else
  {
    Serial.println(sensorName);
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(2, sensorName);
      delay(2000);
    #endif //TWO_LINE_DISPLAY
    
    #if defined TWO_LINE_DISPLAY_DIG2
      mySerial.write(254); // command character
      mySerial.write(192); // move to line 2, position 0,
      mySerial.print(sensorName);
      delay(2000);
    #endif //TWO_LINE_DISPLAY_DIG2
    
    GDX.GoDirectBLE_Begin(sensorName,channel, period);//specify channel and period here also
  }
  delay(1000);
  GDX.autoID();// this is the routine to get device info
  Serial.print("Found: ");
  delay(1000);
  Serial.println(GDX.deviceName());
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, "Found           ");
    //CharDisplayPrintLine(2, GDX.deviceName());
    //strcpy(strUnits,"deg C");//HACK if you want degrees displayed !!!
  #endif //TWO_LINE_DISPLAY
  #if defined TWO_LINE_DISPLAY_DIG2
    mySerial.write(254); // command character
    mySerial.write(128); // move to line 2, position 0,
    mySerial.print("Found           ");
  #endif //TWO_LINE_DISPLAY_DIG2
  delay(1000);
  Serial.print("ChannelName: ");
  Serial.println(GDX.channelName());
  Serial.print("ChannelUnits: ");
  // Cache the unit string and try to remap special UTF8
  // characters to ones that we can display.
  //THIS SEEMS TO BE A PROBLEM sprintf(strUnits, "%s", GDX.channelUnits());
  //ConvertUTF8ToASCII(strUnits);
  Serial.print("strUnits ");
  Serial.println(GDX.channelUnits());

  #if defined STATUS //THERE ARE PROBLEMS LURKING HERE IN THE STATUS
    Serial.println("RSSI: ");
    Serial.println(GDX.RSSI());
    Serial.print("battery: ");
    Serial.print(GDX.batteryPercent());
    Serial.println(" %");
    Serial.print("ChargeState: ");
    Serial.print(GDX.chargeState());
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
    Serial.print("chargeState: ");
    Serial.println(strBuffer);
  
  
    #if defined TWO_LINE_DISPLAY
      CharDisplayPrintLine(1, "RSSI ");
      //Serial1.write(254); // cursor to beginning of second line
      //Serial1.write(192); 
      //Serial1.print(GDX.RSSI());//this is broken!!!
      delay(2000);
      CharDisplayPrintLine(1, "battery: PC");
      //Serial1.write(254); // cursor to beginning of second line
      //Serial1.write(192); 
      //Serial1.print(GDX.batteryPercent());
      //Serial1.write(254);// cursor to the end of second line
      //Serial1.write(204);
      //Serial1.print("ChargeState: ");
      //Serial1.print("PC");
      delay(2000);
    
      CharDisplayPrintLine(1, "ChargeState: ");
      //CharDisplayPrintLine(2, strBuffer);// left over from use in the switch above
      delay(2000);
    #endif //TWO_LINE_DISPLAY

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
  
   #if defined BLE_SENSORS
    float x, y, z;//accelerations
    if (IMU.accelerationAvailable()) 
    {
       IMU.readAcceleration(x, y, z);
       //Serial.print("x acceleration ");
       //Serial.println(x);
    if (x>.1)// if tilted
        {
          channelReading= channelReading*1.8+32;//convert C to F degrees  HACK
          //sprintf(strBuffer, "%.2f %s", channelReading,"deg F");// causes crash
          sprintf(strBuffer, "%.2f %s", channelReading,"deg F");  
        }//end of if tilted
     }//end of if IMU
  #endif //BLE_SENSORS

 #if defined BLE_SENSE_TH
  // read all the sensor values
  float temperature = HTS.readTemperature();
  float humidity    = HTS.readHumidity();

  // print each of the sensor values
  Serial.print("Readings from Nano 33 Sense: Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");
 #endif BLE_SENSE_TH

 #if defined BLE_SENSE_P
    float pressure = BARO.readPressure();
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" kPa");
 #endif BLE_SENSE_P

 #if defined BLE_SENSE_APDS9960
      // Check if a proximity reading is available.
      if (APDS.proximityAvailable()) {
        proximity = APDS.readProximity();
      }// if APDS
    
      //check if a gesture reading is available
      if (APDS.gestureAvailable()) {
        int gesture = APDS.readGesture();
        switch (gesture) {
          case GESTURE_UP:
            Serial.println("Detected UP gesture");
            break;
    
          case GESTURE_DOWN:
            Serial.println("Detected DOWN gesture");
            break;
    
          case GESTURE_LEFT:
            Serial.println("Detected LEFT gesture");
            break;
    
          case GESTURE_RIGHT:
            Serial.println("Detected RIGHT gesture");
            break;
    
          default:
            // ignore
            break;
        }
      }
 
      // check if a color reading is available
      if (APDS.colorAvailable()) {
        APDS.readColor(r, g, b);
      }
    
      // Print updates every 100ms
      //if (millis() - lastUpdate > 100) 
      //{
        lastUpdate = millis();
        Serial.print("Proximity: ");
        Serial.println(proximity);
        Serial.print(" rgb=");
        Serial.print(r);
        Serial.print(",");
        Serial.print(g);
        Serial.print(",");
        Serial.println(b);
     // }
  #endif BLE_SENSE_APDS9960
  
  Serial.print(GDX.channelName());
  Serial.print(" channelReading ");
  Serial.print(": ");
  Serial.print(strBuffer);
  Serial.print(" ");
  Serial.println(channelReading);
  
  #if defined TWO_LINE_DISPLAY
    CharDisplayPrintLine(1, GDX.channelName());
    CharDisplayPrintLine(2,strBuffer);
  #endif // TWO_LINE_DISPLAY

  #if defined TWO_LINE_DISPLAY_DIG2
   mySerial.write(254);
   mySerial.write(1); //clear display
   mySerial.write(254); // command character
   mySerial.write(128); // move to line 1, position 0,
   mySerial.print(GDX.channelName());
   mySerial.write(254); // command character
   mySerial.write(192); // move to line 2, position 0,
   mySerial.print(channelReading);
  #endif // TWO_LINE_DISPLAY_DIG2
  
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
  delay(period-400);//in my tests, this will adjust the loop repeats at about the same time as the sample period
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

void DisplayTest()
{
  for( int b=137;b<158;b=b+10)
  {
  //this code is for the older Sparkfun displays with the add on boards
  //it seems to work with the new ones, also
  //Serial1.write(254);//clear screen
  //Serial1.write(1);
  //delay(2000);
  Serial1.write(124); // adjust backlight brightness of display
  Serial1.write(b); //max=157, 150=73%, 130=40%,128=off
  Serial1.write(254); // cursor to beginning of first line
  Serial1.write(128);
  Serial1.print("                ");
  Serial1.write(254); // cursor to beginning of first line
  Serial1.write(192);
  Serial1.print("                ");
  Serial1.write(254); // cursor to beginning of first line
  Serial1.print("Brightness Level");
  Serial1.write(254); // cursor to beginning of second line
  Serial1.write(192); 
  Serial1.print("b=");
  Serial1.write(254);// cursor to the end of second line
  Serial1.write(204);
  Serial1.print(b);
  delay(3000);
    }
  Serial1.write(124); // adjust backlight brightness of display
  Serial1.write(150); //max=157, 150=73%, 130=40%,128=off
  delay(2000);
}
void DisplayTest4()
{
  Serial1.write('|');
  //Serial1.write(24);
  //Serial1.write(contrast);
  //Serial1.write('|');
  //Serial1.write('-');
  CharDisplayPrintLine(1, "               ");
  CharDisplayPrintLine(2, "               ");
  CharDisplayPrintLine(3, "               ");
  CharDisplayPrintLine(4, "               ");
  delay(2000);  
  CharDisplayPrintLine(1, "      this     ");
  CharDisplayPrintLine(2, "        is     ");
  CharDisplayPrintLine(3, "          a    ");
  CharDisplayPrintLine(4, "           test");
  delay(2000);
  Serial1.print("Hello ");
  delay(2000);
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

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void clearDisplay()
{
  s7s.write(0x76);  // Clear display command
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255

void setBrightness(byte value)
{
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimals(byte decimals)
{
  s7s.write(0x77);
  s7s.write(decimals);
}
*/
