#pragma once
#include "../ImpBase.h"

#include "BTApiHCI.h"
#include "BtCommonDef.h"

using namespace std;

struct  Bt_Hci_Packet_Type
{
	BT_HCI_PACKET_TYPE Type;
	const char* pszType;
};

struct  Bt_Hci_Packet_Type_Str
{
	Bt_Hci_Packet_Type stuType[16];
};

static Bt_Hci_Packet_Type_Str BT_HCI_PACKET_TYPE_STRING[MAX_BT_TYPE] =
{
	{
		HCI_BDR_NULL, "NULL",
		HCI_BDR_POLL, "POLL",
		HCI_BDR_FHS, "FHS",
		HCI_BDR_DM1, "DM1",
		HCI_BDR_DH1, "DH1",
		HCI_BDR_HV1, "HV1",
		HCI_BDR_HV2, "HV2",
		HCI_BDR_HV3, "HV3",
		HCI_BDR_DV, "DV",
		HCI_BDR_AUX1, "AUX1",
		HCI_BDR_DM3, "DM3",
		HCI_BDR_DH3, "DH3",
		HCI_BDR_EV4, "EV4",
		HCI_BDR_EV5, "EV5",
		HCI_BDR_DM5, "DM5",
		HCI_BDR_DH5, "DH5"
	},
	{
		HCI_EDR_ID, "ID",
		HCI_EDR_INVALID, "INVALID",
		HCI_EDR_RESERVED1, "RESERVED1",
		HCI_EDR_RESERVED2, "RESERVED2",
		HCI_EDR_2DH1, "2-DH1",
		HCI_EDR_EV3, "EV3",
		HCI_EDR_2EV3, "2-EV3",
		HCI_EDR_3EV3, "3-EV3",
		HCI_EDR_3DH1, "3-DH1",
		HCI_EDR_AUX1, "AUX1",
		HCI_EDR_2DH3, "2-DH3",
		HCI_EDR_3DH3,"3-DH3",
		HCI_EDR_2EV5, "2-EV5",
		HCI_EDR_3EV5, "3-EV5",
		HCI_EDR_2DH5, "2-DH5",
		HCI_EDR_3DH5, "3-DH5"
	},
	{
		HCI_RF_PHY_1M, "RF_PHY_1M",
		HCI_RF_PHY_2M, "RF_PHY_2M",
		HCI_RF_PHY_S2, "RF_PHY_S2",
		HCI_RF_PHY_S8, "RF_PHY_S8",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET,"",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, "",
		HCI_INVALID_PACKET, ""
	}
};

// static LPCSTR BT_HCI_PACKET_TYPE_STRING[MAX_BT_TYPE][MAX_HCI_BDR_TYPE] =
// {
// 	{"NULL","POLL","FHS","DM1","DH1","HV1","HV2","HV3","DV","AUX1","DM3","DH3","EV4","EV5","DM5","DH5"},
// 	{"ID","INVALID","RESERVED1","RESERVED2","2-DH1","EV3","2-EV3","3-EV3","3-DH1","AUX1","2-DH3","3-DH3","2-EV5","3-EV5","2-DH5","3-DH5"},
// 	{"RF_PHY_1M","RF_PHY_2M","RF_PHY_S2","RF_PHY_S8","","","","","","","","","","","",""}
// };

static char BT_HCI_PATTERN_STRING[][32] =
{
	"PATTERN_0000",
	"PATTERN_1111",
	"PATTERN_1010",
	"PATTERN_PBRS9",
	"PATTERN_11110000"
};

class CBTTestSysHCI : public CImpBase
{
	 DECLARE_RUNTIME_CLASS(CBTTestSysHCI)
protected:
	CBTTestSysHCI(void);
    virtual ~CBTTestSysHCI(void);

	virtual SPRESULT __InitAction (void);
	virtual SPRESULT __PollAction (void);
	virtual void     __LeaveAction(void);
	virtual BOOL       LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);

	virtual SPRESULT TxSpecialPatternMeas(BT_TYPE eMode, HCIBTMeasParamChan* pUplinkMeasParam, DWORD dwMask, SPBT_RESULT_T* pTestResult);
	virtual BOOL MeasureUplink(BT_TYPE eMode, HCIBTMeasParamChan* pUplinkMeasParam, SPBT_RESULT_T* pTestResult);
	virtual BOOL TestPER(BT_TYPE eMode, HCIBTMeasParamChan* pDownlinkMeasParam, SPWI_VALUES_T* pPER);
	virtual SPRESULT ConfigTesterParam(HCIBTMeasParamChan* pParam);

private:
	BT_HCI_PACKET_TYPE BTGetHciPacket(BT_TYPE eType, LPCSTR lpPacket);
	LPCSTR BTGetPacketHciString(BT_TYPE eType, BT_HCI_PACKET_TYPE ePacket);
	BT_PACKET BTHciPacket2BtPacket(BT_HCI_PACKET_TYPE);

	SPRESULT JudgeMeasRst(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType);
	SPRESULT _JudgeBdrRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult);
	SPRESULT _JudgeEdrRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType);
	SPRESULT _JudgeBleRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult);
	SPRESULT _JudgeBleExRslt(DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, BT_PACKET ePacketType);
	SPRESULT ShowMeasRst(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
	SPRESULT _ShowMeasRstBdr(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
	SPRESULT _ShowMeasRstEdr(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);
	SPRESULT _ShowMeasRstBle(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET /*ePacketType*/);
	SPRESULT _ShowMeasRstBleEx(BT_TYPE eMode, DWORD  dwItemMask, SPBT_RESULT_T* pTestResult, int nChan, double dBSLevel, BT_RFPATH_ENUM ePath, BT_PACKET ePacketType);






private:
	CBTApiHCI			* m_pBTApi;
	SPWI_BT_PARAM_TESTER m_stTester;
	HCIBTMeasParamBand   m_VecBTParamBandImp;
};
