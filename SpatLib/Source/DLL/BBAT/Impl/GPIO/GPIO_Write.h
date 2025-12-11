#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CGPIO_Write : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGPIO_Write)
public:
    CGPIO_Write(void);
    virtual ~CGPIO_Write(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	struct GPIOWriteValue
	{
		int8 iOperate;  // = BBAT_GPIO::WRITE;
		// BBAT_GPIO
		int8 iGPIONumber;
		int8 iGPIOValue; // BBAT_GPIO_VALUE
	};

	enum BBAT_GPIO_VALUE
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
    int m_iGPIONo;
    int m_iLevel;
};


