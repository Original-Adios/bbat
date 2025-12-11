#pragma once
#include "../ImpBase.h"

using namespace std;

class CLogEnable : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CLogEnable)
protected:
	CLogEnable(void);
    virtual ~CLogEnable(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);
private:
	BOOL EnableWcnLogCmd(BOOL bOn);
	BOOL SendCommandAndRecvResponse( 
		const char* lpszCmd, 
		char* lpszBuf /* OUT */, 
		unsigned long nBufSize,
		unsigned long ulTimeOut /* = TIMEOUT_3S */, 
		unsigned int nRepeatCount  = 3 
		);

private:
	BOOL m_bArmEn;
	BOOL m_bCp2En;

	char  m_szRecvBuf[2048];
	char  m_szCmd[2048];

};
