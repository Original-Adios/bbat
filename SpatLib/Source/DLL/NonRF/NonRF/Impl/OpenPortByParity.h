#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class COpenPortByParity : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COpenPortByParity)
public:
	COpenPortByParity(void);
    virtual ~COpenPortByParity(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    RM_MODE_ENUM    m_eMode;
    BOOL       m_bAutoSetup;
	DWORD	   m_dwPortParity;
};
