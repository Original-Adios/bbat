#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CRGB_ADC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CRGB_ADC)
public:
    CRGB_ADC(void);
    virtual ~CRGB_ADC(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
	
	struct MiscValue
	{
		int8 iMiscIndex; // BBAT_MISC
		int8 Color;
		//BBAT_RGB_COLOR
		int8 Operate;
		//BBAT_RGB_OPERATE
	};

	enum BBAT_MISC
	{
		LCDBackLight = 0,
		Reserved = 1,
		KeypadBackLight = 2,
		Vibrator = 3,
		FlashLight = 4,
		OTG = 5,
		HeadSetDetect = 6,
		COB = 7,
		FrontFlashLight = 8,
		RGBLight = 9,
		NTC = 10,
		Torch = 11,
		FingerPrint = 12,
		NFC = 13,
		RGBShine = 33		//ÈýÉ«µÆÒÀ´ÎÁÁÃð
	};

	enum BBAT_RGB_COLOR
	{
		RED,
		GREEN,
		BLUE
	};
	enum BBAT_RGB_OPERATE
	{
		CLOSE,
		OPEN
	};

	enum BBAT_GPIO
	{
		Read = 0,
		Write = 1
	};
	struct GPIOWriteValue
	{
		int8 iOperate = BBAT_GPIO::Write;
		// BBAT_GPIO
        int8 iGPIONumber = 0;
        int8 iGPIOValue = 0; // BBAT_GPIO_VALUE
	};
	enum BBAT_GPIO_VALUE
	{
		PullDown = 0,
		PullUp = 1
	};
	




private:
	BOOL    m_bRGB[3];
	uint8 ADC_Channel[3];
    int Upspec[3];
    int Downspec[3];

	//add keyout1	
	BOOL    m_bKeyOut1;
    int		m_iDeltaSpec;

	SPRESULT Send(ICommChannel* pCom,LPCSTR lpData);
	SPRESULT RecvAndCompare(ICommChannel* pCom,LPCSTR lpCompareData);
	SPRESULT RGBOperate(int iColor, int iOperate, const char* iPrintMsg);
	SPRESULT GetADC(uint8 iChannel);
	SPRESULT GPIOWrite(int iGPIOValue,const char* iPrintMsg);
};


