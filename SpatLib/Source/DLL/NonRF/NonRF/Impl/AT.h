#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CAT : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAT)
public:
    CAT(void);
    virtual ~CAT(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

protected:
    std::string  m_strAT;
    std::string m_strRsp;
    uint32  m_u32TimeOut;
};
