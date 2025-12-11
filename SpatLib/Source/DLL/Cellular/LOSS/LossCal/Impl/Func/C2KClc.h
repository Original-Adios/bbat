#pragma once

#include "ILossFunc.h"

#define CDMA_MAX_NV_BAND_NUM (2)

typedef struct
{
	uint8	correct_freq_num;
	int8	power_correct[119];
}c2k_cal_config_hdt_band;

typedef struct
{
	uint8	hdt_enable;
	uint8	flag_map;
	uint8	reserved[2];
	c2k_cal_config_hdt_band	HdtBandConf[CDMA_MAX_NV_BAND_NUM];
}c2k_cal_config_hdt;

typedef struct
{
	uint8	dcdc_apt_enable;
	uint8	dcdc_fix_vol;
	uint8	reserved[2];
}c2k_cal_config_apt_header;

typedef struct
{
	uint8	high_used;
	uint8	mid_used;
	uint8	low_used;
	uint8	reserved;
}c2k_cal_config_apt_band_header;

typedef struct
{
	int8	start_power;
	int8	end_power;
	uint8	dcdc_index;
	uint8	reserved;
}c2k_cal_config_PaLvl;

typedef struct
{
	c2k_cal_config_PaLvl	AptPaLvlParam[CDMA_TX_MAX_APT_NUM];
}c2k_cal_config_apt_band_PaLvl;

typedef struct
{
	c2k_cal_config_apt_band_header	BandHeader;
	c2k_cal_config_apt_band_PaLvl	PaLvl[CDMA_MAX_PA_LVL];
}c2k_cal_config_apt_band;

typedef struct
{
	c2k_cal_config_apt_header	AptHeader;
	c2k_cal_config_apt_band	AptBand[CDMA_MAX_NV_BAND_NUM];
}c2k_cal_config_apt;

typedef struct
{
	int8	high_start_power;
	int8	high_end_power;
	int8	mid_start_power;
	int8	mid_end_power;
	int8	low_start_power;
	int8	low_end_power;
	int8	reserved[2];
}c2k_cal_config_paswitch_band;

typedef struct
{
	c2k_cal_config_paswitch_band	PaSwitchBand[CDMA_MAX_NV_BAND_NUM];
}c2k_cal_config_pa;

typedef struct
{
	c2k_cal_config_hdt	hdt_config;
	c2k_cal_config_apt	apt_config;
	c2k_cal_config_pa	pa_config;
}Nvm_Download_Optimize_Cal_Config; //NVM_C2K_DOWNLOAD_OPTIMIZE_CAL_CONFIG

class CC2KClc : public ILossFunc
{
public:
    CC2KClc(CImpBase *pSpat);
    virtual ~CC2KClc(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
private:
    SPRESULT TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT &pwrRlt);
    SPRESULT InitSeq(PC_C2K_CAL_TX_TUNE_REQ_T *pSeq);
    SPRESULT LoadNV();
    SPRESULT InitData();

    BOOL m_bApt;
};
