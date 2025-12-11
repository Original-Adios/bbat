#pragma once
#include "../ImpBase.h"

#define MaxFMRssiNumber -20
//////////////////////////////////////////////////////////////////////////
class CFm_Transmit : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CFm_Transmit)
public:
	CFm_Transmit(void);
	virtual ~CFm_Transmit(void);

protected:
	virtual SPRESULT __PollAction(void);
    virtual void __LeaveAction(void);
	virtual BOOL LoadXMLConfig(void);

private:    
    double m_dSNRSpec = 0.0;
	double m_dPeakSpec = 0.0;



    SPRESULT FM_TransmitAction();
	SPRESULT Phone_FMTransOpen();
	SPRESULT Phone_FMTransClose();
    SPRESULT AFCB_HeadMicModeSelect();
    SPRESULT AFCB_MainMicAmpEnable(bool bStatus);
    SPRESULT AFCB_HeadSetLRSelect();
    SPRESULT AFCB_WaveGetData();
    SPRESULT AFCB_WaveSetStatus(bool bstatus);

	uint16 m_u16Freq1 = 0;
    uint16 m_u16Freq2 = 0;
    uint16 m_u16Freq3 = 0;
    uint16 m_u16Freq4 = 0;
    int m_nSleepTime = 0;

};