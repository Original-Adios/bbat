#include "StdAfx.h"
#include "WcnTestSysWearAT.h"
#include "wcnUtility.h"
#include "ExtraLogFile.h"
#include <process.h>
#include "SimpleAop.h"

IMPLEMENT_RUNTIME_CLASS(CWcnTestSysWearAT)


extern const int WLAN_TX_TIME;
extern const int WLAN_RX_TIME;
extern const int WLAN_END_TIME;
extern const int WLAN_RATE_TABLE[MAX_WLAN_RATE];//Marlin2
extern const int BT_TX_TIME;
extern const int BT_RX_TIME;
extern const int BT_END_TIME;

CWcnTestSysWearAT::CWcnTestSysWearAT(void)
{
	m_nBtChanCnt = 0;
	m_hThread = NULL;
}


CWcnTestSysWearAT::~CWcnTestSysWearAT(void)
{
}

SPRESULT CWcnTestSysWearAT::RunList()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_rltRunScrip = SP_OK;
	SP_GetProperty(m_hDUT, SP_ATTR_CHANNEL_ATTR, 0, (LPVOID)&m_ca);
	Sleep(2000);
	m_rltRunScrip = WcnRunCase();
	SP_EndPhoneTest(m_hDUT);
	return m_rltRunScrip;
}

SPRESULT CWcnTestSysWearAT::ConfigWlanParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	if(!m_bBt)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	}
	for(int i=0; i< (int)m_vectWlanMeasParam.size(); i++)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiAddCase(m_vectWlanMeasParam[i].stChan.nCenChan, m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate,\
			WLAN_TX_TIME, WLAN_RX_TIME, 0));
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::GetWlanRxResult()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT rlt = SP_OK;
	int nRetCount = 0;
	int nChanCount = (int)m_vectWlanMeasParam.size();
	WIFI_CASE_RLT* pRlt = new WIFI_CASE_RLT[nChanCount];
	WifiGetRlt(pRlt, nChanCount, nRetCount);
	CHKRESULT_WITH_NOTIFY_FUNNAME(WifiGetRlt(pRlt, nChanCount, nRetCount));
	if(nChanCount != nRetCount)
	{
		delete[] pRlt;
		NOTIFY("WLAN RX Count", LEVEL_ITEM, nChanCount, nRetCount, nChanCount);
		return SP_E_WCN_WLAN_PER_FAIL;
	}
	for(int i=0; i< nChanCount; i++)
	{
		if(!IS_BIT_SET(m_vectWlanMeasParam[i].stTesterParamGroupSub.dwMask, WIFI_PER))
		{
			continue;
		}
		int nGood = pRlt[i].nGood;
		double dPER = 0;
		//if (nGood <  m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets)
		{
			dPER = ((double)(m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets - nGood))/(m_vectWlanMeasParam[i].stTesterParamGroupSub.nTotalPackets) * 100.0;
		}

		LPCSTR strCBW = CwcnUtility::WLAN_BW_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.nCBWType];
		LPCSTR strSBW = CwcnUtility::WLAN_BW_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.nSBWType];
		LPCSTR lpRate = CwcnUtility::WlanGetRateString( m_vectWlanMeasParam[i].stTesterParamGroupSub.eRate);
		NOTIFY("SensitivityTest"
			, LEVEL_ITEM|LEVEL_FT
			, 0.0
			, dPER
			, m_vectWlanSpec[m_vectWlanMeasParam[i].eMode].dPer
			, CwcnUtility::WLAN_BAND_NAME[m_vectWlanMeasParam[i].eMode]
		, m_vectWlanMeasParam[i].stChan.nCenChan
			, "-"
			, "%s;%s;CBW:%s;SBW:%s"
			, CwcnUtility::WLAN_ANT_NAME[m_vectWlanMeasParam[i].stTesterParamGroupSub.eAnt]
		, lpRate
			, strCBW
			, strSBW);
		if(!IN_RANGE(0.0, dPER, m_vectWlanSpec[m_vectWlanMeasParam[i].eMode].dPer))
		{
			if(rlt == SP_OK)
			{
				rlt = SP_E_WCN_WLAN_PER_FAIL;
			}
		}
	}
	delete[] pRlt;
	return rlt;
}

