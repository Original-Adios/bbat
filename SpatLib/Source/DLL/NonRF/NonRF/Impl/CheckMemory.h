#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckMemory : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckMemory)
public:
    CCheckMemory(void);
    virtual ~CCheckMemory(void);

private:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strMemory;
};
