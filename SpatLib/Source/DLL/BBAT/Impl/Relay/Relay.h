#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CRelay : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CRelay)
public:
    CRelay(void);
    virtual ~CRelay(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    DWORD m_dwUartNo;
    DWORD m_dwBaudrate;
    BOOL    m_bRelay1;
    BOOL    m_bRelay1_Open;
    BOOL    m_bRelay1_Close;
    BOOL    m_bRelay2;
    BOOL    m_bRelay2_Open;
    BOOL    m_bRelay2_Close;
	SPRESULT Send(ICommChannel* pCom,unsigned char* lpData);
	SPRESULT RecvAndCompare(ICommChannel* pCom,LPCSTR lpCompareData);
};


