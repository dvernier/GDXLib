/* This is a library to make using Vernier GDX sensors 
Version 0.1
*/
#include "ArduinoBLE.h"
#include "GDXLib.h"
#define GDXLib_LIB_VERSION "0.1"//automatically displayed
GDXLib::GDXLib()
{ 
}
////////////////////////////////
char deviceNam[18];// 32 bytes !!! I made these bigger to avoid problems
char channelName[32];//60 bytes
char channelUnits[18];// 32 bytes
char sN[16];// 32 bytes this is an expermment
uint8_t batteryPercent;//HACK
uint8_t chargerStatus;
byte scanRSSI;
char strBuffer[64];
char strFW1[16];
char strFW2[16];
float channelReading;
char sensorName[] = "GDX-ACC 0H010767"; // we will use this to display later
#define D2PIO_CMD_ID_GET_STATUS                          0x10
#define D2PIO_CMD_ID_START_MEASUREMENTS                  0x18
#define D2PIO_CMD_ID_STOP_MEASUREMENTS                   0x19
#define D2PIO_CMD_ID_INIT                                0x1A
#define D2PIO_CMD_ID_SET_MEASUREMENT_PERIOD              0x1B
#define D2PIO_CMD_ID_CLEAR_ERROR_FLAGS                   0x34
#define D2PIO_CMD_ID_GET_SENSOR_CHANNEL_INFO             0x50
#define D2PIO_CMD_ID_GET_SENSOR_CHANNELS_AVAILABLE_MASK  0x51
#define D2PIO_CMD_ID_DISCONNECT                          0x54
#define D2PIO_CMD_ID_GET_DEVICE_INFO                     0x55
#define D2PIO_CMD_ID_GET_SENSOR_CHANNELS_DEFAULT_MASK    0x56
#define D2PIO_CMD_ID_IDENTIFY                            0x58
#define D2PIO_CMD_ID_GET_BATTERY_LEVEL                   0x78

#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_NORMAL                   0
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_APERIODIC_DIGITAL        1
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_APERIODIC_ANALOG         2
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_MULTIPLE_PERIODS         3
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_PERIODIC_DIGITAL_COUNTS  4
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_NORMAL_SIGNED            5
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_NORMAL_REAL32            6
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_WIDE_REAL32              7
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_SINGLE_CHANNEL_REAL32    8
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_SINGLE_CHANNEL_INT32     9
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_APERIODIC_REAL32         10
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_APERIODIC_INT32          11
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_NEXT_PERIODIC_TIMESTAMP  12
#define NGI_BLOB_MEAS_BLOB_SUB_TYPE_DROPPED_MEASUREMENT      13

#define D2PIO_MASK_STATUS_ERROR_CMD_NOT_RECOGNIZED      0x01
#define D2PIO_MASK_STATUS_ERROR_CMD_IGNORED             0x02
#define D2PIO_MASK_STATUS_ADC_UNCALIBRATED              0x04
#define D2PIO_MASK_STATUS_AUTOID_UNCALIBRATED           0x08
#define D2PIO_MASK_STATUS_ERROR_INTERNAL_ERROR1         0x10
#define D2PIO_MASK_STATUS_ERROR_AUDIO_CONTROL_FAILURE   0x10
#define D2PIO_MASK_STATUS_ERROR_INTERNAL_ERROR2         0x20
#define D2PIO_MASK_STATUS_ERROR_AUDIO_STREAM_FAILURE    0x20
#define D2PIO_MASK_STATUS_ERROR_MASTER_FIFO_OVERFLOW    0x40
#define D2PIO_MASK_STATUS_ERROR_DIGITAL_TRANSITION_LOST 0x80

#define D2PIO_CHARGER_STATE_IDLE     0
#define D2PIO_CHARGER_STATE_CHARGING 1
#define D2PIO_CHARGER_STATE_COMPLETE 2
#define D2PIO_CHARGER_STATE_ERROR    3

#define D2PIO_MAX_ORDERCODE_LENGTH 16
#define D2PIO_MAX_SERIALNUMBER_LENGTH 16
#define D2PIO_BLE_ADDRESS_LENGTH 6
#define D2PIO_MAX_DEVICENAME_LENGTH 32
#define D2PIO_MAX_DEVICEDESCRIPTION_LENGTH 64
#define D2PIO_MAX_NUM_BYTES_IN_SENSOR_DESCRIPTION 60
#define D2PIO_MAX_NUM_BYTES_IN_SENSOR_UNIT 32

// Define this in order to display the
// raw D2PIO bytes stream via the serial port.
#define TRACE_D2PIO_PACKETS

struct D2PIOGetStatusCmdResponsePayload
{
  uint8_t  status; //See D2PIO_MASK_STATUS_*.
  uint8_t  spare;
  uint8_t  majorVersionMasterCPU;
  uint8_t  minorVersionMasterCPU;
  uint16_t buildNumMasterCPU;
  uint8_t  majorVersionSlaveCPU;
  uint8_t  minorVersionSlaveCPU;
  uint16_t buildNumSlaveCPU;
  uint8_t  batteryLevelPercent;
  uint8_t  chargerState;       //D2PIO_CHARGER_STATE_*
} __attribute__ ((packed));

struct D2PIOStartMeasurementsParams
{
  int8_t   channel;  /* -1 => all channels specified by channelBits. */
  uint8_t  sampleAtTime0flag;
  uint32_t channelBits;    //Id's channels that should start taking measurements. Ignored unless (channel == -1).
  int64_t  startTimestamp; //Microseconds since Jan 1 1970 Coordinated Universal Time(aka UTC aka Greenwich Mean Time), (timestamp == 0) => start immediately.
} __attribute__ ((packed));

struct D2PIOStopMeasurementsParams
{
  int8_t   channel;  /* -1 => all channels specified by channelBits. */
  uint8_t  spare;
  uint32_t channelBits;    //Id's channels that should start taking measurements. Ignored unless (channel == -1).
} __attribute__ ((packed));

