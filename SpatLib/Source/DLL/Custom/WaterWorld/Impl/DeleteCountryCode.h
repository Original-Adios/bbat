#pragma once
#include "ImpBase.h"
#include "PLink_InputBarCodes.h"


typedef enum
{
	COVERBYTIP,
	COVER,
	NOCOVER
}COVERTYPE;
//////////////////////////////////////////////////////////////////////////
class CDeleteCountryCode : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CDeleteCountryCode)
public:
	CDeleteCountryCode(void);
	virtual ~CDeleteCountryCode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
	SPRESULT ClearMiscData(void);
	SPRESULT CheckSnAllow(void);
	SPRESULT SaveInfo2CCDeleteRecord(void);
	SPRESULT CheckCountryCode(void);
private:
    std::string m_strSn;
    std::string m_strMo;
    std::string m_strCode;
	std::string m_strSoftwarePN;
};

