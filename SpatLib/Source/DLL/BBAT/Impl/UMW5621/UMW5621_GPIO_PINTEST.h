#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CUMW5621_GPIO_PINTEST : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUMW5621_GPIO_PINTEST)
public:
    CUMW5621_GPIO_PINTEST(void);
    virtual ~CUMW5621_GPIO_PINTEST(void);
	

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	
};


