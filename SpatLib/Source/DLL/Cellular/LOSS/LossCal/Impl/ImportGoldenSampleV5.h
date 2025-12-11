#pragma once
#include "ImpBase.h"
#include "ImportGoldenSample.h"

class CImportGoldenSampleV5 :
    public CImportGoldenSample
{
    DECLARE_RUNTIME_CLASS(CImportGoldenSampleV5)
public:
	CImportGoldenSampleV5(void);
    virtual ~CImportGoldenSampleV5(void);

    virtual SPRESULT ImportLTE(LPCWSTR lpPath);
	

private:
    SPRESULT LteSerializationV4(std::vector<uint8>& arrInfo);
    void GetLossFmtStrLTE(std::wstring& strFmt, BOOL bExp, BOOL bTX);
    std::vector<LTE_Loss_Data_V5> m_vecLteData;
};
