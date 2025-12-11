#pragma once
#include "ImpBase.h"

/*
    Bug1189339:
    Set AP SSID and PSK into miscdata

    Stored in miscdata offset 0x2800

    adb shell  cat /dev/block/by-name/miscdata > misc.bin
*/
//////////////////////////////////////////////////////////////////////////
class CSetSSID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSetSSID)
public:
    CSetSSID(void);
    virtual ~CSetSSID(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strSSID;
    std::string m_strPSK;
};
