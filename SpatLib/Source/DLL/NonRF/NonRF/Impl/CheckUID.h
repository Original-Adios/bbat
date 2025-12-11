#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckUID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckUID)
public:
    CCheckUID(void);
    virtual ~CCheckUID(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

    BOOL SaveToFile(const char* lpszUID);

private:
    std::wstring m_strUIDFile;
};
