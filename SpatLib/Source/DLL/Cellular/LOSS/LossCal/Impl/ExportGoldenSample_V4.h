#pragma once
#include "ExportGoldenSample.h"

class CExportGoldenSample_V4 :
    public CExportGoldenSample
{
    DECLARE_RUNTIME_CLASS(CExportGoldenSample_V4)
public:
	CExportGoldenSample_V4(void);
    virtual ~CExportGoldenSample_V4(void);

    virtual SPRESULT ExportLTE(LPCWSTR lpPath);
private:

    SPRESULT LteDeserialization(std::vector<uint8>& arrInfo);
    void GetLossFmtStrLTE(std::wstring& strFmt, BOOL bExp, BOOL bTX);
    std::vector<LTE_Loss_Data_V4> m_vecLteData_V4;

};
