#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUMP5865_GPIO_GetLevel : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUMP5865_GPIO_GetLevel)
public:
    CUMP5865_GPIO_GetLevel(void);
    virtual ~CUMP5865_GPIO_GetLevel(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	
	struct GPIOReadValue
	{
		int8 iOperate;
		// BBAT_GPIO
		int8 iGPIONumber;
	};

	struct GPIOData
	{
		int8 GPIOVal;
	};



private:
    int m_iGPIONo;
    int m_iCompare_Value;
};

#define GPIO_READ 0
