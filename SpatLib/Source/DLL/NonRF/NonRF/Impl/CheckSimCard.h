#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
class CCheckSimCard : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckSimCard)
public:
    CCheckSimCard(void);
    virtual ~CCheckSimCard(void);

private:
    virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);

	SPRESULT CheckSimCard();

private:
	BOOL m_bCheckSimCard1;
	BOOL m_bCheckSimCard2;

	std::wstring m_strSimCard1CheckType;
	std::wstring m_strSimCard2CheckType;

	SPRESULT CheckSimNoInserted(
    std::string& strRsp, 
    uint32 u32RetryCount /* = 3 */, 
    uint32 u32Interval /* = 200 */,
    uint32 u32TimeOut /* = TIMEOUT_3S */
    );

};
