#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckCU : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCU)
public:
    CCheckCU(void);
    virtual ~CCheckCU(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strTargetCU;
	BOOL m_bCheckCU;
};