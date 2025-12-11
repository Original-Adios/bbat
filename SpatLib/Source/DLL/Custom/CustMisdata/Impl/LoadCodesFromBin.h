#pragma once
#include "ImpBase.h"
#include <string>

#define MAX_MDB_COL_NUM 25
#define ShareMemory_My_BIN_FILE                     L"My_7403AD14-C3AF-4159-9460-EB1CE78C8CAE_BINFILE"


enum CODE_INDEX
{
	CODE_NONE  = -1,
	CODE_START =  0,
	CODE_BT    = BC_START,
	CODE_WIFI,
	CODE_IMEI1,
	CODE_IMEI2,
	CODE_IMEI3,
	CODE_IMEI4,
	CODE_SN1,
	CODE_SN2,
	CODE_MEID1,
	CODE_MEID2,
	CODE_ENETMAC,
	CODE_CUST_1,
	CODE_CUST_2,
	CODE_CUST_3,
	CODE_CUST_4,
	CODE_CUST_5,
	CODE_CUST_6,
	CODE_CUST_7,
	CODE_CUST_8,
	CODE_CUST_9,
	CODE_CUST_10,
	CODE_CUST_11,
	CODE_CUST_12,
	CODE_MAX_CUSTMIZENUM
};

//////////////////////////////////////////////////////////////////////////
class CLoadCodesFromBin : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadCodesFromBin)
public:
    CLoadCodesFromBin(void);
    virtual ~CLoadCodesFromBin(void);
	SPRESULT GetCodeFromBin(LPCWSTR pPath);
	SPRESULT LookupCode(LPCWSTR lpckey, INPUT_CODES_T * pInputCode);
	BOOL CheckIMEIValid(LPCSTR lpIMEI);
	__int64 ComputeCD(__int64 nImei);
protected:
	virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction (void);
	std::wstring m_strBinPath;
	std::wstring m_strBinData;
	bool m_bReadFile;
};
