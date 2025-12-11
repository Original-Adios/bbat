#pragma once

#include "ILossFunc.h"
#include <list>

#define CDMA_MAX_NV_BAND_NUM (2)

typedef struct
{
	uint16 band_num;
	uint16 band_num_network;
	uint16 freq_nums_used;
	uint16 apc_comp_items_selection;
	uint16 comp_option;
	uint16 pa_gain_modes;
	int16 max_tx_power;
	int16 min_tx_power;
	uint16 pa_adjust_h_m;
	uint16 pa_adjust_m_l;
	uint16 pa_adjust_h_l;
	int16 max_prach_power;
	uint16 upa_pwr_limit;
}Nvm_all_tx_compensation_band_sequence_uint;

//NVM_C2K_CAL_DATA_APC_BAND
typedef struct
{
	uint16	freqx_apc_control_word[92];
	uint16	hdt_range;
	int16	hdt_high2mid_delta;
	int16	hdt_mid2low_delta;
	uint16	ul_cal_uarfcn;
}c2k_apc_control_words_onefreq;

typedef struct
{
	int16	power;
	int16	hdt_value;
}hdtx;

typedef struct
{
	hdtx	hdt[9];
}hdt_freqx_array;

typedef struct
{
	c2k_apc_control_words_onefreq	ApcCtrlWordFreq[21];
}Nvm_Apc_Result;

typedef struct
{
	hdt_freqx_array	HdetAdcFreq[21];
}Nvm_Hdet_Result;

typedef struct
{
	Nvm_Apc_Result	ApcRlst;
	Nvm_Hdet_Result	HdetAdcRlst;
	uint16	Reserved[6];
}Nvm_Cal_Apc_Band; //NVM_C2K_CAL_DATA_APC_BAND

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

typedef struct
{
	uint8	dl_freq_num;
	uint8	dl_gain_num;
	uint8	dl_noise_num;
}Dl_Agc_Config;

typedef struct
{
	int8	ul_gain_max_power;
	int8	ul_gain_min_power;
	uint8	ul_gain_start_index;
	uint8	ul_gain_end_index;
}Ul_PaMode_Config;

typedef struct
{
	uint8	band_num;
	Dl_Agc_Config	dl_agc_num[MAX_CDMA_ANT];
	uint8	ul_freq_num;
	uint8	ul_mod_num;
	Ul_PaMode_Config ul_pamode_gain_conf[CDMA_MAX_PA_LVL];
	uint8	ul_hdt_value_count;
	uint8	ul_max_power_setting_en;
	uint8	ul_max_high_byte;
	uint8	ul_max_low_byte;
	uint8	ul_cal_mode;
	uint8	reserved[2];
	int8	dl_imbalance_q2q;
	int8	dl_imbalance_i2q;
	uint8	rf_chip_type;
	uint8	rf_cali_version;
	uint8	dl_ici_enable;
	uint8	reserved1[30];
}Nvm_Cal_Info_Band; //NVM_C2K_CAL_DATA_CAL_INFO_BAND

class CC2KGs : public ILossFunc
{
public:
    CC2KGs(CImpBase *pSpat);
    virtual ~CC2KGs(void);

    virtual SPRESULT Run();

    virtual SPRESULT Init();
    virtual SPRESULT Release();
    virtual void ConfigFreq();
private:
    SPRESULT InitSeq(PC_C2K_CAL_TX_TUNE_REQ_T *pSeq);
    SPRESULT LoadNV();
    SPRESULT InitData();

	std::list<double> m_listFreq;

    BOOL m_bApt;
	double m_dC2KTargetPwr[BI_C_MAX_BAND];
	uint16  m_nIndex[BI_C_MAX_BAND];
	int m_nPaLvl[BI_C_MAX_BAND];
};
	