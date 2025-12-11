#pragma once
#include "FuncBase.h"
#include "FileConfig_UIS8910.h"
#include "UIS8910NV.h"
#include <vector>

typedef struct
{
    INT16 min_gain_lower;
    INT16 min_gain_upper;
    INT16 max_gain_lower;
    INT16 max_gain_upper;
}T_UIS8850_AGC_TAB_GAIN_INFO;

typedef struct Tag_UIS8850_BAND_NV
{
    T_UIS8910_BAND_INFO band_info[UIS8910_BAND_NUM];
    T_UIS8910_APT_INFO apt_info[UIS8910_BAND_NUM];
    T_UIS8850_AGC_TAB_GAIN_INFO agc_tab_gaib_info[UIS8910_BAND_NUM];
    unsigned short agc_tab_gaib_hml_index_info[3];
    unsigned short reserved[3];
    Tag_UIS8850_BAND_NV()
    {
        ZeroMemory(band_info, UIS8910_BAND_NUM * sizeof(T_UIS8910_BAND_INFO));
        ZeroMemory(apt_info, UIS8910_BAND_NUM * sizeof(T_UIS8910_APT_INFO));
        ZeroMemory(agc_tab_gaib_info, UIS8910_BAND_NUM * sizeof(T_UIS8850_AGC_TAB_GAIN_INFO));
        ZeroMemory(agc_tab_gaib_hml_index_info, 3 * sizeof(unsigned short));
        ZeroMemory(reserved, 3 * sizeof(unsigned short));
    }
}T_UIS8850_BAND_NV;

class CNv_UIS8910 :
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
    CNv_UIS8910(LPCWSTR lpName, CFuncCenter* pFuncCenter);
    ~CNv_UIS8910(void);

    virtual SPRESULT PreInit();

    SPRESULT Load();
    void ApplyChecksum();

public:
    BOOL m_bSameDownloadChecksum;
    int m_nBandCount;

	std::vector<NvData_BandInfo>         m_arrDownload_BandInfo;
    std::vector<NvData_BandInfo*>        m_arrpDownload_BandInfo;
    std::vector<Band_Info> m_arrBandInfo;
    NvData_CalibrationVersion*  m_pCalibration_Version;

private:
    SPRESULT InsertBand(LTE_BAND_E Band);
    SPRESULT InitBandNv();
    SPRESULT InitBandNvV5();
    SPRESULT InitBandNvV6();
	SPRESULT InitBandNv8910();
    void PrintCompatibleBand();
	SPRESULT GetUeRfbandInfo();

private:
    uint32 m_uVersion;
    uint32 m_arrChecksum[4];
    SP_HANDLE  m_hDUT;

//     PC_LTE_NV_V3_DATA_T Download_BandInfo_V6;
//     PC_LTE_NV_V2_DATA_T Download_BandInfo_V5;
    PC_LTE_NV_DATA_T    m_LteNv_Download_Checksum;
    PC_LTE_NV_DATA_T    m_LteNv_Cali_Version;

    NvData_DownloadChecksum*        m_pDownload_Checksum;
    NvData_DownloadChecksum         m_Last_Download_Checksum;
    std::vector<NvData_BandInfo*>    m_arrpDownload_BandInfo_Complete;

    CFileConfig_UIS8910* m_pFileConfig;  

    std::vector<int> m_arrBandNv;
    std::vector<int> m_arrCompatibleBandNv;

    std::vector<PC_LTE_NV_V2_DATA_T> m_arrDownload_BandInfo_Complete_V2;
    std::vector<PC_LTE_NV_V3_DATA_T> m_arrDownload_BandInfo_Complete_V3;

    T_UIS8910_BAND_INFO m_arrUeband_info[UIS8910_BAND_NUM];
    int m_nMaxBandCount;
};
