#pragma once
#include "../ImpBase.h"
 
//////////////////////////////////////////////////////////////////////////
class CFastCharge : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CFastCharge)
public:
    CFastCharge(void);
    virtual ~CFastCharge(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
	
	struct ChargeValue
	{
		int8 iOperate;   // BBAT_CHARGE 
		int8 Fast_Charge_Oper;	//BBAT_FAST_CHARGE_OPERATE
	};

	enum BBAT_CHARGE
	{
		OPEN = 1,
		READ,
		CLOSE,
		FAST_CHARGE
	};

	enum BBAT_FAST_CHARGE_OPERATE
	{
		FAST_CHARGE_CLOSE,
		CHANNEL_1,
		CHANNEL_2,
		OPEN_ALL = 3,		//打开所有快充接口
		FAST_CHARGE_READ = 4,			//读电流
	};


	struct ChargeData
	{
		uint8 iCurrent_1;
		uint8 iCurrent_2;
		uint8 iVotage_1;
		uint8 iVotage_2;
	};

private:
    int m_iCurrentUpSpec;
    int m_iCurrentDownSpec;
    int m_iVoltageUpSpec;
    int m_iVoltageDownSpec;

	SPRESULT FastChargeClose();
	SPRESULT FastChargeOpen(int iChannel);
	SPRESULT CheckCurrent(ChargeData DataRecv, int iChannel);
};


