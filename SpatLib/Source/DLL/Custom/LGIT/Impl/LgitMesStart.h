#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CLgitMesStart : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLgitMesStart)
public:
    CLgitMesStart(void);
    virtual ~CLgitMesStart(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    DWORD m_dwPort;
    std::string m_strIP;
};
