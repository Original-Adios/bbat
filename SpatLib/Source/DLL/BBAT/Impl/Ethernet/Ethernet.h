#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CEthernet : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CEthernet)
public:
    CEthernet(void);
    virtual ~CEthernet(void);

protected:
    enum ETHERNET_TYPE
    {
        RGMII = 1,
        USXGMII = 2
    };

    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT EthernetLoopTest_RGMII(void);
    SPRESULT EthernetLoopTest_USXGMII(void);

private:
    int m_nRGMII = 0;
    int m_nUSXGMII = 0;
};