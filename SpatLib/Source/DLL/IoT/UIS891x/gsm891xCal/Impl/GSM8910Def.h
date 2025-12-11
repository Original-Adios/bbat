#pragma once
#include "SpatLibExport.h"
#include "gsmUtility.h"
#include "IRFDevice.h"

#define NB_MEAS_ILOSS 30

typedef enum 
{
    CALIB_PROCESS_STOP                          = 0x00000000,
    CALIB_PROCESS_CONTINUE                      = 0x00000001,
    CALIB_PROCESS_PENDING                       = 0x00000002,
    CALIB_PROCESS_NEED_CALM                     = 0x00000003,
    CALIB_PROCESS_ERR_BAD_POW                   = 0x000000F0,
    CALIB_PROCESS_ERR_NO_MONO_POW               = 0x000000F1,
    CALIB_PROCESS_ERR_ZERO_DAC                  = 0x000000F2,
    CALIB_PROCESS_ERROR                         = 0x000000FF
} CALIB_H_ENUM_0_T;


typedef enum 
{
    JDL_ARFCN_MIN,
    JDL_ARFCN_MAX
} jdl_arfcn_pos_t;

typedef struct 
{
    UINT32                         revision;                     //0x00000000
    UINT32                         number;                       //0x00000004
    UINT32                         date;                         //0x00000008
    INT8*                          string;                       //0x0000000C
} HAL_MAP_VERSION_T; //Size : 0x10

typedef struct _tagBand_select
{
    int m_iGsmBand;
    int m_iEGsmBand;
    int m_iDcsBand;
    int m_iPcsBand;

	_tagBand_select()
	{
        m_iGsmBand = 0;
        m_iEGsmBand = 0;
        m_iDcsBand = 0;
        m_iPcsBand = 0;
	}
} Band_select;


typedef struct _tagChannel_select
{
    int m_iGsmChannel;
    int m_iEGsmChannel;
    int m_iDcsChannel;
    int m_iPcsChannel;

	_tagChannel_select()
	{
        m_iGsmChannel = 0;
        m_iEGsmChannel = 0;
        m_iDcsChannel = 0;
        m_iPcsChannel = 0;
	}
} Channel_select;

typedef struct _tagTest_Channel
{
    vector<int> m_iGsmarrChannels;
    vector<int> m_iEGsmarrChannels;
    vector<int> m_iDcsarrChannels;
    vector<int> m_iPcsarrChannels;

	_tagTest_Channel()
	{
		m_iGsmarrChannels.clear();
        m_iEGsmarrChannels.clear();
        m_iDcsarrChannels.clear();
        m_iPcsarrChannels.clear();
	}
} Test_Channel;

typedef struct _tagTest_Item
{
    int m_iUseCharge;
    int m_iTestPowerflag;
    int m_iTestPandFErrorflag;
    int m_iTestPVTflag;
    int m_irxlevelflag;
    int m_iTestSensitivity;
    int m_iTestMSpecture;
    int m_iTestSSpecture;
	int m_iSinalTest;

    _tagTest_Item()
	{
        m_iUseCharge = 0;
        m_iTestPowerflag = 0;
        m_iTestPandFErrorflag = 0;
        m_iTestPVTflag = 0;
        m_irxlevelflag = 0;
        m_iTestSensitivity = 0;
        m_iTestMSpecture = 0;
        m_iTestSSpecture = 0;
        m_iSinalTest = 0;
	}
} Test_Item;

typedef struct _tagTest_Sensitivity
{
    vector<float> m_fGsmSensitivity;
    vector<float> m_fEGsmSensitivity;
    vector<float> m_fDcsSensitivity;
    vector<float> m_fPcsSensitivity;
	float m_berLimit;

	_tagTest_Sensitivity()
	{
		m_fGsmSensitivity.clear();
		m_fEGsmSensitivity.clear();
		m_fDcsSensitivity.clear();
		m_fPcsSensitivity.clear();
		m_berLimit = 0;
	}
} Test_Sensitivity;

typedef struct 
{
    int m_iGsmFrequencyError;
    int m_iEGsmFrequencyError;
    int m_iDcsFrequencyError;
    int m_iPcsFrequencyError;
    int m_iGsmPhaseError;
    int m_iEGsmPhaseError;
    int m_iDcsPhaseError;
    int m_iPcsPhaseError;
    int m_iGsmRMSError;
    int m_iEGsmRMSError;
    int m_iDcsRMSError;
    int m_iPcsRMSError;
} Test_PandFError;

