
#ifndef GDXLib_h
#define GDXLib_h
#define GDXLIB_LIB_VERSION "0.1" // displayed automatically

/* This is a library to make using GDX sensors easy
*/
class GDXLib
{
public:
    GDXLib();
    
    void autoID();//this is the function for the autoID code
    

    void GoDirectBLE_Begin();
    char* channelUnits()   { return _channelUnits ;};
    /*
    // it returns calibration information
    int channel()       { return _channel; };
    float voltageID()   { return _voltageID; }; 
    int sensorNumber()  { return _sensorNumber;}; 
    // Jenny suggests this change:  
    char* sensorName()   { return _sensorName ;};
    char* shortName()    { return _shortName  ;};
    char* sensorUnits()  { return _sensorUnits;};
    float slope()        { return _slope; }; 
    float intercept()    { return _intercept; }; 
    float cFactor()      { return _cFactor; }; 
    int calEquationType(){ return _calEquationType; };
    int page()           { return _page; }; 

    float readSensor();//This function converts count to sensor reading
    float sensorReading()     { return _sensorReading; };

    void DCUPWM(int PWMSetting); //function to control PWM via the DCU, line 4 (Arduino line 9)

    void DCU (int DCUSetting); //function for controlling a DCU
  
    void DCUStep(int stepCount, int stepDirection, int stepDelay); //function for stepper motors

    float readMotionDetector();//function for Motion Detectors
    float distance()     { return _distance; };
*/
protected://  also known as local
    char _channelUnits[32];
    /*
    float _voltageID;
    int  _channel;
    int _sensorNumber;
    char _sensorName[16];
    char _shortName[12];// 11 char + terminator
    char _sensorUnits[7];
    float _slope;
    float _sum;
    float _intercept;
    float _cFactor;
    int _calEquationType;
    int _page;
    float _sensorReading;
    float _distance;
    // this misc. variable is used many places
    int _i;
    */
};
#endif

// END OF FILE
