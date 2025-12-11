#pragma once
#include "ImpBase.h"
#include "ModeSwitch.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
class CEnterMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CEnterMode)
public:
    CEnterMode(void);
    virtual ~CEnterMode(void);

protected:
    virtual BOOL     LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);

protected:
	CModeOptions  m_Options;
    BOOL    m_bLoadSN;
    UINT32  m_nMaxRetryCount;
    BOOL    m_bLogUsingOtherPort;
};
