#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
class CReadModemVersion : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadModemVersion)
public:
    CReadModemVersion(void);
    virtual ~CReadModemVersion(void);

protected:
    virtual SPRESULT __PollAction (void);
    virtual BOOL     LoadXMLConfig(void);

private:
    uint16 m_u16ModemVersion;
};
