#pragma once
#include "ImpBase.h"
#include <string>


//////////////////////////////////////////////////////////////////////////
class CReadHwVer : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadHwVer)
public:
    CReadHwVer(void);
    virtual ~CReadHwVer(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    struct SWVER_T
    {
        BOOL bLoad;
        BOOL bVerify;
        uint32 uExpVer;

        SWVER_T(void)
        {
            bLoad   = FALSE;
            bVerify = FALSE;
            uExpVer = 0;
        }
    } m_HwVer;
};
