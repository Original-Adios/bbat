#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CSIMCARD : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CSIMCARD)
public:
    CSIMCARD(void);
    virtual ~CSIMCARD(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
	BOOL    m_bSIMCard1 = 0;
	BOOL    m_bSIMCard2 = 0;
	BOOL    m_bSIMCard3 = 0;
	BOOL    m_bSIMCard4 = 0;
	BOOL    m_bTCard1 = 0;
	BOOL    m_bTCard2 = 0;
	BOOL    m_bFpSimCard1 = 0;
	BOOL    m_bFpSimCard2 = 0;

	SPRESULT RunSimCard(BBAT_SimCard_VALUE_BYTE SimCardId);
	SPRESULT RunTCard(BBAT_TCard_VALUE_BYTE TCardId);
	SPRESULT RunFpSimCard(BBAT_SimCard_VALUE_BYTE SimCardId);
};


