#pragma once
#include "ImpBase.h"
#include <assert.h>


//////////////////////////////////////////////////////////////////////////
class CCustomControl : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCustomControl)
public:
    CCustomControl(void);
    virtual ~CCustomControl(void);

protected:
    virtual BOOL     LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);

private:
    BOOL    m_bMessageBox;
    string  m_strMsgBoxText;
    string  m_strMsgBoxCap;

};