typedef struct 
{
    int m_iGsmRxLevel;
    int m_iEGsmRxLevel;
    int m_iDcsRxLevel;
    int m_iPcsRxLevel;
    int m_iGsmRxLevelLimit;
    int m_iEGsmRxLevelLimit;
    int m_iDcsRxLevelLimit;
    int m_iPcsRxLevelLimit;
} Cali_RxLevel;

typedef struct _tagTest_RxLevel
{
    vector<int> m_iGsmRxLevel;
    vector<int> m_iEGsmRxLevel;
    vector<int> m_iDcsRxLevel;
    vector<int> m_iPcsRxLevel;
    int m_iGsmRxLevelLimit;
    int m_iEGsmRxLevelLimit;
    int m_iDcsRxLevelLimit;
    int m_iPcsRxLevelLimit;

	_tagTest_RxLevel()
	{
		m_iGsmRxLevel.clear();
		m_iEGsmRxLevel.clear();
		m_iDcsRxLevel.clear();
		m_iPcsRxLevel.clear();
		m_iGsmRxLevelLimit = 0;
		m_iEGsmRxLevelLimit = 0;
		m_iDcsRxLevelLimit = 0;
		m_iPcsRxLevelLimit = 0;
	}

} Test_RxLevel;

typedef struct _tagTest_power
{
    vector<int> m_iGsmPCL;
    vector<int> m_iEGsmPCL;
    vector<int> m_iDcsPCL;
    vector<int> m_iPcsPCL;
    vector<int> m_iGsmPWR;
    vector<int> m_iEGsmPWR;
    vector<int> m_iDcsPWR;
    vector<int> m_iPcsPWR;

	_tagTest_power()
	{
		m_iGsmPCL.clear();
		m_iEGsmPCL.clear();
		m_iDcsPCL.clear();
		m_iPcsPCL.clear();
		m_iGsmPWR.clear();
		m_iEGsmPWR.clear();
		m_iDcsPWR.clear();
		m_iPcsPWR.clear();
	}

} Test_power;

typedef struct _tagMaxandMin_pclvalue
{
    float m_iPerPclMaxValue;
    float m_iPerPclMinValue;

	_tagMaxandMin_pclvalue()
	{
        m_iPerPclMaxValue = 0.0f;
        m_iPerPclMinValue = 0.0f;
	}
} MaxandMin_pclvalue;

typedef struct 
{
    MaxandMin_pclvalue Gsm_pclvalue[15];
} Gsm_AllPclLimitvalue;

typedef struct 
{
    MaxandMin_pclvalue Dcs_pclvalue[16];
} Dcs_AllPclLimitvalue;

typedef struct _tagCalib_Item
{
    int m_iUseCharge;
    int m_iCalibBattery;
    int m_iCalibCrystal;
    int m_iCalibAFC;
    int m_iCalibILoss;
    int m_iCalibPAProfile;
    int m_iCalibPAOffset;
    int m_iSetNewPcl;
    int m_iFastCalib;
    int m_iCalibPAProfileDac;
    int m_iCalibTiming;
    int m_iCalibRamp;
    int m_iCalibSaveCfp;

	_tagCalib_Item()
	{
        m_iUseCharge = 0;
        m_iCalibBattery = 0;
        m_iCalibCrystal = 0;
        m_iCalibAFC = 0;
        m_iCalibILoss = 0;
        m_iCalibPAProfile = 0;
        m_iCalibPAOffset = 0;
        m_iSetNewPcl = 0;
        m_iFastCalib = 0;
        m_iCalibPAProfileDac = 0;
        m_iCalibTiming = 0;
        m_iCalibRamp = 0;
        m_iCalibSaveCfp = 0;
	}
} Calib_Item;


typedef struct 
{
//  float m_TxPower[3];
    float m_TxLoss[3];
    float m_TxFrequency[3];
//  float m_RxPower[3];
    float m_RxLoss[3];
    float m_RxFrequency[3];
} ILossSetup;


typedef struct 
{
    INT16  flag;
	INT16  targetPcl[15];
} T_TARGETPCL_GSM;

typedef struct 
{
    INT16  flag;
	INT16  targetPcl[16];
} T_TARGETPCL_DCS;

