#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CfnMesStart : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CfnMesStart)
public:
    CfnMesStart(void);
    virtual ~CfnMesStart(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

	KIDOMES_T m_kidoMes;
};
