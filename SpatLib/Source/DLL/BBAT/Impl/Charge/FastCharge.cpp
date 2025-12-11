#include "StdAfx.h"
#include "FastCharge.h"

//
IMPLEMENT_RUNTIME_CLASS(CFastCharge)

//////////////////////////////////////////////////////////////////////////
CFastCharge::CFastCharge(void)
{
    m_iCurrentUpSpec = 0;
    m_iCurrentDownSpec = 0;
    m_iVoltageUpSpec = 0;
    m_iVoltageDownSpec = 0;
}

CFastCharge::~CFastCharge(void)
{
}
SPRESULT CFastCharge::__PollAction(void)
{
	//open channel_1
	//sleep(500)
	//open channel_2
	//close
	CHKRESULT(FastChargeOpen(BBAT_FAST_CHARGE_OPERATE::CHANNEL_1));
	Sleep(500);
	//如果第一路测试结果满足要求, 发命令打开第二路充电(手机收到命令软件自动关闭第一路充电), 不满足测试要求,工具判断第一路测试fail
	CHKRESULT(FastChargeOpen(BBAT_FAST_CHARGE_OPERATE::CHANNEL_2));
	CHKRESULT(FastChargeClose());
	return SP_OK;

}
BOOL CFastCharge::LoadXMLConfig(void)
{
    m_iCurrentUpSpec = GetConfigValue(L"Option:CurrentUpSpec", 0);
    m_iCurrentDownSpec = GetConfigValue(L"Option:CurrentDownSpec", 0);
    m_iVoltageUpSpec = GetConfigValue(L"Option:VoltageUpUpSpec", 0);
    m_iVoltageDownSpec = GetConfigValue(L"Option:VoltageDownSpec", 0);

	return TRUE;
}

SPRESULT CFastCharge::FastChargeClose()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CHARGE;
	ChargeValue DataSend;
	DataSend.iOperate = BBAT_CHARGE::FAST_CHARGE;
	DataSend.Fast_Charge_Oper = BBAT_FAST_CHARGE_OPERATE::FAST_CHARGE_CLOSE;

	SPRESULT res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, Timeout_PHONECOMMAND_BBAT);
	if (res != SP_OK)
	{
		PrintErrorMsg(SP_E_BBAT_VALUE_FAIL, "BBAT Fast Charge Close Fail", LEVEL_ITEM);
		return SP_E_BBAT_VALUE_FAIL;
	}
	PrintSuccessMsg(SP_OK, "BBAT Fast Charge Close Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CFastCharge::FastChargeOpen(int iChannel)
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CHARGE;
	ChargeValue DataSend;
	DataSend.iOperate = BBAT_CHARGE::FAST_CHARGE;
	DataSend.Fast_Charge_Oper = (int8)iChannel;

	ChargeData DataRecv;
	memset(&DataRecv, 0, sizeof(DataRecv));

	SPRESULT res = SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), &DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT);
	if (res != SP_OK)
	{
		_UiSendMsg("BBAT Fast Charge", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr, 
			"Channel = %d, Open Fail", iChannel);
		return SP_E_BBAT_VALUE_FAIL;
	}
	_UiSendMsg("BBAT Fast Charge", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"Channel = %d, Open Pass", iChannel);
	CHKRESULT(CheckCurrent(DataRecv, iChannel));
	return SP_OK;
}

SPRESULT CFastCharge::CheckCurrent(ChargeData DataRecv, int iChannel)
{
	if (DataRecv.iCurrent_1 > m_iCurrentUpSpec || DataRecv.iCurrent_1 < m_iCurrentDownSpec) //mV
	{
		_UiSendMsg("BBAT Fast Charge, Current Fail", LEVEL_ITEM, 
			m_iCurrentDownSpec, DataRecv.iCurrent_1, m_iCurrentUpSpec, 
			NULL, -1, NULL,"Channel = %d",iChannel);
		//close
		CHKRESULT(FastChargeClose());
		return SP_E_BBAT_VALUE_FAIL;
	}
	_UiSendMsg("BBAT Fast Charge, Current Pass", LEVEL_ITEM,
		m_iCurrentDownSpec, DataRecv.iCurrent_1, m_iCurrentUpSpec,
		NULL, -1, NULL, "Channel = %d", iChannel);
	return SP_OK;
}