SPRESULT CWcnTestSysWearAT::ConfigBtParam()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	m_nBtChanCnt = 0;
	if(!m_bWlan)
	{
		CHKRESULT_WITH_NOTIFY_FUNNAME(WifiClearCase());
	}
	CHKRESULT_WITH_NOTIFY_FUNNAME(BtClearCase());
	for(int i=0; i<(int)m_VecBTParamBandImp.size(); i++)
	{
		BT_PATTERN ePattern = BDR_TX_PBRS9;
		if(m_VecBTParamBandImp[i].eMode == BLE || m_VecBTParamBandImp[i].eMode == BLE_EX)
		{
			ePattern = BLE_TX_PBRS9;
		}
		for(int j=0; j< (int)m_VecBTParamBandImp[i].vecBTFileParamChan.size(); j++)
		{
			CHKRESULT_WITH_NOTIFY_FUNNAME(BtAddCase(m_VecBTParamBandImp[i].eMode, m_VecBTParamBandImp[i].vecBTFileParamChan[j].nCh,\
				m_VecBTParamBandImp[i].vecBTFileParamChan[j].ePacketType, ePattern, BT_TX_TIME, BT_RX_TIME, 0));
			m_nBtChanCnt++;
		}
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::GetBtRxResult()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	SPRESULT rlt = SP_OK;
	int nRetCount = 0;
	int nIndex = 0 ;
	BT_CASE_RLT* pRlt = new BT_CASE_RLT[m_nBtChanCnt];
	BtGetRlt(pRlt, m_nBtChanCnt, nRetCount);
	CHKRESULT_WITH_NOTIFY_FUNNAME(BtGetRlt(pRlt, m_nBtChanCnt, nRetCount));
	if(m_nBtChanCnt != nRetCount)
	{
		delete[] pRlt;
		NOTIFY("BT RX Count", LEVEL_ITEM, m_nBtChanCnt, nRetCount, m_nBtChanCnt);
		return SP_E_WCN_BT_PER_FAIL;
	}
	for(int i=0; i<(int)m_VecBTParamBandImp.size(); i++)
	{
		for(int j=0; j< (int)m_VecBTParamBandImp[i].vecBTFileParamChan.size(); j++,nIndex++)
		{
			if(m_VecBTParamBandImp[i].eMode == BLE || m_VecBTParamBandImp[i].eMode == BLE_EX)
			{
				if(!IS_BIT_SET(m_VecBTParamBandImp[i].vecBTFileParamChan[j].dwMask, BLE_PER))
				{
					continue;
				}
				double dHighLimit = m_VecBTParamBandImp[i].stSpec.stBleSpec.dBer;
				if(m_VecBTParamBandImp[i].eMode == BLE_EX)
				{
					dHighLimit = m_VecBTParamBandImp[i].stSpec.stBleExSpec.dBer;
				}
				double dPer = 0;
				int nGood = pRlt[nIndex].nTotalPackets - pRlt[nIndex].nErrPackets;
				if( nGood < m_VecBTParamBandImp[i].vecBTFileParamChan[j].nTotalPackets)
				{
					dPer = ((double)(m_VecBTParamBandImp[i].vecBTFileParamChan[j].nTotalPackets - nGood))/(m_VecBTParamBandImp[i].vecBTFileParamChan[j].nTotalPackets) * 100.0;
				}
				if(!IN_RANGE(0.0, dPer, dHighLimit))
				{
					if(rlt == SP_OK)
					{
						rlt = SP_E_WCN_BT_PER_FAIL;
					}
				}
				NOTIFY("Sensitivity - PER"
					, LEVEL_ITEM|LEVEL_FT
					, 0.0
					, dPer
					, dHighLimit
					, CwcnUtility::BT_BAND_NAME[m_VecBTParamBandImp[i].eMode]
					, m_VecBTParamBandImp[i].vecBTFileParamChan[j].nCh
					, "-"
					,"%s;BSLevel:%.2f"
					, CwcnUtility::BT_ANT_NAME[ANT_SHARED]
				    , m_VecBTParamBandImp[i].vecBTFileParamChan[j].dVsgLvl
					);
			}
			else
			{
				if(!IS_BIT_SET(m_VecBTParamBandImp[i].vecBTFileParamChan[j].dwMask, BDR_BER))
				{
					continue;
				}
				
				
				double dHighLimit = m_VecBTParamBandImp[i].stSpec.stBdrSpec.dBer;
				if(m_VecBTParamBandImp[i].eMode == EDR)
				{
					dHighLimit = m_VecBTParamBandImp[i].stSpec.stEdrSpec.dBer;
				}
				double dBer = 0;
				if(pRlt[nIndex].nTotalBits < m_VecBTParamBandImp[i].vecBTFileParamChan[j].nTotalPackets)
				{
					dBer = 100.0;
				}
				else
				{
					dBer = 1.0 * pRlt[nIndex].nErrBits / pRlt[nIndex].nTotalBits * 100.0;
				}
				if(!IN_RANGE(0.0, dBer, dHighLimit))
				{
					if(rlt == SP_OK)
					{
						rlt = SP_E_WCN_BT_PER_FAIL;
					}
				}
				NOTIFY("SensitivityTest"
					, LEVEL_ITEM|LEVEL_FT
					, 0.0
					, dBer
					, dHighLimit
					, CwcnUtility::BT_BAND_NAME[m_VecBTParamBandImp[i].eMode]
				    , m_VecBTParamBandImp[i].vecBTFileParamChan[j].nCh
					, "-"
					,"%s;%s;BSLevel:%.2f"
					, CwcnUtility::BT_ANT_NAME[ANT_SHARED]
					, CwcnUtility::BTGetPacketString(m_VecBTParamBandImp[i].eMode, m_VecBTParamBandImp[i].vecBTFileParamChan[j].ePacketType)
					, m_VecBTParamBandImp[i].vecBTFileParamChan[j].dVsgLvl
					);
			}
		}
	}
	delete[] pRlt;
	return rlt;
}