struct D2PIOSetMeasurementPeriodParams
{
  int8_t   channel;  /* -1 => all channels specified by channelBits. */
  uint8_t  spare;
  uint64_t measurementPeriod; //In 'ticks', typically a tick == 1 microsecond
} __attribute__ ((packed));

struct D2PIOGetSensorChannelInfoCmdResponse
{
  int8_t   channel;
  uint8_t  spare;
  uint32_t sensorId; //unique record key for the Sensor Map
  uint8_t  numericMeasType;    //D2PIO_NUMERIC_MEAS_TYPE_...
  uint8_t  samplingMode;       //D2PIO_SAMPLING_MODE_...
  char     sensorDescription[D2PIO_MAX_NUM_BYTES_IN_SENSOR_DESCRIPTION];
  char     sensorUnit[D2PIO_MAX_NUM_BYTES_IN_SENSOR_UNIT];
  double   measurementUncertainty; //real number expressed in sensorUnit's.
  double   minMeasurement;     //real number expressed in sensorUnit's.
  double   maxMeasurement;     //real number expressed in sensorUnit's.
  uint32_t minMeasurementPeriod;   //in microsecond ticks
  uint64_t maxMeasurementPeriod;   //in microsecond ticks
  uint32_t typMeasurementPeriod;   //in microsecond ticks
  uint32_t measurementPeriodGranularity; //in microsecond ticks, supported periods are integer multiples of measurementPeriodGranularity
  uint32_t mutualExclusionMask; //channels that cannot be enabled at the same time as this channel
} __attribute__ ((packed));

struct D2PIOGetDeviceInfoCmdResponse
{
  char     OrderCode[D2PIO_MAX_ORDERCODE_LENGTH];//NULL terminated UTF-8 string.
  char     SerialNumber[D2PIO_MAX_SERIALNUMBER_LENGTH];//NULL terminated UTF-8 string.
  char     DeviceName[D2PIO_MAX_DEVICENAME_LENGTH];//NULL terminated UTF-8 string.
  uint16_t manufacturerId;
  uint16_t manufacturedYear;
  uint8_t  ManufacturedMonth;
  uint8_t  ManufacturedDay;
  uint8_t  majorVersionMasterCPU;
  uint8_t  minorVersionMasterCPU;
  uint16_t buildNumMasterCPU;
  uint8_t  majorVersionSlaveCPU;
  uint8_t  minorVersionSlaveCPU;
  uint16_t buildNumSlaveCPU;
  uint8_t  BLE_Address[D2PIO_BLE_ADDRESS_LENGTH];  // Raw binary, not NULL terminated string. Displays like this --> [5]:[4]:[3]:[2]:[1]:[0]
  uint32_t NVMemSize;
  char     DeviceDescription[D2PIO_MAX_DEVICEDESCRIPTION_LENGTH];//NULL terminated UTF-8 string.
} __attribute__ ((packed));


#define GDX_BLE_AUTO_CONNECT_RSSI_THRESHOLD -44

#define GDX_BLE_STATE_RESET      0
#define GDX_BLE_STATE_SCAN_IDLE  1
#define GDX_BLE_STATE_SCAN_WEAK  2
#define GDX_BLE_STATE_SCAN_FLUSH 3
#define GDX_BLE_STATE_CONNECT    4
#define GDX_BLE_STATE_DISCOVER   5
#define GDX_BLE_STATE_STATUS     6
#define GDX_BLE_STATE_SETUP      7
#define GDX_BLE_STATE_READY      8
#define GDX_BLE_STATE_MEASURE    9
#define GDX_BLE_STATE_IDLE       10
#define GDX_BLE_STATE_ERROR      11

#define D2PIO_SCAN_RESULT_NONE     0
#define D2PIO_SCAN_RESULT_WEAK     1
#define D2PIO_SCAN_RESULT_FLUSH    2
#define D2PIO_SCAN_RESULT_SUCCESS  3


static int                                         g_State = GDX_BLE_STATE_RESET;
static BLEDevice                                   g_peripheral;
static BLECharacteristic                           g_d2pioCommand;
static BLECharacteristic                           g_d2pioResponse;
static struct D2PIOGetStatusCmdResponsePayload     g_status;
static struct D2PIOGetDeviceInfoCmdResponse        g_deviceInfo;
static struct D2PIOGetSensorChannelInfoCmdResponse g_channelInfo;
static const char*                                 g_deviceName;
static byte                                        g_channelNumber;
static unsigned long                               g_samplePeriodInMilliseconds;
static bool                                        g_autoConnect;
static byte                                        g_rollingCounter = 0;
static byte                                        g_ReadBuffer[256];
static unsigned long                               g_MeasurementCounter;
static float                                       g_measurement;
static byte                                        g_RSSIStrength;
static unsigned long                               g_RSSIAge;



 //below are mentions of functions to avoid the not found stuff!!!
void GoDirectBLE_Start();
void GoDirectBLE_GetStatus();
bool D2PIO_StartMeasurements(byte channelNumber);
byte D2PIO_CalculateChecksum(const byte buffer[]);
bool D2PIO_ReadMeasurement(byte buffer[], int timeout, float& measurement);

//=============================================================================
// GoDirectBLE_Test2() Function !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//=============================================================================
void GDXLib::GoDirectBLE_Test2()//test function to call at the top
{
  Serial.println( "*** test2 function at the top ");
  Serial.print ("*** ");
  Serial.println(_c);
  _c=_c+5;
}
//=============================================================================
// GoDirectBeginL() Function !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//=============================================================================!@
void GDXLib::GoDirectBLE_BeginL(byte channelNumber, unsigned long samplePeriodInMilliseconds)
 { 
  //if (2>3)
  // GoDirectBLE_Start();//HACK
   _a=6;
   _b=9;
   _c=_a+_b;
   Serial.print ("*** in BeginL ");
   Serial.print ("*** ");
   Serial.println(_c);
   GoDirectBLE_Test();//at the bottom
   GoDirectBLE_Test2();//at the top using "GDXLib.GoDirectBLE_Test2();"fails
   GoDirectBLE_Test3();//bottom; using "GDXLib.GoDirectBLE_Test3(); " fails here
   Serial.print(" end of GoDirectBLE_BeginL");
 }

 float GDXLib::getNumber()
 {
 //byte number=GoDirectBLE_GetScanRSSI(); //this works!
 float sss=GoDirectBLE_GetMeasurement();
 return sss;
 }
