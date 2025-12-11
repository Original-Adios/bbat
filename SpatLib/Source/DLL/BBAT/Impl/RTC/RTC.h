#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CRTC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CRTC)
public:
    CRTC(void);
    virtual ~CRTC(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

	SPRESULT RunOpen();
	SPRESULT RunRead();
	SPRESULT RunClose();


};
