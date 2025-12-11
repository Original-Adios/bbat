#pragma once
#include "ImpBaseUIS8910.h"
#include "GSMCalDef.h"
#include "GSM8910Def.h"
#include "ExtraLogFile.h"

extern int g_GsmPclPwr[20];
extern int g_EGsmPclPwr[20];
extern int g_DcsPclPwr[20];
extern int g_PcsPclPwr[20];

//////////////////////////////////////////////////////////////////////////
class CImpBaseUIS8910;
class CFunUIS8910
{
protected:
    CFunUIS8910(CImpBaseUIS8910 *pImp);
    virtual ~CFunUIS8910(void);

public:
    static CFunUIS8910* InitFun(CImpBaseUIS8910 *pImp ,LPVOID dResearved = NULL);
    static void	FinalFun(CFunUIS8910 **pTest);

    virtual SPRESULT EnterFun(E_TEST_MODE eTestMode, E_SUB_TEST_MODE eSubMode);
	virtual SPRESULT LeaveFun();
	virtual SPRESULT SetAnt(int nAnt);
	virtual SPRESULT AntSwitch(int nOld, int nNew);

	void DisplayORFSMCode(char band);
	void DisplayORFSSCode(char band);
	void DisplayPVTCode(char band);
	void DisplayPFCode(char band);
	int pcl2dbm(char band, char pcl);
	float ch2freq(unsigned char band, unsigned short arfcn, BOOL dw1up0);
#if 0
	//void GetTokenStrings(LPTSTR lpszSouce, LPTSTR *lppszDest, LPCTSTR lpsczDelimit, int *lpnCount);
	void GetTokenStrings(LPSTR lpszSouce, LPSTR *lppszDest, LPCSTR lpsczDelimit, int *lpnCount);
	//LPSTR _W2CA(LPCWSTR lpszInput);
    void BufferChangeFormat(void);
    
	//------------ 仪表相关
	char gpib_InBuffer[4096];
    SPRESULT WriteGpibCmd(LPCSTR lpszCmd);
	SPRESULT ReadGpibCmd();
#endif
	//SPRESULT CalibInit(double GSMRF,double EGSMRF,double DCSRF, double PCSRF);
	SPRESULT CalibInit();
	
	SPRESULT SetBCHChannel(char band,int channel);
	SPRESULT SetTCHChannel(char band, unsigned short channel);
	SPRESULT SetTCHPcl(char band, char pcl);
	SPRESULT SetTCHTsc(unsigned char tsc);
	SPRESULT SetTCHPower(float Power);
	SPRESULT SetCellPower(char band, float Power);
	SPRESULT SetMeasureRxILoss(unsigned char SelPow);
	//void SetRfPathLoss(char band);
	SPRESULT SetRfPathLoss(float iloss);
	SPRESULT SetRxChannel(char band, int Arfcn);
	SPRESULT GetFreqErr(float *m_freqerr,char band,char pcl);
	SPRESULT GetTxPower(float exppow,float *measurepow);
	SPRESULT GetTxOffsetPower(char band,int pcl,double *measurepow);
	SPRESULT SetDacScanFreq(char band,int channel,float &fPower);
	SPRESULT SetDacScanPwr(float &expPower);
	SPRESULT ResetTester(char band);
	SPRESULT NonsigTotalInitialize();
	SPRESULT NonsigInitialize(char band,int tch, int bch, int pwr, float iLoss);
	SPRESULT NSFTSetup(char band, int channel);
	SPRESULT NsftMeasureTxPower(char band, int pcl, int power, float *pPwrUse);
	SPRESULT TestModulation(unsigned char band, unsigned int tch);
	SPRESULT TestSwSpec(unsigned char band, unsigned int tch);
	SPRESULT NsftMeasurePVT(char band, unsigned int tch);
	SPRESULT NsftMeasurePhaseNFrequency(char band, float *pFreq);
	SPRESULT SetGSMFastCaliUTSParam(unsigned char band,unsigned short channel, int frame_num);
	SPRESULT GetGSMFastCaliULResult(unsigned char band,volatile float *powbuffer);
	SPRESULT CFunUIS8910::SetGSMFastoffsetCaliUTSParam(unsigned char band);
	SPRESULT CFunUIS8910::GetFastCaliboffsetUTSResult(unsigned char band,volatile float* measured_pow);
	void SetFastCalibMode();
	void SetFastCalibStopMode();
	//------------ 仪表相关 end