/* void GDXLib::autoID()
 {
 _shortName[20]="1234567890123456789";
 }
 */
float GDXLib::readSensor() 
{
  //GoDirectBLE_Test();
  float sensorReading= 34343434;
  Serial.print("***sensorReading ");
  Serial.println(sensorReading);
  //char strBuffer[64];
  //if (!BLE.connected())
  //    GoDirectBLE_Start();
  // the function being called is labelled as follows:  bool GDXLib::D2PIO_ReadMeasurement(byte buffer[], int timeout, float& measurement)
  if (!D2PIO_ReadMeasurement(g_ReadBuffer, 5000, g_measurement))//'g_ReadBuffer' was not declared in this scope
    GoDirectBLE_Start();
  sensorReading=g_measurement;
  return sensorReading;

  }


// note that the begin methods are way below, but they should be set up
//void GDXLib::GoDirectBLE_Begin(char* deviceName, byte channelNumber, unsigned long samplePeriodInMilliseconds)
//void GDXLib::GoDirectBLE_Begin()  // maybe rename this GoDirectStart()

//=============================================================================
// DumpGatttService() Function
//=============================================================================
bool GDXLib::DumpGatttService(BLEDevice peripheral, char* uuid)
{
  int i;

  // Discover peripheral attributes
  delay(2000);
  Serial.println("***Discovering service attributes ...");
  if (!peripheral.discoverService(uuid))
  {
    Serial.println("***Service attribute discovery failed!");
    return false;
  }

  int totalServices = peripheral.serviceCount();
  if (totalServices < 1) return false;

  Serial.print("**Found ");
  Serial.println(peripheral.service(uuid).uuid());
  delay(2000);

  int totalChars = peripheral.service(uuid).characteristicCount();
  Serial.println("***Characteristics: ");
  for (i = 0; i < totalChars; i++)
  {
    Serial.print("***  ");
    Serial.print(peripheral.service(uuid).characteristic(i).uuid());
    Serial.println();
  }

  return true;
}

//=============================================================================
// D2PIO_Scan() Function
//=============================================================================
int D2PIO_Scan(bool useRssiThreshold, int threshold)
{
  // Check if a peripheral has been discovered
  //Serial.print("***in D2PIO_Scan, useRssiThreshold= ");//!!!!
  //Serial.print(useRssiThreshold);
  //Serial.print("*** threshold= ");
  //Serial.println(threshold);
  Serial.println("***+");
  BLEDevice peripheral = BLE.available();
  if (!peripheral)
  {
    // Age the last known RSSI reading
    if (g_RSSIStrength > 0) g_RSSIAge++;

    // Flush the RSSI if it is too old
    if (g_RSSIAge > 200000)
    {
      g_RSSIAge = 0;
      g_RSSIStrength = 0;
      return D2PIO_SCAN_RESULT_FLUSH;
    }

    return D2PIO_SCAN_RESULT_NONE;
  }

  // Make sure we found a GDX device
  if ((peripheral.localName()[0] != 'G') ||
      (peripheral.localName()[1] != 'D') ||
      (peripheral.localName()[2] != 'X'))
    return D2PIO_SCAN_RESULT_NONE;

  // Create a relative strength reading from 0 to 16
  // 0  = Very weak
  // 16 = Strong enough to connect
  g_RSSIAge = 0;
  int rssiTestLevel = threshold;
  for (g_RSSIStrength = 16; g_RSSIStrength > 0; g_RSSIStrength--)
  {
    if (peripheral.rssi() > rssiTestLevel) break;
    rssiTestLevel = rssiTestLevel - 2;
  }

  Serial.print("***Found ");
  Serial.print(peripheral.localName());
  Serial.print("*** at ");
  Serial.print(peripheral.address());
  Serial.print("*** with RSSI ");
  Serial.print(peripheral.rssi());
  Serial.println();

  //  uint8_t mfgData[64];
  //  uint8_t mgfDataLen = 0;
  //  Serial.print("***MfgData=");
  //  Serial.print(peripheral.hasManufacturerData());
  //  Serial.print("*** ");
  //  peripheral.getManufacturerData(mfgData, mgfDataLen);
  //  for (int i = 0; i < mgfDataLen; i++)
  //  {
  //    Serial.print(mfgData[i], HEX);
  //    Serial.print("*** ");
  //  }
  //  Serial.println();

  // Check RSSI threshold (optional)
  if (useRssiThreshold && (peripheral.rssi() < threshold)) return D2PIO_SCAN_RESULT_WEAK;

  g_peripheral = peripheral;
  return D2PIO_SCAN_RESULT_SUCCESS;
}

