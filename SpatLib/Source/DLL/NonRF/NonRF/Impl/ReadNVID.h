#pragma once
#include "ImpBase.h"

class CReadNVID : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CReadNVID)
public:
	CReadNVID();
	virtual ~CReadNVID();

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	uint16 m_arrID[2048];
	INT m_nLength;
};

