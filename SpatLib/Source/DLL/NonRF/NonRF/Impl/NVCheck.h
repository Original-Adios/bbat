#pragma once
#include "ImpBase.h"
#include "NVCheck/FuncBase.h"
#include <string>
#include <map>

class CNVCheck : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CNVCheck)
public:
    CNVCheck(void);
    virtual ~CNVCheck(void);

protected:
    virtual BOOL     LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);
    
private:
    CFuncBase* m_pFunc;
    std::wstring m_wstrType;     
};