//=============================================================================
// D2PIO_DiscoverService() Function
//=============================================================================
bool GDXLib::GDXLib::D2PIO_DiscoverService(BLEDevice peripheral)
{
  char uuidService[]  = "d91714ef-28b9-4f91-ba16-f0d9a604f112";
  char uuidCommand[]  = "f4bf14a6-c7d5-4b6d-8aa8-df1a7c83adcb";
  char uuidResponse[] = "b41e6675-a329-40e0-aa01-44d2f444babe";

  // --------------------------------------------
  // Discover the D2PIO service
  // --------------------------------------------
  Serial.println("***Discovering D2PIO service attributes ...");
  if (!peripheral.discoverService(uuidService))
  {
    Serial.println("***ERROR: D2PIO service attribute discovery failed!");
    return false;
  }
  Serial.print("***Found D2PIO service ");
  Serial.println(peripheral.service(uuidService).uuid());

  // --------------------------------------------
  // Discover the command characteristic
  // --------------------------------------------
  g_d2pioCommand = peripheral.service(uuidService).characteristic(uuidCommand);
  if (!g_d2pioCommand)
  {
    Serial.println("***ERROR: D2PIO command characteristic discovery failed!");
    return false;
  }
  Serial.print("***Found D2PIO command characteristic ");
  Serial.println(peripheral.service(uuidService).characteristic(uuidCommand).uuid());

  // --------------------------------------------
  // Discover the response characteristic
  // --------------------------------------------
  g_d2pioResponse = peripheral.service(uuidService).characteristic(uuidResponse);
  if (!g_d2pioResponse)
  {
    Serial.println("***ERROR: D2PIO response characteristic discovery failed!");
    return false;
  }
  Serial.print("***Found D2PIO response characteristic ");
  Serial.println(peripheral.service(uuidService).characteristic(uuidResponse).uuid());

  if (!g_d2pioResponse.subscribe()) {
    Serial.println("***ERROR: Failed to subscribe to D2PIO esponse characteristic");
    return false;
  }
  Serial.println("***Subscribed to D2PIO response notifications");
  //d2pioResponse.setEventHandler(BLEValueUpdated, D2PIO_ResponseHandler);

  return true;
}

//=============================================================================
// D2PIO_CalculateChecksum() Function
//=============================================================================
byte GDXLib::GDXLib::D2PIO_CalculateChecksum(const byte buffer[])
{
  byte length   =  buffer[1];
  byte checksum = -buffer[3];
  byte i;

  for (i = 0; i < length; i++) checksum += buffer[i];
  return checksum;
}

//=============================================================================
// D2PIO_Dump() Function
//=============================================================================
#ifdef TRACE_D2PIO_PACKETS
void GDXLib::D2PIO_Dump(const char* strPrefix, const byte buffer[])
{
  byte i;
  Serial.print(strPrefix);

  for (i = 0; i < buffer[1]; i++)
  {
    Serial.print(buffer[i], HEX);
    Serial.print("*** ");
  }
  Serial.println();
}
#else
#define D2PIO_Dump(strPrefix, buffer)
#endif

//=============================================================================
// D2PIO_Write() Function
//=============================================================================
bool GDXLib::D2PIO_Write(const byte buffer[])
{
  D2PIO_Dump("D2PIO >> ", buffer);

  byte lengthRemaining = buffer[1];
  byte lengthChunk;
  byte offset = 0;

  while (lengthRemaining > 0)
  {
    lengthChunk = lengthRemaining;
    //if (lengthChunk > 20) lengthChunk = 20;

    if (!g_d2pioCommand.writeValue(&buffer[offset], lengthChunk))
    {
      Serial.println("***ERROR: D2PIO_Init write failed");
      return false;
    }

    lengthRemaining = lengthRemaining - lengthChunk;
    offset = offset + lengthChunk;
  }

  return true;
}

//=============================================================================
// D2PIO_ReadBlocking() Function
//=============================================================================
bool GDXLib::D2PIO_ReadBlocking(byte buffer[], int timeout)
{
  byte offset = 0;
  int timeoutCounter = 0;

  while (true)
  {
    // Wait for the next chunk
    while (!g_d2pioResponse.valueUpdated())
    {
      timeoutCounter++;
      if (timeoutCounter > timeout)
      {
        Serial.println("***ERROR: D2PIO_ReadBlocking timeout!");
        return false;
      }
      delay(1);
    }

    // Copy the current chunk into the output buffer
    memcpy(&buffer[offset], g_d2pioResponse.value(), g_d2pioResponse.valueLength());
    offset = offset + g_d2pioResponse.valueLength();

    // Check if we have received the complete packet
    if ((offset >= 1) && (offset == buffer[1])) break;
  }

  D2PIO_Dump("D2PIO << ", buffer);
  return true;
}

//=============================================================================
// D2PIO_ReadMeasurement() Function
//=============================================================================
bool GDXLib::D2PIO_ReadMeasurement(byte buffer[], int timeout, float& measurement)
{
  byte offset = 0;
  int timeoutCounter = 0;

  // Return immediately if there is nothing to do.
  if (!g_d2pioResponse.valueUpdated()) return false;

  while (true)
  {
    // Copy the current chunk into the output buffer
    memcpy(&buffer[offset], g_d2pioResponse.value(), g_d2pioResponse.valueLength());
    offset = offset + g_d2pioResponse.valueLength();

    // Check if we have received the complete packet
    if ((offset >= 1) && (offset == buffer[1])) break;

    // Now that we have started received a measurement,
    // we must wait for all of it to arrive.
    while (!g_d2pioResponse.valueUpdated())
    {
      timeoutCounter++;
      if (timeoutCounter > timeout)
      {
        Serial.println("***ERROR: D2PIO_ReadMeasurement timeout!");
        return false;
      }
      delay(1);
    }
  }

  D2PIO_Dump("D2PIO << ", buffer);

  // Extract normal measurement packets -- NGI_BLOB_MEAS_BLOB_SUB_TYPE_NORMAL_REAL32
  // We only take the first measurement from the packet.  The protocol allows
  // multiple to get stuffed into one packet but we just ignore the extras.
  if (buffer[4] == NGI_BLOB_MEAS_BLOB_SUB_TYPE_NORMAL_REAL32)
  {
    float record;
    memcpy(&record, &buffer[9], 4);
    measurement = record;
  }
  else if (buffer[4] == NGI_BLOB_MEAS_BLOB_SUB_TYPE_WIDE_REAL32)
  {
    float record;
    memcpy(&record, &buffer[11], 4);
    measurement = record;
  }
  else if (buffer[4] == NGI_BLOB_MEAS_BLOB_SUB_TYPE_SINGLE_CHANNEL_INT32)
  {
    int32_t recordI32 = 0;
    memcpy(&recordI32, &buffer[8], 4);
    measurement = recordI32;
  }
  else if (buffer[4] == NGI_BLOB_MEAS_BLOB_SUB_TYPE_APERIODIC_INT32)
  {
    int32_t recordI32 = 0;
    memcpy(&recordI32, &buffer[8], 4);
    measurement = recordI32;
  }
  else
  {
    // Other BLOB sub-types not currently supported
    return false;
  }

  return true;
}

