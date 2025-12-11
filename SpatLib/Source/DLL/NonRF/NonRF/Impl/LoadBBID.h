#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CLoadBBID : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CLoadBBID)
public:
	CLoadBBID(void);
	virtual ~CLoadBBID(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

private:
	std::string m_strCode;
};
