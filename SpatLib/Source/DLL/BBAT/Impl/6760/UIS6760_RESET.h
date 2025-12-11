#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CUIS6760_RESET : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CUIS6760_RESET)
public:
    CUIS6760_RESET(void);
    virtual ~CUIS6760_RESET(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    DWORD m_dwUartNo;
    DWORD m_dwBaudrate;
	SPRESULT Send(ICommChannel* pCom,unsigned char* lpData);
	SPRESULT RecvAndCompare(ICommChannel* pCom,LPCSTR lpCompareData);
};


