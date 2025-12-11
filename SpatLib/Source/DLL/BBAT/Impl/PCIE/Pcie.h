#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CPcie : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CPcie)
public:
    CPcie(void);
    virtual ~CPcie(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);


private:
	int m_nPcie2;
	int m_nPcie3;
};