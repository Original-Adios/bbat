#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CSaveUEInfo : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSaveUEInfo)
public:
    CSaveUEInfo(void);
    virtual ~CSaveUEInfo(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    string_t m_strFilePath;
};