SPRESULT CWcnTestSysWearAT::WifiClearCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTCLRWIFICASE", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiClearCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if(NULL == strstr(szBuff, "+ZCTCLRWIFICASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiClearCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::WifiAddCase( int nChan, E_WLAN_RATE eRate, int nTxDelay, int nRxDelay, int nEndDelay )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	int nRate = WLAN_RATE_TABLE[eRate];
	sprintf_s(szCmd, "AT+ZCTSETWIFICASE=%d,%d,%d,%d,%d", nChan, nRate, nTxDelay, nRxDelay, nEndDelay);
	SPRESULT res = SP_SendATCommand(m_hDUT, szCmd, TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiAddCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if(NULL == strstr(szBuff, "+ZCTSETWIFICASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiAddCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::WifiGetRlt( WIFI_CASE_RLT *pRlt, int nSize, int& nCount )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	nCount = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTWIFIRESULT", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiGetRlt:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	char *pHead = strstr(szBuff, "+ZCTWIFIRESULT:LIST=");
	if(NULL == pHead)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiGetRlt:AT Head return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	pHead += strlen("+ZCTWIFIRESULT:LIST=");
	char *pTail = strstr(szBuff, "\r\nOK\r\n");
	if(NULL == pTail)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WifiGetRlt:AT OK return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	*pTail = '\0';
	INT nItemCount = 0;
	LPSTR *lpItem = GetTokenStringA(pHead, "\r\n", nItemCount);
	if(nItemCount > nSize)
	{
		nItemCount = nSize;
	}

	vector<string> vector('\0');
	for (int i = 0; i < nItemCount; i++)
	{
		vector.push_back(lpItem[i]);
	}

	for(int i=0; i<nItemCount; i++, nCount++)
	{
		int nDataCount = 0;
		INT *lpData = GetTokenIntegerA(vector[i].c_str(), ",", nDataCount);
		if(NULL == lpData || nDataCount < 7)
		{
			LogFmtStrA(SPLOGLV_ERROR, "Data Count %d WifiGetRlt:AT return error!", nDataCount);
			return SP_E_WCN_WLAN_ATCMD_ERROR;
		}
		pRlt[i].nIndex = lpData[0];
		pRlt[i].nCh = lpData[1];
		pRlt[i].nRate = lpData[2];
		pRlt[i].nRssi = lpData[3];
		pRlt[i].nGood = lpData[4];
		pRlt[i].nErr = lpData[5];
		pRlt[i].nFail = lpData[6];
		LogFmtStrA(SPLOGLV_INFO, "Index=%d  Ch=%d Rate=%d Rssi=%d Good=%d Err=%d  Fail=%d", 
			pRlt[i].nIndex, pRlt[i].nCh, pRlt[i].nRate, pRlt[i].nRssi, pRlt[i].nGood, pRlt[i].nErr, pRlt[i].nFail);
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::BtClearCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTCLRBTCASE", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtClearCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if(NULL == strstr(szBuff, "+ZCTCLRBTCASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtClearCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::BtAddCase( BT_TYPE eModeType, int nChan,  BT_PACKET ePacketType,  BT_PATTERN ePattern,int nTxDelay, int nRxDelay, int nEndDelay )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szCmd[256];
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	int nPacket = GetPacketType(eModeType, ePacketType);
	int nMode = 2;
	if(eModeType == BDR || eModeType == EDR)
	{
		nMode = 1;
	}
	sprintf_s(szCmd, " AT+ZCTSETBTCASE=%d,%d,%d,%d,%d,%d,%d,%d", nMode, ePattern, nChan, nPacket, 20, nTxDelay, nRxDelay, nEndDelay);
	SPRESULT res = SP_SendATCommand(m_hDUT, szCmd, TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtAddCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	if(NULL == strstr(szBuff, "+ZCTSETBTCASE:OK"))
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtAddCase:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::BtGetRlt( BT_CASE_RLT *pRlt, int nSize, int& nCount )
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	nCount = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTBTRESULT", TRUE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtGetRlt:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	char *pHead = strstr(szBuff, "+ZCTBTRESULT:LIST=");
	if(NULL == pHead)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtGetRlt:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	pHead += strlen("+ZCTBTRESULT:LIST=");
	char *pTail = strstr(szBuff, "\r\nOK\r\n");
	if(NULL == pTail)
	{
		LogFmtStrA(SPLOGLV_ERROR, "BtGetRlt:AT return error!");
		return SP_E_WCN_WLAN_ATCMD_ERROR;
	}
	*pTail = '\0';
	INT nItemCount = 0;
	LPSTR *lpItem = GetTokenStringA(pHead, "\r\n", nItemCount);
	if(nItemCount > nSize)
	{
		nItemCount = nSize;
	}
	for(int i=0; i<nItemCount; i++, nCount++)
	{
		int nDataCount = 0;
		INT *lpData = GetTokenIntegerA(lpItem[i], ",", nDataCount);
		if(NULL == lpData || nDataCount < 6)
		{
			LogFmtStrA(SPLOGLV_ERROR, "BtGetRlt:AT return error!");
			return SP_E_WCN_WLAN_ATCMD_ERROR;
		}
		pRlt[i].nIndex = lpData[0];
		pRlt[i].nErrBits = lpData[1];
		pRlt[i].nTotalBits = lpData[2];
		pRlt[i].nErrPackets = lpData[3];
		pRlt[i].nTotalPackets = lpData[4];
		pRlt[i].nRssi = lpData[5];
	}
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::WcnRunCase()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	char szBuff[1024] = {0};
	uint32 nRet = 0;
	SPRESULT res = SP_SendATCommand(m_hDUT, "AT+ZCTTRIGER", FALSE, szBuff, sizeof(szBuff), &nRet, 5000);
	if(SP_OK != res)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WcnRunCase:SP_SendATCommand failed, nErr = 0x%X", res);
		return res;
	}
	/*if(NULL == strstr(szBuff, "+ZCTTRIGER:OK"))
	{
	LogFmtStrA(SPLOGLV_ERROR, "WcnRunCase:AT return error!");
	return SP_E_WCN_WLAN_ATCMD_ERROR;
	}*/

	Sleep(m_nTriggerTimeOut);
	LogFmtStrA(SPLOGLV_INFO, "Wait for After DUT ZCTTRIGER %d(ms)", m_nTriggerTimeOut);
	return SP_OK;
}

SPRESULT CWcnTestSysWearAT::DoList()
{
	auto _function = SimpleAop(this, __FUNCTION__);
	/*if(NULL != m_hThread)
	{
		CloseHandle(m_hThread);
	}
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &RunListThreadAT, this, 0, NULL);
	if(NULL == m_hThread)
	{
		LogFmtStrA(SPLOGLV_ERROR, "_beginthreadex failed ");
		return SP_E_CREATE_OBJECT;
	}
	return SP_OK;*/
	m_rltRunScrip = SP_OK;
	SP_GetProperty(m_hDUT, SP_ATTR_CHANNEL_ATTR, 0, (LPVOID)&m_ca);
	m_rltRunScrip = WcnRunCase();
	SP_EndPhoneTest(m_hDUT);
	m_dwTime = GetTickCount();
	return m_rltRunScrip;
}

unsigned _stdcall CWcnTestSysWearAT::RunListThreadAT( LPVOID pParam )
{
	if(NULL == pParam)
	{
		return 0;
	}
	CWcnTestSysWearAT *pThis = (CWcnTestSysWearAT *)pParam;
	pThis->RunList();
	return 0;
}



