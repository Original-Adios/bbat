#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////

class CGetOTP : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGetOTP)
public:
    CGetOTP(void);
    virtual ~CGetOTP(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
	std::string m_strServerIP;
	BOOL m_bTriad;
	char m_szDid[9];
	char m_szKey[17];
	char m_Uid[21];
	bool GetOTP(_pUNISOC_MES_CMN_CODES p_mes_codes, std::string &strMsg);
};
