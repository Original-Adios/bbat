#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class COpenSocket : public CImpBase
{
	DECLARE_RUNTIME_CLASS(COpenSocket)
public:
	COpenSocket(void);
	virtual ~COpenSocket(void);

protected:
	virtual BOOL     LoadXMLConfig(void);
	virtual SPRESULT __PollAction (void);


protected:
	DWORD m_dwPort;
	std::string m_strIP;
	BOOL m_bPing;
	int m_nPingTimeOut;
	int m_nPingCount;
};