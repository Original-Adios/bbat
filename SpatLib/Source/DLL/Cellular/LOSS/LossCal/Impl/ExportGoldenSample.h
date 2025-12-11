#pragma once
#include "ImpBase.h"
#include "CommonDefine.h"

class CExportGoldenSample :
    public CImpBase
{
    DECLARE_RUNTIME_CLASS(CExportGoldenSample)
public:
    CExportGoldenSample(void);
    virtual ~CExportGoldenSample(void);
    virtual SPRESULT   __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);
	
private:
	SPRESULT ExportGsm(LPCWSTR lpPath);
	SPRESULT ExportTds(LPCWSTR lpPath);
	SPRESULT ExportWcdma(LPCWSTR lpPath);
	SPRESULT ExportC2K(LPCWSTR lpPath);
	virtual SPRESULT ExportLTE(LPCWSTR lpPath);
	SPRESULT ExportNR(LPCWSTR lpPath);
	SPRESULT ExportWlan(LPCWSTR lpPath);
	SPRESULT ExportBT(LPCWSTR lpPath);
	SPRESULT ExportGPS(LPCWSTR lpPath);


    SPRESULT LteDeserialization(std::vector<uint8> &arrInfo);
	SPRESULT NrDeserialization(std::vector<uint8>* parrData);

private:
	std::wstring m_strFilePath;
	std::vector<LTE_Loss_Data> m_vecLteData;
	std::vector<GsData> m_vecNrData;
};
