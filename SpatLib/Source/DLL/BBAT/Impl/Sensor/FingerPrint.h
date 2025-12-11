#pragma once
#include "../ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CFingerPrint : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CFingerPrint)
public:
	CFingerPrint(void);
	virtual ~CFingerPrint(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:

};