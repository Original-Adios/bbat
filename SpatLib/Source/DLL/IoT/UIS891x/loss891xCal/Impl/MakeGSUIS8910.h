#pragma once
#include "ImpBaseUIS8910.h"
#include "ILossFunc_UIS8910.h"

class CMakeGSUIS8910 :
    public CImpBaseUIS8910
{
    DECLARE_RUNTIME_CLASS(CMakeGSUIS8910)
public:
    CMakeGSUIS8910(void);
    virtual ~CMakeGSUIS8910(void);
    virtual SPRESULT   __InitAction(void);
    virtual SPRESULT   __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);
    SPRESULT LoadWifiUeInfo(void);
    int m_wifi_UeInfo[20];
};
