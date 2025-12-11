#pragma once
#include "ImpBase.h"

/*
    Bug1189339:
    Setup connection mode between PC and DUT
    0: USB
    1: fixed AP by WIFI

    Stored in miscdata offset 0x2424

    adb shell  cat /dev/block/by-name/miscdata > misc.bin
*/
//////////////////////////////////////////////////////////////////////////
class CSetupConnMode : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSetupConnMode)
public:
    CSetupConnMode(void);
    virtual ~CSetupConnMode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    INT m_nMode;
    std::wstring m_strMode;
};
