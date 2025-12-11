#pragma once
#include "ImpBase.h"
#include <string>


//////////////////////////////////////////////////////////////////////////
class CReadSwVer : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadSwVer)
public:
    CReadSwVer(void);
    virtual ~CReadSwVer(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    struct SWVER_T
    {
        BOOL bLoad;
        BOOL bVerify;
        std::string strExpVer;

        SWVER_T(void)
        {
            bLoad   = FALSE;
            bVerify = FALSE;
            strExpVer.clear();
        }
    } m_CP, m_AP;
};
