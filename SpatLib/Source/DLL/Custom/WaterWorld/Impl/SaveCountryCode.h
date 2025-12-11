#pragma once
#include "ImpBase.h"
#include "PLink_InputBarCodes.h"

//////////////////////////////////////////////////////////////////////////
class CSaveCountryCode : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CSaveCountryCode)
public:
	CSaveCountryCode(void);
	virtual ~CSaveCountryCode(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
	SPRESULT SaveMiscData(void);
	SPRESULT CheckSnAllow(BarCode_T* pBarcodes);
	SPRESULT CheckPhoneModel(void);
	SPRESULT SaveInfo2TpCountryCode(void);
	SPRESULT CheckVoltage(void);
	SPRESULT CheckResetFlag(void);
private:
    std::string m_strSn;
    std::string m_strSmo;
    std::string m_strCode;
    std::string m_strDetailModel;
	std::string m_strSoftwarePN;
	UINT m_ckBatery;
	double m_dUpVoltage;
	double m_dDownVoltage;
};

