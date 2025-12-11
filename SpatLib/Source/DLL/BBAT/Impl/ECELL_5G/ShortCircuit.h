#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CShortCircuit : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CShortCircuit)
public:
    CShortCircuit(void);
    virtual ~CShortCircuit(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	
typedef enum  _DEVICE_AUTOTEST_ID_E
{
    DEVICE_AUTOTEST_SET = 0,   
    DEVICE_AUTOTEST_KEYPAD = 1,     
    DEVICE_AUTOTEST_LCD_PARALLEL = 2,  
    DEVICE_AUTOTEST_LCD_SPI = 3,  
    DEVICE_AUTOTEST_CAMERA_IIC = 4,  
    DEVICE_AUTOTEST_CAMERA_PARALLEL = 5,  
    DEVICE_AUTOTEST_CAMERA_SPI = 6,  
    DEVICE_AUTOTEST_GPIO =7,  //and TP test     
    DEVICE_AUTOTEST_TF = 8, 
    DEVICE_AUTOTEST_SIM = 9,
    DEVICE_AUTOTEST_MIC = 10,
    DEVICE_AUTOTEST_SPEAK = 11, //speak&&receiver&&earphone
    DEVICE_AUTOTEST_MISC = 12,    //lcd backlight,vibrator,keypadbacklight   
    DEVICE_AUTOTEST_FM = 13,
    DEVICE_AUTOTEST_ATV = 14, 
    DEVICE_AUTOTEST_BT = 15,
    DEVICE_AUTOTEST_WIFI = 16,     

    DEVICE_AUTOTEST_IIC_DEV= 17, //speak&&receiver&&earphone

    DEVICE_AUTOTEST_CHARGE= 18,

    DEVICE_AUTOTEST_RESERVE_1= 19,//read BBAT Info
    DEVICE_AUTOTEST_RESERVE_2= 20,//write BBAT Info
    DEVICE_AUTOTEST_SENSOR= 21,//Sensor
    DEVICE_AUTOTEST_GPS= 22,//
    DEVICE_AUTOTEST_RESERVE_5= 23,
    DEVICE_AUTOTEST_RESERVE_6= 24,
    DEVICE_AUTOTEST_RESERVE_7= 25,
    DEVICE_AUTOTEST_RESERVE_8= 26,
    DEVICE_AUTOTEST_RESERVE_9= 27,
    DEVICE_AUTOTEST_RESERVE_10 =28,
    DEVICE_AUTOTEST_MAX_F
}DEVICE_AUTOTEST_ID_E;


private:
};


