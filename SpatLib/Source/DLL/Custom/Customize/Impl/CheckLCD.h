#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CCheckLCD : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckLCD)
public:
    CCheckLCD(void);
    virtual ~CCheckLCD(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strLCD;
};