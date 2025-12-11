#pragma once
#include "ImpBase.h"
#include <string>

//////////////////////////////////////////////////////////////////////////
class CAddTextTestInfo : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CAddTextTestInfo)
public:
    CAddTextTestInfo(void);
    virtual ~CAddTextTestInfo(void);

protected:
    virtual SPRESULT __PollAction (void);
    virtual BOOL LoadXMLConfig(void);

private:
	std::string m_strStationName;
	std::string m_strFilePath;
};
