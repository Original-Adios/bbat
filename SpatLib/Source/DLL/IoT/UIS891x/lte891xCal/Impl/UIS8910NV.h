#pragma once

typedef short SINT16;

#define UIS8910_BAND_NUM  24
#define UIS8910_AGC_NUM   125
#define UIS8910_CALIBAND40_POS  13
#define UIS8910_CALIBAND41_POS  14

typedef struct
{
    int nNumeral;
    int nIndicator;
	int nActiveFlag;
}T_UIS8910_BAND_INFO;

typedef struct
{
    UINT16 cdac;
    UINT16 afc;
}T_UIS8910_AFC_INFO;

#if 0
typedef struct
{
    unsigned short atp_status;
    unsigned short apt_fix_volt;
	unsigned short apt_seg_num;
	unsigned short reserved;
	short apt_seg_thresh[8];
	unsigned short apt_seg_volt[8];
}T_UIS8910_APT_INFO;
#endif

typedef struct
{
    short HPM_start_pwr;
    short HPM_stop_pwr;
	short MPM_start_pwr;
	short MPM_stop_pwr;
	short LPM_start_pwr;
	short LPM_stop_pwr;
}rf_lte_nv_PM_switch_backup_t;

typedef struct
{
    short start_pwr;
    short stop_pwr;
	unsigned short apt_index;
}rf_lte_nv_pwr_seg_backup_t;


typedef struct
{
    unsigned short use_seg_num;
    short reserved;
	rf_lte_nv_pwr_seg_backup_t pwr_seg[4];
}rf_lte_nv_HPM_config_backup_t;

typedef struct
{
    unsigned short use_seg_num;
    short reserved;
	rf_lte_nv_pwr_seg_backup_t pwr_seg[4];
}rf_lte_nv_MPM_config_backup_t;

typedef struct
{
    unsigned short use_seg_num;
    short reserved;
	rf_lte_nv_pwr_seg_backup_t pwr_seg[8];
}rf_lte_nv_LPM_config_backup_t;

typedef struct
{
    unsigned short atp_status;    //PA mode & APT flag  1：active
    unsigned short apt_fix_dac; 
	unsigned short apt_volt_step; 
	unsigned short reserved;
	unsigned short apt_seg_dac[8];
	rf_lte_nv_PM_switch_backup_t PM_switch;
	rf_lte_nv_HPM_config_backup_t HPM_config;
	rf_lte_nv_MPM_config_backup_t MPM_config;
	rf_lte_nv_LPM_config_backup_t LPM_config;

}T_UIS8910_APT_INFO;

typedef struct Tag_UIS8910_BAND_NV
{
    T_UIS8910_BAND_INFO band_info[UIS8910_BAND_NUM];
    T_UIS8910_APT_INFO apt_info[UIS8910_BAND_NUM];
	T_UIS8910_AFC_INFO afc_info; 

	Tag_UIS8910_BAND_NV()
	{
        ZeroMemory(band_info, UIS8910_BAND_NUM*sizeof(T_UIS8910_BAND_INFO));
        ZeroMemory(apt_info, UIS8910_BAND_NUM*sizeof(T_UIS8910_APT_INFO));
        ZeroMemory(&afc_info, sizeof(T_UIS8910_AFC_INFO));
	}

}T_UIS8910_BAND_NV;

typedef struct
{
    int nPaMode;
    int nIndex;
	int nApt;
	int nHighPower;
	int nLowPower;
}T_UIS8910_APTSIM;

typedef struct
{
    int aptNum;
    T_UIS8910_APTSIM aptSet[40];
}T_UIS8910_APCSIM;

////////////////////
typedef struct
{
   int8 arrRb[7];
}rf_lte_TxDroop_BW;

typedef struct
{
   rf_lte_TxDroop_BW arrBw[6];
}rf_lte_TxDroop_CHAN;

typedef struct
{
    rf_lte_TxDroop_CHAN arrChan[3];
}rf_lte_TxDroop;
//////////////////////

typedef struct
{
	unsigned short LTE_AFC          :1;
	unsigned short LTE_AGC_TDD      :1;
	unsigned short LTE_APC_TDD      :1;
	unsigned short LTE_RESEVERD1    :1;
	unsigned short LTE_AGC_FDD      :1;
	unsigned short LTE_APC_FDD      :1;
	unsigned short LTE_RESEVERD2    :1;
	unsigned short LTE_TX_DROOP     :1;
	unsigned short FINAL_LTE        :1;
	unsigned short ANT_LTE          :1;
	unsigned short LTE_XTAL_THERM   :1;
	unsigned short FINAL_LTE2       :1;
	unsigned short LTE_RESEVERD4    :4;
}rf_lte_calib_flag;

