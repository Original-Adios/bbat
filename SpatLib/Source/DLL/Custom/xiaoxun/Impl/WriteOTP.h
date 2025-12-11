#pragma once
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CWriteOTP : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteOTP)
public:
    CWriteOTP(void);
    virtual ~CWriteOTP(void);

protected:
    virtual SPRESULT __PollAction(void);
private:
	bool MacPass(char *pIp, char *pWifiMac);
};