//=============================================================================
// D2PIO_Init() Function
//=============================================================================
bool GDXLib::D2PIO_Init()
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, 0x1A,
    0xa5, 0x4a, 0x06, 0x49,
    0x07, 0x48, 0x08, 0x47,
    0x09, 0x46, 0x0a, 0x45,
    0x0b, 0x44, 0x0c, 0x43,
    0x0d, 0x42, 0x0e, 0x41
  };

  // Reset the rolling packet counter
  g_rollingCounter = 0xFF;

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;
  return true;
}

//=============================================================================
// D2PIO_SetMeasurementPeriod() Function
//=============================================================================
bool GDXLib::D2PIO_SetMeasurementPeriod(unsigned long measurementPeriodInMilliseconds)
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, 0x1B,
    0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };

  // Convert to milliseconds and populate the payload
  unsigned long measurementPeriodInMicroseconds = measurementPeriodInMilliseconds * 1000;
  command[7]  = (measurementPeriodInMicroseconds >> 0)  & 0xFF;
  command[8]  = (measurementPeriodInMicroseconds >> 8)  & 0xFF;
  command[9]  = (measurementPeriodInMicroseconds >> 16) & 0xFF;
  command[10] = (measurementPeriodInMicroseconds >> 24) & 0xFF;

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;
  return true;
}

//=============================================================================
// D2PIO_GetDefaultChannel() Function
//=============================================================================
//bool GDXLib::D2PIO_GetDefaultChannel(byte& channelNumber)
//{
//    byte command[] = {
//      0x58, 0x00, 0x00, 0x00, 0x56
//    };
//
//    // Populate the packet header bytes
//    command[1] = sizeof(command);
//    command[2] = g_rollingCounter--;
//    command[3] = D2PIO_CalculateChecksum(command);
//
//    if (!D2PIO_Write(command)) return false;
//    if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;
//
//    // Extract the channel mask from the packet
//    unsigned long channelMask;
//    memcpy(&channelMask, &g_ReadBuffer[6], 4);
//
//    // Find the first channel number that is set
//    int i;
//    unsigned long testMask = 1;
//    for (i = 0; i < 32; i++)
//    {piiiiiiiiiiiiiiuu
//      if (testMask & channelMask)
//      {
//        channelNumber = i;
//        break;
//      }
//      testMask = testMask << 1;
//    }
//
//    if (i == 32) return false;
//    return true;
//}

//=============================================================================
// D2PIO_GetAvailableChannels() Function
//=============================================================================
bool GDXLib::D2PIO_GetAvailableChannels(unsigned long& availableMask)
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, 0x51
  };

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;

  // Extract the channel mask from the packet
  unsigned long mask;
  memcpy(&mask, &g_ReadBuffer[6], 4);
  availableMask = mask;
  return true;
}

//=============================================================================
// D2PIO_GetDefaultChannels() Function
//=============================================================================
bool GDXLib::D2PIO_GetDefaultChannels(unsigned long& defaultMask)
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, 0x56
  };

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;

  // Extract the channel mask from the packet
  unsigned long mask;
  memcpy(&mask, &g_ReadBuffer[6], 4);
  defaultMask = mask;
  return true;
}

//=============================================================================
// D2PIO_GetStatus() Function
//=============================================================================
bool GDXLib::D2PIO_GetStatus()
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, D2PIO_CMD_ID_GET_STATUS
  };

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;

  D2PIOGetStatusCmdResponsePayload* pResponse;
  pResponse = (D2PIOGetStatusCmdResponsePayload*)&g_ReadBuffer[6];
  memcpy(&g_status, pResponse, sizeof(D2PIOGetStatusCmdResponsePayload));

  Serial.println("***Device status:");
  Serial.print("***  Status: ");
  Serial.println(pResponse->status);
  Serial.print("***  Master FW version: ");
  Serial.print(pResponse->majorVersionMasterCPU);
  Serial.print("***.");
  Serial.print(pResponse->minorVersionMasterCPU);
  Serial.print("***.");
  Serial.println(pResponse->buildNumMasterCPU);
  Serial.print("***  Slave FW version: ");
  Serial.print(pResponse->majorVersionSlaveCPU);
  Serial.print("***.");
  Serial.print(pResponse->minorVersionSlaveCPU);
  Serial.print("***.");
  Serial.println(pResponse->buildNumSlaveCPU);
  Serial.print("*** in D2PIO_GetStatus Battery percent: ");
  batteryPercent=(pResponse->batteryLevelPercent);
  Serial.println(pResponse->batteryLevelPercent);
  //Serial.print("***  batteryLevelPercent: ");
  //Serial.println(batteryLevelPercent);
  
  Serial.print("***  Charger state: ");
  Serial.println(pResponse->chargerState);
  chargerStatus= (pResponse->chargerState);//!!!
  return true;
}

