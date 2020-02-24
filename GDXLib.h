#ifndef GDXLib_h
#define GDXLib_h
//define GDXLIB_LIB_VERSION "0.1" // displayed automatically
// This is a library to make using GDX sensors easy
#include "ArduinoBLE.h"

class GDXLib {
 public:
    GDXLib();//definition of the GDXLib class
    void GoDirectBLE_Begin();
    void GoDirectBLE_Begin(char* deviceName, byte channelNumber, unsigned long samplePeriodInMilliseconds);
    void GoDirectBLE_BeginL(byte channelNumber, unsigned long samplePeriodInMilliseconds);//a public method
    float readSensor();//a public method
    char strBuffer[64]; //used in Read Sensor
    static byte g_ReadBuffer[256];////used in Read Sensor
    void GoDirectBLE_Test();// should not need to be public
    
 private:// also known as local  
    char* _deviceName;  // used in begin
    byte _channelNumber;// used in begin
    unsigned long _samplePeriodInMilliseconds;// used in begin
    
    int _a;//used in BeginL
    int _b;//used in BeginL
    int _c;//used in BeginL
    char _strBuffer[64]; //used in Read Sensor
    
    bool DumpGatttService(BLEDevice peripheral, char* uuid);
    bool D2PIO_DiscoverService(BLEDevice peripheral);
    byte D2PIO_CalculateChecksum(const byte buffer[]);
    void D2PIO_Dump(const char* strPrefix, const byte buffer[]);
    bool D2PIO_Write(const byte buffer[]);
    bool D2PIO_ReadBlocking(byte buffer[], int timeout);
    bool D2PIO_ReadMeasurement(byte buffer[], int timeout, float& measurement);
    bool D2PIO_Init();
    bool D2PIO_SetMeasurementPeriod(unsigned long measurementPeriodInMilliseconds);
    bool D2PIO_GetAvailableChannels(unsigned long& availableMask);
    bool D2PIO_GetDefaultChannels(unsigned long& defaultMask);
    bool D2PIO_GetStatus();
    bool D2PIO_GetDeviceInfo();
    bool D2PIO_GetChannelInfo(byte channelNumber, bool verbose);
    bool D2PIO_GetChannelInfoAll();
    bool D2PIO_Autoset();
    bool D2PIO_StartMeasurements(byte channelNumber);
    
    
    void GoDirectBLE_Measure();  //
    //void GoDirectBLE_Start();

    void GoDirectBLE_Reset();  //
    void GoDirectBLE_Read();
    void GoDirectBLE_GetStatus();
    byte GoDirectBLE_GetScanRSSI();
    const char* GoDirectBLE_GetDeviceName();
    const char* GoDirectBLE_GetSerialNumber();
    const char* GoDirectBLE_GetOrderCode();
    const char* GoDirectBLE_GetChannelUnits();
    bool GoDirectBLE_DisplayChannelAsInteger();
    char* GoDirectBLE_GetChannelName();
    float GoDirectBLE_GetMeasurement();
    void GoDirectBLE_End();
    //void GoDirectBLE_Test();
};
#endif