typedef struct rf_lte_calib_dcxo_tag
{
	unsigned short cadc;
	unsigned short cafc;
	unsigned short slope;
	unsigned short reserved;
}rf_lte_calib_dcxo;

typedef struct rf_lte_calib_afc_tag
{
	rf_lte_calib_dcxo dcxo;
}rf_lte_calib_afc;

typedef struct rf_lte_calib_xtal_therm_tag
{
	int k;
	int c;
}rf_lte_calib_xtal_therm;

#if 0
typedef struct rf_lte_calib_com_tag
{
	unsigned short version;
	rf_lte_calib_flag calib_flag;
	rf_lte_calib_afc afc;
	unsigned short reserved[20];
}rf_lte_calib_com;
#endif

typedef struct rf_lte_calib_com_tag
{
	unsigned short version;
	rf_lte_calib_flag calib_flag;   //回写
	rf_lte_calib_afc afc;           //回写
	unsigned int calib_version;     //版本匹配
	unsigned int rfnv_version;
	unsigned short SARtoSDM_flag;
	unsigned short apt_flag;        //0xCA01 表示有效 回写
	unsigned int calib_time_ymd;    //校准时间，回写
	unsigned short mode_flag;       //0xca01:cat1/0xca02:catm/0xca03:nonbl
	unsigned short xtal_therm_flag; //0xca01:
	unsigned short TxDroop_flag;    //0xca01:1RB/PRB/0xca02:1RB/PRB/FRB
	unsigned short AFC_flag;        //0xca01:VCTCXO/0xca02:CRY-CAP-SW
	unsigned int calib_time_hms;    //校准时间，回写
    unsigned int calib_band_flag;   //校准band标志
	rf_lte_calib_xtal_therm xtal_therm;
}rf_lte_calib_com;

#if 0
typedef struct
{
	unsigned short apc_idx      :10;
	unsigned short pamode_idx   :2;
	unsigned short apt_idx      :4;
}rf_lte_calib_apc_value;
#endif

typedef struct rf_lte_calib_agc_tag
{
	short gain_center_channel[126];
	short gain_channel_arry[50];  //3*15
}rf_lte_calib_agc;

typedef struct rf_lte_calib_apc_tag
{
	unsigned short index_center_channel[80];
	short index_channel_arry[240];  //16*15
}rf_lte_calib_apc;

typedef struct rf_lte_calib_agc_ch20_tag
{
	short gain_center_channel[126];
	short gain_channel_arry[60];  //3*20
}rf_lte_calib_agc_ch20;

typedef struct rf_lte_calib_apc_ch20_tag
{
	unsigned short index_center_channel[80];
	short index_channel_arry[320];  //16*20
}rf_lte_calib_apc_ch20;

typedef struct rf_lte_calib_agc_ch40_tag
{
	short gain_center_channel[126];
	short gain_channel_arry[120];  //3*40
}rf_lte_calib_agc_ch40;

typedef struct rf_lte_calib_apc_ch40_tag
{
	unsigned short index_center_channel[80];
	short index_channel_arry[640];  //16*40
}rf_lte_calib_apc_ch40;

typedef struct rf_lte_calib_param_tag
{
	rf_lte_calib_agc agc;
	rf_lte_calib_apc apc;
	T_UIS8910_APT_INFO apt;
	rf_lte_TxDroop TxDroop;
	unsigned short reserved[15];
}rf_lte_calib_param;

typedef struct rf_lte_calib_param_ch20_tag
{
	rf_lte_calib_agc_ch20 agc;
	rf_lte_calib_apc_ch20 apc;
	T_UIS8910_APT_INFO apt;
	rf_lte_TxDroop TxDroop;
	unsigned short reserved[15];
}rf_lte_calib_param_ch20;

typedef struct rf_lte_calib_param_ch40_tag
{
	rf_lte_calib_agc_ch40 agc;
	rf_lte_calib_apc_ch40 apc;
	T_UIS8910_APT_INFO   apt;
	rf_lte_TxDroop TxDroop;
	unsigned short reserved[15];
}rf_lte_calib_param_ch40;

typedef struct rf_lte_calib_tab_tag
{
	rf_lte_calib_com    com;
	rf_lte_calib_param  param_NormalT[21];
	rf_lte_calib_param_ch20  param_T22;
	rf_lte_calib_param_ch20  param_T23;
	rf_lte_calib_param_ch40  param_T24;
}rf_lte_calib_tab_t;


