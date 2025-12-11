#pragma once
#include "../ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
//////////////////////////////////////////////////////////////////////////

class CCheckWcnUid : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckWcnUid)
public:
	CCheckWcnUid(void);
    virtual ~CCheckWcnUid(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    SPRESULT LeaveBTMode(void);
	std::string m_strWcnUid;
};
