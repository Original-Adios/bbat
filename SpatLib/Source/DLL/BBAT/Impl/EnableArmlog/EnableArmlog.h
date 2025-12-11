#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CEnableArmlog : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CEnableArmlog)
public:
	CEnableArmlog(void);
	virtual ~CEnableArmlog(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:

};