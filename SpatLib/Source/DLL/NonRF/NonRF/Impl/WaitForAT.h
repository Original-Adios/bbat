#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CWaitForAT : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWaitForAT)
public:
    CWaitForAT(void);
    virtual ~CWaitForAT(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

protected:
    std::string  m_strAT;
    std::string m_strRsp;
    uint32  m_u32TimeOut;
	uint32  m_u2ToToalTimeOut;
};
