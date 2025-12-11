#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUMP5865_GPIO_SetLevel : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUMP5865_GPIO_SetLevel)
public:
    CUMP5865_GPIO_SetLevel(void);
    virtual ~CUMP5865_GPIO_SetLevel(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	
	struct GPIOWriteValue
	{
		int8 iOperate;  // = BBAT_GPIO::WRITE;
		// BBAT_GPIO
		int8 iGPIONumber;
		int8 iGPIOValue; // BBAT_GPIO_VALUE
	};	enum BBAT_GPIO_VALUE
	{
		PullDown = 0,
		PullUp = 1
	};
	enum BBAT_GPIO
	{
		Read = 0,
		Write = 1
	};


private:
    BOOL m_iGPIONo;
    int m_iLevel;
};


