#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckMemoryRange : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckMemoryRange)
public:
    CCheckMemoryRange(void);
    virtual ~CCheckMemoryRange(void);

private:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    int m_nDDR_UL;
	int m_nDDR_DL;
	int m_nEMMC_UL;
	int m_nEMMC_DL;
};
