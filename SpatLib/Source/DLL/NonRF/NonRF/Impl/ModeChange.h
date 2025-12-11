#pragma once
#include "EnterMode.h"

//////////////////////////////////////////////////////////////////////////
class CModeChange : public CEnterMode
{
    DECLARE_RUNTIME_CLASS(CModeChange)
public:
    CModeChange(void);
    virtual ~CModeChange(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
