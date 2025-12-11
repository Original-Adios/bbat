#pragma once
#include "ImpBase.h"
#include "ImportGoldenSample.h"

class CImportGoldenSampleV4 :
    public CImportGoldenSample
{
    DECLARE_RUNTIME_CLASS(CImportGoldenSampleV4)
public:
	CImportGoldenSampleV4(void);
    virtual ~CImportGoldenSampleV4(void);

    virtual SPRESULT ImportLTE(LPCWSTR lpPath);
	

private:
    SPRESULT LteSerializationV4(std::vector<uint8>& arrInfo);
    void GetLossFmtStrLTE(std::wstring& strFmt, BOOL bExp, BOOL bTX);
    std::vector<LTE_Loss_Data_V4> m_vecLteData;
};