//=============================================================================
// D2PIO_GetDeviceInfo() Function
//=============================================================================
bool GDXLib::D2PIO_GetDeviceInfo()
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, D2PIO_CMD_ID_GET_DEVICE_INFO
  };

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;

  D2PIOGetDeviceInfoCmdResponse* pResponse;
  pResponse = (D2PIOGetDeviceInfoCmdResponse*)&g_ReadBuffer[6];
  memcpy(&g_deviceInfo, pResponse, sizeof(D2PIOGetDeviceInfoCmdResponse));

  Serial.println("***Device info:");
  Serial.print("***  Description: ");
  Serial.println(pResponse->DeviceDescription);
  Serial.println(pResponse->SerialNumber);
  Serial.print("***  Device name: ");
  Serial.println(pResponse->DeviceName);
  strcpy(deviceNam, pResponse->DeviceName);//!!!
  Serial.print("***  Mfg ID: ");
  Serial.println(pResponse->manufacturerId);
  Serial.print("***  Mfg Date: ");
  Serial.print(pResponse->ManufacturedMonth);
  Serial.print("***/");
  Serial.print(pResponse->ManufacturedDay);
  Serial.print("***/");
  Serial.println(pResponse->manufacturedYear);
  Serial.print("***  Master FW version: ");
  Serial.print(pResponse->majorVersionMasterCPU);
  Serial.print("***.");
  Serial.print(pResponse->minorVersionMasterCPU);
  Serial.print("***.");
  Serial.println(pResponse->buildNumMasterCPU);
  Serial.print("***  Slave FW version: ");
  Serial.print(pResponse->majorVersionSlaveCPU);
  Serial.print("***.");
  Serial.print(pResponse->minorVersionSlaveCPU);
  Serial.print("***.");
  Serial.println(pResponse->buildNumSlaveCPU);
  Serial.print("***  BLE address: ");
  Serial.print(pResponse->BLE_Address[5], HEX);
  Serial.print("***:");
  Serial.print(pResponse->BLE_Address[4], HEX);
  Serial.print("***:");
  Serial.print(pResponse->BLE_Address[3], HEX);
  Serial.print("***:");
  Serial.print(pResponse->BLE_Address[2], HEX);
  Serial.print("***:");
  Serial.print(pResponse->BLE_Address[1], HEX);
  Serial.print("***:");
  Serial.println(pResponse->BLE_Address[0], HEX);
  Serial.print("***  NVRAM size: ");
  Serial.println(pResponse->NVMemSize);
  return true;
}
//=============================================================================
// D2PIO_GetChannelInfo() Function
//=============================================================================
bool GDXLib::D2PIO_GetChannelInfo(byte channelNumber, bool verbose)
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, D2PIO_CMD_ID_GET_SENSOR_CHANNEL_INFO,
    0x00
  };

  // Specify the channel number parameter
  command[5] = channelNumber;

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;

  D2PIOGetSensorChannelInfoCmdResponse* pResponse;
  pResponse = (D2PIOGetSensorChannelInfoCmdResponse*)&g_ReadBuffer[6];
  memcpy(&g_channelInfo, pResponse, sizeof(D2PIOGetSensorChannelInfoCmdResponse));

  if (verbose)
  {
    Serial.print("***Channel[");
    Serial.print(channelNumber);
    Serial.println("***] info:");
    Serial.print("***  Description: ");
    Serial.println(pResponse->sensorDescription); 
    strcpy(channelName, pResponse->sensorDescription);//!!!note this works, but is it the right channel?
    Serial.print("***  ID: ");
    Serial.println(pResponse->sensorId);
    Serial.print("***  Measurement type: ");
    Serial.println(pResponse->numericMeasType);
    Serial.print("***  Sampling mode: ");
    Serial.println(pResponse->samplingMode);
    Serial.print("***  Units: ");
    Serial.println(pResponse->sensorUnit);
    strcpy(channelUnits, pResponse->sensorUnit);//!!!note this works, but is it the right channel?
    Serial.print("***  Measurement uncertainty: ");
    Serial.println(pResponse->measurementUncertainty);
    Serial.print("***  Measurement min: ");
    Serial.println(pResponse->minMeasurement);
    Serial.print("***  Measurement max: ");
    Serial.println(pResponse->maxMeasurement);
    Serial.print("***  Period typical: ");
    Serial.println(pResponse->typMeasurementPeriod);
    Serial.print("***  Period min: ");
    Serial.println(pResponse->minMeasurementPeriod);
    Serial.print("***  Period max: ");
    Serial.println((long int)(pResponse->maxMeasurementPeriod));
    Serial.print("***  Period granularity: ");
    Serial.println(pResponse->measurementPeriodGranularity);
    Serial.print("***  Mutual exclusion mask: 0x");
    Serial.println(pResponse->mutualExclusionMask);
  }

  return true;
}
//=============================================================================
// D2PIO_GetChannelInfoAll() Function
//=============================================================================
bool GDXLib::D2PIO_GetChannelInfoAll()
{

  unsigned long availableMask = 0;
  unsigned long testMask = 1;
  byte i;

  if (!D2PIO_GetAvailableChannels(availableMask)) return false;

  for (i = 0; i < 32; i++)
  {
    if (testMask & availableMask)
    {
      if (!D2PIO_GetChannelInfo(i, true)) return false;
    }
    testMask = testMask << 1;
  }
 ;  return true;
}

//=============================================================================
// D2PIO_Autoset() Function
//=============================================================================
bool GDXLib::D2PIO_Autoset()
{
  unsigned long availableMask = 0;
  unsigned long defaultMask = 0;
  unsigned long testMask = 1;
  byte i;
  if (!D2PIO_GetAvailableChannels(availableMask)) return false;
  if (!D2PIO_GetDefaultChannels(defaultMask)) return false;

  // Select the first channel number that is called out in the default mask
  for (i = 0; i < 32; i++)
  {
    if (testMask & defaultMask & availableMask)
    {
      g_channelNumber = i;
      break;
    }
    testMask = testMask << 1;
  }
  if (i == 32) return false;

  // Get the channel info
  if (!D2PIO_GetChannelInfo(g_channelNumber, false)) return false;

  // Set the sample rate according to the typical value for this sensor.
  // However we limit it to about 200ms for the sake of Arduino.
  // Not sure if this is actually slow enough!?
  g_samplePeriodInMilliseconds = g_channelInfo.typMeasurementPeriod / 1000;
  if (g_samplePeriodInMilliseconds < 200) g_samplePeriodInMilliseconds = 200;

  Serial.print("***Autoset channel number: ");
  Serial.println(g_channelNumber);
  Serial.print("***Autoset sample period (ms): ");
  Serial.println(g_samplePeriodInMilliseconds);

  return true;
}

