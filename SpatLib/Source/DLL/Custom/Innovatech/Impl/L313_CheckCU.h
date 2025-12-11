#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CL313_CheckCU : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CL313_CheckCU)
public:
    CL313_CheckCU(void);
    virtual ~CL313_CheckCU(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strTargetCU;
	BOOL m_bCheckCU;
	BYTE m_byGetCuFromMes;
};