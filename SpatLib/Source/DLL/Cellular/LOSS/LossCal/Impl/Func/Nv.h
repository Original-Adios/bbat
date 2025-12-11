#pragma once
#include "FuncBase.h"
#include "FileConfig.h"
#include <vector>

class CNv :
    public CFuncBase
{
public:
    struct NvData_BandInfo
    {
        uint16 Band;
        uint16 Indicator;
        uint16 Compatible;
        uint16 TxDcEnable : 8;
        uint16 DivDisable : 8;
        uint16 RxCa;
        uint16 TxCa;
    };

private:
    struct NvData_DownloadChecksum
    {
        uint16 Checksum[4];
		NvData_DownloadChecksum()
		{
			memset(Checksum, 0, sizeof(Checksum));
		}
    };

    struct NvData_CalibrationVersion
    {
        uint16 Version;
    };

    struct Band_Info
    {
        LTE_BAND_E nBand;
        int nNumeral;
        int nIndicator;
    };
public:
    CNv(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNv(void);

    virtual SPRESULT PreInit();

    SPRESULT Load();
    void ApplyChecksum();

public:
    BOOL m_bSameDownloadChecksum;
    int m_nBandCount;

    std::vector<NvData_BandInfo*>        m_arrpDownload_BandInfo;
    std::vector<Band_Info> m_arrBandInfo;
    NvData_CalibrationVersion*  m_pCalibration_Version;

private:
    SPRESULT InsertBand(LTE_BAND_E Band);
    SPRESULT InitBandNv();
    SPRESULT InitBandNvV5();
    SPRESULT InitBandNvV6();
    void PrintCompatibleBand();

private:
    uint32 m_uVersion;
    SP_HANDLE  m_hDUT;

//     PC_LTE_NV_V3_DATA_T Download_BandInfo_V6;
//     PC_LTE_NV_V2_DATA_T Download_BandInfo_V5;
    PC_LTE_NV_DATA_T    m_LteNv_Download_Checksum;
    PC_LTE_NV_DATA_T    m_LteNv_Cali_Version;

    NvData_DownloadChecksum*        m_pDownload_Checksum;
    NvData_DownloadChecksum         m_Last_Download_Checksum;
    std::vector<NvData_BandInfo*>    m_arrpDownload_BandInfo_Complete;

    CFileConfig* m_pFileConfig;  

    std::vector<int> m_arrBandNv;
    std::vector<int> m_arrCompatibleBandNv;

    std::vector<PC_LTE_NV_V2_DATA_T> m_arrDownload_BandInfo_Complete_V2;
    std::vector<PC_LTE_NV_V3_DATA_T> m_arrDownload_BandInfo_Complete_V3;

    int m_nMaxBandCount;
};
