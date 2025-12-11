#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckUsageCount : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckUsageCount)
public:
    CCheckUsageCount(void);
    virtual ~CCheckUsageCount(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    enum
    {
        CABLEPIN = 0,
        FAKRA,
        JIGCOUNT,
        MAX_USAGE
    };

    string_t m_strFilePath;
    static LPCSTR USAGE_INFO[MAX_USAGE];
};
