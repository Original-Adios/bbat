#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CL313_WriteCU : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CL313_WriteCU)
public:
    CL313_WriteCU(void);
    virtual ~CL313_WriteCU(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strCU;
	BYTE m_byGetCuFromMes;
};