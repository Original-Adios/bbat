#pragma once
#include "SpatLibExport.h"
#include "gsmUtility.h"
#include "IRFDevice.h"

#define TDMA_SLOT3									(0x20)
#define TDMA_SLOT4									(0x10)
#define TDMA_SLOT5									(0x08)

#define MAX_GSM_UL_SWEEP_POINTS						( 80 )
#define MIN_SWEEP_POINTS_PER_POWER_LEVEL            ( 8  )


typedef struct
{		
	int16		nMaxFactor;	
	int16		nMinFactor;	
	int16		nMinStepFactor;	
	int16		nStep;
}CPaParam;

typedef struct
{
	int			nPcl[MAX_GSM_PCL];
	double		dExpPwr[MAX_GSM_PCL];            
	double		dTolerance[MAX_GSM_PCL]; 
    double      dChanTol[MAX_GSM_PCL];
}CTragetPower;


typedef struct
{
	BOOL  bScan;
	//BOOL  bAlwaysScan;
	int16 nDefFactor[MAX_GSM_PCL];
    double dPower[MAX_GSM_PCL];
}CScanParam;

typedef struct
{
	BOOL			bSpecChanel;
	int16			nChannel[NUM_FREQ_LEVEL]; 
    int16           nBoundary[NUM_FREQ_LEVEL];
}CAPCChannel;

typedef struct
{
	int16 nFactorTable[NUM_FREQ_LEVEL * MAX_GSM_PCL];
}CGSMAPCNV;

typedef struct
{   
	int				nBand;	
	double			dMpComp[NUM_FREQ_LEVEL];
	CAPCChannel		tChannel;
	CTragetPower	tTargetPower;
	CScanParam		tScanParam;
	CPaParam		tPaParam;
	//CGSMAPCNV		tNV;
}CAPCCalParam;

//////////////////////////////////////////////////////////////////////////

#define SIZE_OF_RX_COMPENSATE_VALUE (75)
#define SIZE_OF_CALI_AGC_CRTL_WORD  (91)

#define NV_DIV_DISABLE (0X00)
#define NV_DIV_ENABLE  (0X01)
#define NV_DIV_INDEPENDENT   (0X04)
#define NV_PCS1900_DIV (0X10)
#define NV_DCS1800_DIV (0X20)
#define NV_EGSM900_DIV (0X40)
#define NV_GSM850_DIV  (0X80) 

enum
{
	GSM_SAW,
	GSM_SAWLESS,
	GSM_SAW_NUM,
};

//enum
//{
//	GSM_MAIN,
//	GSM_DIV,
//	GSM_MAX_ANT,
//};



typedef struct _cal_ch_t
{
	short cal_ch[20];
	short cal_ch_num;
}cal_ch_t;

typedef struct
{
	int16   nRefChannel;
	cal_ch_t tCompChannel;
}CAGCChannel;

typedef struct _cali_point_t
{
	short cali_gain_index;
	short cellPower;
}cali_point_t;

typedef struct _cali_param_t
{
	short cali_point_num;
	short comp_point;
	short max_rx_gain_index;
	short min_gain;
	short max_gain;
	cali_point_t cali_point[5];
	short reserve[12];
}cali_param_t;

typedef struct
{
	uint16	nMaxGainIndex;
	uint16	nAgcWord[SIZE_OF_CALI_AGC_CRTL_WORD];
	uint16	nCompTable[SIZE_OF_RX_COMPENSATE_VALUE];
}CGSMAGCNV;


typedef struct
{ 
	BOOL		 bNv;
	int			 nBand;
	CAGCChannel  tChannel;
	cali_param_t tCalGain[GSM_SAW_NUM];
	uint16		 arrGainTable[GSM_SAW_NUM][45];
}CAGCCalParam;

class CFPPoint
{
public:
    CFPPoint()
    {
    }
    CFPPoint(int16 nFactor, double dPower)
    {
        this->nFactor = nFactor;
        this->dPower = dPower;
    }

    int16 nFactor;
    double dPower;

    static BOOL FindInvalidData(CFPPoint &pt)
    {
        if (IS_EQUAL(pt.dPower , INVALID_NEGATIVE_DOUBLE_VALUE))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    void operator=(const CFPPoint &pt)
    {
        this->nFactor = pt.nFactor;
        this->dPower = pt.dPower;
    }

    static bool CompareFactor(const CFPPoint& pt1, const CFPPoint& pt2)
    {
        return pt1.nFactor>pt2.nFactor;
    }
};

//3130

#define MAX_3130_CALI_GAIN_INDEX (4)
typedef struct _cali_param_3130_t
{
	uint16 cali_gain;
	int16  cali_power;
	uint16 target_rssi;
	uint16 comp_rssi[BI_GSM_MAX_BAND];
	int16  delta_rssi;
	uint16 mid_point_gain[MAX_3130_CALI_GAIN_INDEX];
	uint16 mid_point_index[MAX_3130_CALI_GAIN_INDEX];
	int16  min_gain;
	int16  max_gain;
	uint16 digtal_gain[91];
}cali_param_3130_t;

typedef struct
{ 
	BOOL			  bBand[BI_GSM_MAX_BAND];
	CAGCChannel		  tChannel[BI_GSM_MAX_BAND];
	cali_param_3130_t tCalGain;
}CAGCCal3130Param;