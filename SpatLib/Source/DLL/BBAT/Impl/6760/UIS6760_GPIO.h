#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUIS6760_GPIO : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUIS6760_GPIO)
public:
    CUIS6760_GPIO(void);
    virtual ~CUIS6760_GPIO(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
	
	enum BBAT_GPIO
	{
		Read = 0,
		Write = 1
	};

	enum BBAT_GPIO_VALUE
	{
		PullDown = 0,
		PullUp = 1
	};

	enum  BBAT_GPIO_NO
	{
		Uart3_TXD = 1,
		LED_Mode = 36
	};


	struct GPIOWriteSend
	{
		int8 iOperate = BBAT_GPIO::Write;		// BBAT_GPIO
		int8 iGPIONumber = 0;	// BBAT_GPIO_NO
		int8 iGPIOValue = 0; // BBAT_GPIO_VALUE
	};

	struct GPIOReadSend
	{
		int8 iOperate = BBAT_GPIO::Read;		// BBAT_GPIO
		int8 iGPIONumber = 0;
	};

	struct GPIOReadRecv
	{
		int8 iGPIONumber;
	};


private:
    BOOL m_bUart_ADC;
    BOOL m_bLED;
    BOOL m_bPA_LNA;

	SPRESULT TestUart_ADC(void);
	SPRESULT TestLED(void);
	SPRESULT TestPA_LNA(void);
};

#define LED_MODE 36
