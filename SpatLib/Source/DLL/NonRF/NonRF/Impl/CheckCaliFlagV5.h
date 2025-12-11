#pragma once
#include "CheckCaliFlagV3.h"


//////////////////////////////////////////////////////////////////////////
class CCheckCaliFlagV5 : public CCheckCaliFlagV3
{
	DECLARE_RUNTIME_CLASS(CCheckCaliFlagV5)
public:
	CCheckCaliFlagV5(void);
	virtual ~CCheckCaliFlagV5(void);
private:
	virtual SPRESULT RunAfcCheckFlag(BOOL& bCheckAllPass);
	virtual SPRESULT RunLteCheckFlag(BOOL& bCheckAllPass);
	virtual SPRESULT RunNrCheckFlag(BOOL& bCheckAllPass);

};