	//------------ 终端相关
	SPRESULT closeUeTrace();
	SPRESULT send_cmd(uint32 cmd);
    SPRESULT send_mode(uint32 mode);
    SPRESULT mode_rx(uint8 band, uint16 arfcn, uint8 expPow, uint32 tag);
    SPRESULT mode_tx(uint8 band, uint16 arfcn, uint8 pcl, uint16 dacIdVal, uint8 tsc, uint32 tag);
	SPRESULT mode_tx_pcl(uint8 band, uint16 arfcn, uint8 pcl, uint8 tsc);
	SPRESULT mode_tx_pa_val(uint8 band, uint16 arfcn, uint16 dacIdVal, uint8 tsc);
	SPRESULT mode_rx_power(uint8 band, uint16 arfcn, uint8 expPow);
	SPRESULT update(void);
	SPRESULT set_xtal_freq_offset(int32 xtalFreqOffset, bool& stop);
	SPRESULT get_xcv_param(int id, int32 &param);
	SPRESULT get_rx_nb_power(uint8 &val);
	SPRESULT set_sw_param(int id, int32 param);
	SPRESULT get_calibflag(uint32 &flag);
	SPRESULT set_default_power_per_pcl_per_arfcn(int band, T_CALIB_DATA *pData);
	SPRESULT set_pa_profile_dac_req(int *pData0, int *pData1, int *pData2, int *pData3);
	SPRESULT set_pa_profile_meas(uint8 band, int32 measPower, uint16 &paProfNextDacVal, uint8 &paProfCalibDone);
	SPRESULT set_pa_profile_meass(uint8 band, int32 measPower[], uint16 paDacVal[]);
	//SPRESULT set_power_per_pcl_per_arfcn(int band, int pclId, int arfcn, int16 measuredPower,T_CALIB_DATA *pData);
	SPRESULT set_power_per_pcl_per_arfcn(int band, int16 *pMeasPow);
	SPRESULT set_method(bool automaticTool);
	SPRESULT set_iloss_offset(int8 ilossOffset, uint16 &iLossNextArfcn, bool &stop);
	SPRESULT set_connectNs(int rfset, uint8 band, uint16 tch, uint16 bch, uint8 pcl, uint8 &rspid, uint8 &status);
	SPRESULT mode_stop(void);
	SPRESULT burn_flash(bool automaticTool);
	SPRESULT reset_rfparameters(void);
	SPRESULT CalibPrepare(void);
	SPRESULT CalibBurnFlash(void);
	SPRESULT dump_GsmRftable(uint32 caliFlag);
	SPRESULT get_NsftComstatus(uint8 &rspid, uint8 &status);
	SPRESULT get_versions(int &major, int &minor);
	SPRESULT gsm_set_mode(uint32 cmd);
	SPRESULT set_Nsft_configure_req(uint8 &rspid, uint8 &status);
	SPRESULT set_Nsft_berloop_ab_orderreq(int rfset, uint8 &rspid, uint8 &status);
	SPRESULT set_Nsft_ber_start_log_req(int rfset, uint8 &rspid, uint8 &status);
	SPRESULT get_NsftBitNumber(uint32 &nbBits, uint32 &nbErr, uint32 &logindex);
	SPRESULT set_gsmNstFlag(uint32 caliFlag);
	SPRESULT set_gsmAntTestFlag(uint32 caliFlag);
	//------------ 终端相关 end

	LPSTR  m_lpOutputStrA;
	UINT8 g_gsmTrans[0x4000];
	static UINT8 NO_SIG_SETUP[7][13];

protected:   
    E_TEST_MODE m_eTestMode; 
    E_SUB_TEST_MODE m_eSubMode;
    CImpBaseUIS8910 *m_pImp;
    SP_HANDLE m_hDUT;
    IRFDevice *m_pRFTester;
	CExtraLog m_log; 
	//int m_nAnt;
};