typedef struct
{
    INT16 TargetPclPowG[CALIB_GSM_PCL_QTY];
    INT16 TargetPclPowG850[CALIB_GSM_PCL_QTY];
    INT16 TargetPclPowD[CALIB_GSM_PCL_QTY+2];
    INT16 TargetPclPowP[CALIB_GSM_PCL_QTY+3];
    INT16 DefaultTargetPclPowG[CALIB_GSM_PCL_QTY];
    INT16 DefaultTargetPclPowG850[CALIB_GSM_PCL_QTY];
    INT16 DefaultTargetPclPowD[CALIB_GSM_PCL_QTY+2];
    INT16 DefaultTargetPclPowP[CALIB_GSM_PCL_QTY+3];
}T_CALIB_DATA;

typedef struct _tagT_Calib_PclValue
{
    int m_iGsmCrystalPcl;
    int m_iGsmAfcPcl;
    int m_iEGsmAfcPcl;
    int m_iDcsAfcPcl;
    int m_iPcsAfcPcl;

	_tagT_Calib_PclValue()
	{
        m_iGsmCrystalPcl = 0;
        m_iGsmAfcPcl = 0;
        m_iEGsmAfcPcl = 0;
        m_iDcsAfcPcl = 0;
        m_iPcsAfcPcl = 0;
	}

}T_Calib_PclValue;

typedef struct tag_XTAL_LIMIT 
{
    int	 nLowerVal;
    int  nUpperVal;
    tag_XTAL_LIMIT(void)
    {
        nLowerVal = 0;
        nUpperVal = 127;
    }
}T_XTAL_LIMIT;

typedef struct tag_PaProfileDac
{
    float MaxPwrG850;
    float MaxPwrG900;
    float MaxPwrD;
    float MaxPwrP;
    tag_PaProfileDac(void)
    {
        MaxPwrG850 = 0.0;
        MaxPwrG900 = 0.0;
        MaxPwrD = 0.0;
        MaxPwrP = 0.0;
    }
}T_PaProfileDac;

typedef struct _tag_ScanMinPower
{
	float ScanMinPwrG850;
	float ScanMinPwrG900;
	float ScanMinPwrD1800;
	float ScanMinPwrP1900;
    _tag_ScanMinPower(void)
	{
        ScanMinPwrG850 = -3.0;
        ScanMinPwrG900 = -3,0;
        ScanMinPwrD1800 = -6.0;
		ScanMinPwrP1900 = -6.0;
	}
}T_ScanMinPower;
typedef struct 
{
    float lossMin;
    float lossMax;
    float arfcnLimit;
}T_ILOSS_LIMIT;

typedef struct 
{
    INT16 saveDataFlag;
	Band_select bandSel;
	Calib_Item calibItem;
	Channel_select defaultChannel;
    INT16 GsmCrystalPcl;
	T_XTAL_LIMIT Xtallimit;
    T_PaProfileDac PaProfileDac;
    T_ScanMinPower ScanMinPower;
	T_ILOSS_LIMIT lossLimitG850;
	T_ILOSS_LIMIT lossLimitG900;
	T_ILOSS_LIMIT lossLimitD;
	T_ILOSS_LIMIT lossLimitP;
	Cali_RxLevel rxlevelPara;
	T_TARGETPCL_GSM TargetPclPowG850;
	T_TARGETPCL_GSM TargetPclPowG900;
	T_TARGETPCL_DCS TargetPclPowD;
	T_TARGETPCL_DCS TargetPclPowP;
	Gsm_AllPclLimitvalue gsmPclLimit;
	Gsm_AllPclLimitvalue egsmPclLimit;
	Dcs_AllPclLimitvalue dcsPclLimit;
	Dcs_AllPclLimitvalue pcsPclLimit;
    int nDacReScan;
    BOOL bDacScan_flag;
}T_8910CALIB_PARA;


typedef struct 
{
    Band_select bandSel;
	Test_Item   testItem;
	Test_Channel Channel;
	Test_Sensitivity sensitivityPara;
	Test_RxLevel rxlevelPara;
	Test_PandFError pferrorPara;
    Test_power txpower;
	Gsm_AllPclLimitvalue gsmPclLimit;
	Gsm_AllPclLimitvalue egsmPclLimit;
	Dcs_AllPclLimitvalue dcsPclLimit;
	Dcs_AllPclLimitvalue pcsPclLimit;
}T_8910TEST_PARA;

typedef struct 
{
    int m_iPaDac[16];
} Calib_PaProfileDac;

typedef struct _DacVsPower_Tag
{
    float m_fPower;
    int   m_Dacvalue;
    _DacVsPower_Tag()
    {
        m_fPower = 0;
        m_Dacvalue = 0;
    }
} m_DacVsPower_Tag;
