#pragma once
#include "ImpBase.h"
#include <assert.h>
#include "ModeSwitch.h"

//////////////////////////////////////////////////////////////////////////
class CNormalModeToCalMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CNormalModeToCalMode)
public:
    CNormalModeToCalMode(void);
    virtual ~CNormalModeToCalMode(void);

protected:
    virtual BOOL     LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);


private:
	BOOL m_bNormalToCalMode;
	std::string m_strNormalToCal;
	std::string m_strCalToNormal;
	CModeOptions  m_Options;
};