//=============================================================================
// D2PIO_StartMeasurements() Function
//=============================================================================
bool GDXLib::D2PIO_StartMeasurements(byte channelNumber)
{
  byte command[] = {
    0x58, 0x00, 0x00, 0x00, 0x18,
    0xFF,
    0x01,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

  // Convert the channel number to a bitmask and populate the payload
  unsigned long channelMask = 1;
  channelMask = channelMask << channelNumber;
  command[7]  = (channelMask >> 0)  & 0xFF;
  command[8]  = (channelMask >> 8)  & 0xFF;
  command[9]  = (channelMask >> 16) & 0xFF;
  command[10] = (channelMask >> 24) & 0xFF;

  // Populate the packet header bytes
  command[1] = sizeof(command);
  command[2] = g_rollingCounter--;
  command[3] = D2PIO_CalculateChecksum(command);

  if (!D2PIO_Write(command)) return false;
//  if (!D2PIO_ReadBlocking(g_ReadBuffer, 5000)) return false;  ///THIS BIZARRELY CAUSES AN ERROR!!!!!!
  return true;
}

//end of D2PIO functions, start of turnkey stuff


//=============================================================================
// GoDirectBLE_Start() Function
//=============================================================================
void GDXLib::GoDirectBLE_Start()
{
  BLE.begin();
  Serial.print("***this is in the GoDirectBLE_Start Function ");
  Serial.println("***BLE reset");
  // Cleanup any old connections //Kevin's reset
  if (BLE.connected())
    BLE.disconnect();
  if (g_autoConnect)
    BLE.scan(true);
  else
    BLE.scanForName(g_deviceName, true);
}

//=============================================================================
// GoDirectBLE_Begin() Function
//=============================================================================
void GDXLib::GoDirectBLE_Begin(char* deviceName, byte channelNumber, unsigned long samplePeriodInMilliseconds)
{
  Serial.print("***in Begin");
  Serial.println(deviceName);
  g_deviceName = deviceName; // this really is the device name
  g_channelNumber = channelNumber;
  g_samplePeriodInMilliseconds = samplePeriodInMilliseconds;
  g_autoConnect = false;
  Serial.print("***g_channelNumber specified ");
  Serial.println(g_channelNumber);
  Serial.print("***deviceName");
  Serial.println(deviceName);
  BLE.begin();
  Serial.print("***BLE reset");
  // Cleanup any old connections //Kevin's reset
  if (BLE.connected())
    BLE.disconnect();
  if (g_autoConnect)
    BLE.scan(true);
  else
    BLE.scanForName(g_deviceName, true);

  int scanResult = D2PIO_SCAN_RESULT_NONE; //0
  //Kevin's Idle through Flush
  while (scanResult != D2PIO_SCAN_RESULT_SUCCESS) //3
  {
    scanResult = D2PIO_Scan(g_autoConnect, GDX_BLE_AUTO_CONNECT_RSSI_THRESHOLD);
    if (scanResult == D2PIO_SCAN_RESULT_SUCCESS) //3
     {  
       Serial.println("***D2PIO_SCAN_RESULT_SUCCESS");
       break;
      }
    if (scanResult == D2PIO_SCAN_RESULT_WEAK) //1
      Serial.println("***D2PIO_SCAN_RESULT_WEAK");
    if (scanResult == D2PIO_SCAN_RESULT_FLUSH) //2
      Serial.println("D2PIO_SCAN_RESULT_FLUSH");
  }//end while
  // Stop scanning
  BLE.stopScan();
  Serial.print("***Connecting...");
  if (!g_peripheral.connect())
  {
    Serial.println("FAILED!");
    GoDirectBLE_Start();
  }
  else//end if
  {
    Serial.println("SUCCESS");
  delay(2000);  // seems okay without this delay
  if (!D2PIO_DiscoverService(g_peripheral)) //Kevin's Discover FAILS BOTH WAYS
    GoDirectBLE_Start();
  if (!D2PIO_Init())
    GoDirectBLE_Start();

  // Wait for connection interval to finish negotiating
  delay(1000);
  //Serial.println("BLE connection parameters:");
  //Serial.print(  "  Interval: ");
  //Serial.println(g_peripheral.getConnectionInterval());
  //Serial.print(  "  Timeout:  ");
  //Serial.println(g_peripheral.getConnectionTimeout());
  //Serial.print(  "  Latency:  ");
  //Serial.println(g_peripheral.getConnectionLatency());

  Serial.println("***%%%D2PIO_GetStatus()");
  if (!D2PIO_GetStatus())
    GoDirectBLE_Start();

  Serial.println("***%%%D2PIO_GetDeviceInfo()");
  if (!D2PIO_GetDeviceInfo()) //Kevin's Setup
    GoDirectBLE_Start();

  Serial.println("***%%%D2PIO_GetChannelInfoAll()");
  if (!D2PIO_GetChannelInfoAll())
    GoDirectBLE_Start();
    
  }
}
//=============================================================================
// GoDirectBLE_Begin() Function
//=============================================================================
void GDXLib::GoDirectBLE_Begin()  // maybe rename this GoDirectStart()
{
  Serial.println("***in Begin(GDX....");
  Serial.println(g_deviceName);
  g_deviceName = NULL;
  g_channelNumber = -1;
  g_samplePeriodInMilliseconds = 0;
  g_autoConnect = true;
  GoDirectBLE_Start();//!!!! error here finding Start 
  if (g_autoConnect)
  {
    if (!D2PIO_Autoset())
      GoDirectBLE_Start();
  }//end if
 // GoDirectBLE_GetStatus(strFW1, strFW2, batteryPercent);//!!!!DLV HACK 2/11/2020
  Serial.print("***D2PIO_GetChannelInfo(g_channelNumber");
  if (!D2PIO_GetChannelInfo(g_channelNumber, false))
    GoDirectBLE_Start();

  Serial.print("***D2PIO_SetMeasurementPeriod");
  if (!D2PIO_SetMeasurementPeriod(g_samplePeriodInMilliseconds))
    GoDirectBLE_Start();
} //end begins


//=============================================================================
// GoDirectBLE_Measure Function
//=============================================================================
void GDXLib::GoDirectBLE_Measure()  //
{
  if (!BLE.connected())
    GoDirectBLE_Start();
// I seem to get to here ok:
  Serial.println("D2PIO_StartMeasurements  ");
  Serial.println("g_channelNumber %%");
  Serial.println(g_channelNumber);
  if (!D2PIO_StartMeasurements(g_channelNumber))
    GoDirectBLE_Start();

  g_MeasurementCounter = 0;
  g_measurement = 0.0;

  Serial.print("***g_measurement2 "); // this is good!!!!
  Serial.println(g_measurement);
  Serial.print("***g_MeasurementCounter2");
  Serial.println(g_MeasurementCounter);
  g_MeasurementCounter++;
   

}//end function

//=============================================================================
// GoDirectBLE_Reset Function
//=============================================================================
void GDXLib::GoDirectBLE_Reset()  //
{
  // Cleanup any old connections
  if (BLE.connected())
    BLE.disconnect();
}

//=============================================================================
// GoDirectBLE_Read() Function***
//=============================================================================
void GDXLib::GoDirectBLE_Read()
{
  g_MeasurementCounter = 0;
  g_measurement = 0.0;
  // Make sure the sensor is still around
  if (!BLE.connected())
    GoDirectBLE_Start();

  bool data_read = 0;
  while (data_read == 0)
  {
  /*  THIS FAILS, SO I HACKED ALL OF THIS OUT
    //bool D2PIO_ReadMeasurement(byte buffer[], int timeout, float& measurement)
    if (D2PIO_ReadMeasurement(g_ReadBuffer, 5000, g_measurement))
    {
      data_read = 1;
      Serial.print("***g_measurement in loop "); // this is good!!!!
        Serial.println(g_measurement);
        Serial.print("***g_MeasurementCounter in loop");
        Serial.println(g_MeasurementCounter);
      
      //taken out 2/1/2020GoDirectBLE_Read()
      g_MeasurementCounter++;
    }// end if

    else
      Serial.println("***&x");
    delay(500);
        */
  }//end while
}

//=============================================================================
// GoDirectBLE_GetStatus() Function
//=============================================================================
void GoDirectBLE_GetStatus(char* strFirmwareVersion1, char* strFirmwareVersion2, byte& batteryPercent)
{
  sprintf(strFirmwareVersion1, "%d.%d", g_status.majorVersionMasterCPU, g_status.minorVersionMasterCPU);
  sprintf(strFirmwareVersion2, "%d.%d", g_status.majorVersionSlaveCPU,  g_status.minorVersionSlaveCPU);
  batteryPercent = g_status.batteryLevelPercent;

  batteryPercent = g_status.batteryLevelPercent;
  Serial.print("***in GoDirectBLE_GetStatus: batteryPercent ");
  Serial.println(batteryPercent);//this is correct here in the library code
}

//=============================================================================
// GoDirectBLE_GetScanRSSI() Function
//=============================================================================
byte GDXLib::GDXLib::GoDirectBLE_GetScanRSSI()
{
  return g_RSSIStrength;
}

//=============================================================================
// GoDirectBLE_GetDeviceName() Function
//=============================================================================
const char* GDXLib::GoDirectBLE_GetDeviceName()
{
  return g_peripheral.localName().c_str();
}

//=============================================================================
// GoDirectBLE_GetSerialNumber() Function
//=============================================================================
const char* GDXLib::GoDirectBLE_GetSerialNumber()
{
  static char strBuffer[32];
  strcpy(strBuffer, g_peripheral.localName().c_str());
  const char* pch = strtok(strBuffer, " ");
  pch = strtok (NULL, " ");
  return pch;
}

//=============================================================================
// GoDirectBLE_GetOrderCode() Function
//=============================================================================
const char* GDXLib::GoDirectBLE_GetOrderCode()
{
  static char strBuffer[32];
  strcpy(strBuffer, g_peripheral.localName().c_str());
  const char* pch = strtok(strBuffer, " ");
  return pch;
}

//=============================================================================
// GoDirectBLE_GetChannelUnits() Function
//=============================================================================
const char* GDXLib::GoDirectBLE_GetChannelUnits()
{
  return g_channelInfo.sensorUnit;
}

//=============================================================================
// GoDirectBLE_DisplayChannelAsInteger() Function
//=============================================================================
bool GDXLib::GoDirectBLE_DisplayChannelAsInteger()
{
  return (g_channelInfo.numericMeasType == 1);
}

//=============================================================================
// GoDirectBLE_GetChannelName() Function
//=============================================================================
char* GDXLib::GoDirectBLE_GetChannelName()
{
  return g_channelInfo.sensorDescription;
}

//=============================================================================
// GoDirectBLE_GetMeasurement() Function
//=============================================================================
float GDXLib::GoDirectBLE_GetMeasurement()
{
  return g_measurement;
}

//=============================================================================
// GoDirectBLE_End() Function
//=============================================================================
void GDXLib::GoDirectBLE_End()
{
  BLE.end();
}
//=============================================================================
// GoDirectBLE_Test() Function !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//=============================================================================
void GDXLib::GoDirectBLE_Test()
{
  Serial.println( "*** test() function at the bottom");
  Serial.print ("*** ");
  Serial.println(_c);
  _c=_c+1;
}
//=============================================================================
// GoDirectBLE_Test3() Function !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//=============================================================================
void GDXLib::GoDirectBLE_Test3()
{
  Serial.println( "*** test3 function ");
  Serial.print ("*** ");
  Serial.println(_c);
  _c=_c+77;
}