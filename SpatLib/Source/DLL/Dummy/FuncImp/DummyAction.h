#pragma once
#include "SpatBase.h"

//////////////////////////////////////////////////////////////////////////
class CDummyAction : public CSpatBase
{
    DECLARE_RUNTIME_CLASS(CDummyAction)

public:
    CDummyAction(void);
    virtual ~CDummyAction(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    BOOL m_bAssertDUT;
    BOOL m_bUID;
    BOOL m_bSN1;
    BOOL m_bSN2;
    BOOL m_bCPSwVer;
    BOOL m_bAPSwVer;
    BOOL m_bIMEI1;
    BOOL m_bIMEI2;
    BOOL m_bBT;
    BOOL m_bWIFI;
    UINT m_nItemCount;
    std::string m_strSN1;
};
