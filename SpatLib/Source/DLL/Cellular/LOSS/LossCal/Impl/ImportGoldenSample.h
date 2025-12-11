#pragma once
#include "ImpBase.h"
#include "CommonDefine.h"

class CImportGoldenSample :
    public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImportGoldenSample)
public:
    CImportGoldenSample(void);
    virtual ~CImportGoldenSample(void);
    virtual SPRESULT   __PollAction(void);
    virtual BOOL       LoadXMLConfig(void);

private:
	SPRESULT ImportGsm(LPCWSTR lpPath);
	SPRESULT ImportTds(LPCWSTR lpPath);
	SPRESULT ImportWcdma(LPCWSTR lpPath);
	SPRESULT ImportC2K(LPCWSTR lpPath);
	virtual SPRESULT ImportLTE(LPCWSTR lpPath);
	SPRESULT ImportNR(LPCWSTR lpPath);
	SPRESULT ImportWlan(LPCWSTR lpPath);
	SPRESULT ImportBT(LPCWSTR lpPath);
	SPRESULT ImportGPS(LPCWSTR lpPath);

	SPRESULT LteSerialization(std::vector<uint8> &arrInfo);
	SPRESULT NrSerialization(std::vector<uint8> *parrData);

private:
	std::wstring m_strFilePath;
	std::vector<LTE_Loss_Data> m_vecLteData;
	std::vector<GsData> m_vecNrData;
};
