#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CGPIO_Read : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGPIO_Read)
public:
    CGPIO_Read(void);
    virtual ~CGPIO_Read(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	
	struct GPIOValue
	{
		int8 iOperate; // BBAT_GPIO
		int8 iGPIONumber;
	};

	enum BBAT_GPIO
	{
		Read = 0,
		Write = 1
	};
	  
	struct GPIOData
	{
		int8 GPIOVal;
	};



private:
    BOOL m_iGPIONo;
    BOOL m_iCompare_Value;
};


