
#pragma once
#include <stdafx.h>
enum BBAT_MIC_CHANNEL
{
    Main_Mic = 0,
    HeadSet_Mic = 1,
    Sub_Mic = 2,
    Mid_Mic = 3,
    MAX_MIC_CHANNEL = 4
};

enum BBAT_PA_MIC_CHANNEL
{
    PA0,
    PA1
};

enum BBAT_SPK_CHANNEL
{
    Main_Speaker,
    Main_Receiver,
    Head_Set,
    Sub_Speaker,
    MAX_SPK_CHANNEL
};
enum alTestType
{
    PA_MIC,
    G3
};

enum FlashType
{
    FlashCold,      //[Phone]FLASHLIGHT_OPEN
    FlashWarm,      //[Phone]FLASHLIGHT_SECOND_OPEN
    FP_Flash,       //[Phone]FP_FLASHLIGHT_OPEN
    FrontFlash,     //[Phone]FRONT_FLASH_OPEN
    KeypadLight,    //[Phone]KEYPAD_BACKLIGHT_OPEN
    LedRed,         //[Phone]RGB_LED_RED
    LedGreen,       //[Phone]RGB_LED_GREEN
    LedBlue,        //[Phone]RGB_LED_BLUE
    Torch,
    GPIO,
    MAX_FLASH
};
enum Times
{
    FirstTime,
    SecondTime
};

enum LedType
{
    LED0,
    LED1,
    LED2,
    LED3,
    LED4,
    LED5,
    LED6,
    LED7,
    MAX_LED
};
struct AudioResult
{
    double dSNR = 0.0;
    double dPeak = 0.0;
};

enum AFCB_GPIO_NUMBER :BYTE
{
    AFCB_GPIO_A,
    AFCB_GPIO_B,
    AFCB_GPIO_C,
    AFCB_GPIO_D,
    AFCB_GPIO_IO,
    AFCB_MAX_GPIO_NUMBER
};

enum ChargeVoltage
{
    Charge_4V,
    Charge_12V
};

#define RETURNSPRESULT(statement, strTemp)        \
{                                                 \
     if( -1 == statement)                         \
         {                                        \
             PrintErrorMsg(SP_E_BBAT_CMD_FAIL, strTemp, LEVEL_ITEM);\
              return SP_E_BBAT_CMD_FAIL;          \
          }                                       \
     PrintSuccessMsg(SP_OK, strTemp, LEVEL_ITEM); \
}

#define RETURNSPRESULT_IMP(m_pImp, statement, strTemp)                     \
{                                                                          \
    if( -1 == statement)                                                   \
    {                                                                      \
        m_pImp->PrintErrorMsg(SP_E_BBAT_CMD_FAIL, strTemp, LEVEL_ITEM);    \
        return SP_E_BBAT_CMD_FAIL;                                         \
    }                                                                      \
    m_pImp->PrintSuccessMsg(SP_OK, strTemp, LEVEL_ITEM);                   \
}

#define RETURNRESULT_WITH_UISHOW(pImpbase, statement, bUiShow)    \
{                                                                 \
    if (-1 == statement)                                          \
        {                                                         \
            if(bUiShow)                                           \
                {                                                 \
                    std::string itemName = #statement;            \
                    itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");    \
                    pImpbase->NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                  \
                }                                                                                         \
        return SP_E_BBAT_CMD_FAIL;                                \
        }                                                         \
}

#define CHKFBOXRESULT(statement)    \
{                                   \
    if( -1 == statement)            \
        {                           \
            return statement;       \
         }                          \
}