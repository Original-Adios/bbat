#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CLoadWCNID : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CLoadWCNID)
public:
	CLoadWCNID(void);
	virtual ~CLoadWCNID(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

private:
	std::string m_strCode;
};
