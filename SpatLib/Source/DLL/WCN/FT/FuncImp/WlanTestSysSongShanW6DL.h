#pragma once
#include "WlanTestSysSongShanW6.h"
using namespace std;
#include "WlanAntSwitch.h"

class CWlanTestSysSongShanW6DL : public CWlanTestSysSongShanW6
{
	 DECLARE_RUNTIME_CLASS(CWlanTestSysSongShanW6DL)
protected:
	CWlanTestSysSongShanW6DL(void);//
    virtual ~CWlanTestSysSongShanW6DL(void);

	virtual SPRESULT __InitAction (void);	
};
