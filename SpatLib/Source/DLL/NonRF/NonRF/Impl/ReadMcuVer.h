#pragma once
#include "ImpBase.h"
#include <string>


//////////////////////////////////////////////////////////////////////////
class CReadMcuVer : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadMcuVer)
public:
    CReadMcuVer(void);
    virtual ~CReadMcuVer(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT LoadMcuVer(char* szSwVer);

private:
    struct MCUVER_T
    {
        BOOL bLoad;
        BOOL bVerify;
        std::string strExpVer;

        MCUVER_T(void)
        {
            bLoad   = FALSE;
            bVerify = FALSE;
            strExpVer.clear();
        }
    } m_McuVer;
};
