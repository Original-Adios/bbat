#pragma once
#include "ImpBase.h"
#include "../../../IoT/UIS891x/loss891xCal/Impl/ComDef.h"
//////////////////////////////////////////////////////////////////////////
class CCheckGoldenSampleUIS8910 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckGoldenSampleUIS8910)
public:
    CCheckGoldenSampleUIS8910(void);
    virtual ~CCheckGoldenSampleUIS8910(void);

private:
    SPRESULT __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);
private:
    GS_MODULE_GS10 m_gs;
    PC_LTE_NV_UIS8910_DATA_T g_nvTransBuf;
    char    m_szGSBuf[MAX_GOLDEN_SAMPLE_SIZE];    // 4*1024(4K)
    int m_Uetype;
    BOOL m_bGoldenPass;
    int m_UeBootUpWaitTime;
};