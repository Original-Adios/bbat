#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CLoadZCPropVersion : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadZCPropVersion)
public:
    CLoadZCPropVersion(void);
    virtual ~CLoadZCPropVersion(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strVersion;
    std::string m_strVerDate;
	BOOL m_bCompareVer;
};