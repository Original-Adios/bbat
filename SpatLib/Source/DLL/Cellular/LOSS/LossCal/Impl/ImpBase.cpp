#include "StdAfx.h"
#include "ImpBase.h"
#include <assert.h>
#include "ModeSwitch.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase( void )
	: m_pGsmFunc( NULL )
	, m_pTdFunc( NULL )
	, m_pWcdmaFunc( NULL )
	, m_pC2KFunc( NULL )
	, m_pLteFunc( NULL )
	, m_bWcdmaSelected( FALSE )
	, m_bTdSelected( FALSE )
	, m_bGsmSelected( FALSE )
	, m_bC2KSelected( FALSE )
	, m_bNrSelected(FALSE)
	, m_bDMRFTSelected( FALSE )
	, m_bDMRCalSelected( FALSE )
	  //, m_bLteSelected(FALSE)
	, m_dLossValLower( 0 )
	, m_dLossValUpper( 0 )
	, m_dWcdmaTargetPwr( 0 )
	, m_bLossCheck( FALSE )
	, m_dLossCheckSpec( 0.3 )
	, m_eModemVer( MV_V2 )
	, m_dPreLoss( 0.0 )
	, m_bLteSelected( FALSE )
	, m_dC2KTargetPwrConf( 0.0 )
	, m_dTdTargetPwr( 0.0 )
	, m_nDmrFreqCnt( 0 )
	, m_dDmrTargetPwr( 0.0 )
	, m_nDmrPcl( 0 )
	, m_bWcnAnt( FALSE )
	, m_dWlanRefLvl( 0.0 )
	, m_dBtRefLvl( 0.0 )
	, m_bWlanSelected( FALSE )
	, m_bBtSelected( FALSE )
	, m_bGpsSelected( FALSE )
	, m_pDMRCalFunc( NULL )
	, m_pDMRFtFunc( NULL )
	, m_pNrFunc(NULL)
	, m_pWCNFunc( NULL )
	, m_eCSType(WCN_CS_TYPE_SC265X)
{
	ZeroMemory( &m_gs, sizeof( m_gs ) );
	ZeroMemory( &m_lossVal, sizeof( m_lossVal ) );
	ZeroMemory( m_bWcdmaBandNum, sizeof( m_bWcdmaBandNum ) );
	ZeroMemory( m_bTdBandNum, sizeof( m_bTdBandNum ) );
	ZeroMemory( m_bGsmBandNum, sizeof( m_bGsmBandNum ) );
	ZeroMemory( m_bLteBandNum, sizeof( m_bLteBandNum ) );
	ZeroMemory( m_szGSBuf, sizeof( m_szGSBuf ) );
	ZeroMemory( m_bWlanBandNum, sizeof( m_bWlanBandNum ) );
	ZeroMemory( m_bBTBandNum, sizeof( m_bBTBandNum ) );
	ZeroMemory( m_eWcdmaBandAnt, sizeof( m_eWcdmaBandAnt ) );
	ZeroMemory(m_Gps_Setting, sizeof(m_Gps_Setting));
	m_strNegativeLossDetected = "";
}

CImpBase::~CImpBase( void )
{
}

SPRESULT CImpBase::__FinalAction( void )
{
	if ( m_pGsmFunc != NULL )
	{
		m_pGsmFunc->Release();
		delete m_pGsmFunc;
		m_pGsmFunc = NULL;
	}

	if ( m_pTdFunc != NULL )
	{
		m_pTdFunc->Release();
		delete m_pTdFunc;
		m_pTdFunc = NULL;
	}


	if ( m_pWcdmaFunc != NULL )
	{
		m_pWcdmaFunc->Release();
		delete m_pWcdmaFunc;
		m_pWcdmaFunc = NULL;
	}


	if ( m_pC2KFunc != NULL )
	{
		m_pC2KFunc->Release();
		delete m_pC2KFunc;
		m_pC2KFunc = NULL;
	}


	if ( m_pLteFunc != NULL )
	{
		m_pLteFunc->Release();
		delete m_pLteFunc;
		m_pLteFunc = NULL;
	}


	if ( m_pWCNFunc != NULL )
	{
		m_pWCNFunc->Release();
		delete m_pWCNFunc;
		m_pWCNFunc = NULL;
	}

	if ( m_pDMRCalFunc != NULL )
	{
		delete m_pDMRCalFunc;
		m_pDMRCalFunc = NULL;
	}

	if ( m_pDMRFtFunc != NULL )
	{
		delete m_pDMRFtFunc;
		m_pDMRFtFunc = NULL;
	}

	return SP_OK;
}

SPRESULT CImpBase::__PollAction( void )
{
	RFDEVICE_INFO devInfo;
	CHKRESULT_WITH_NOTIFY(m_pRFTester->GetProperty(DP_CABLE_LOSS_EX, 0, (LPVOID)&m_lossVal),
		"Get instrument property");

	return SP_OK;
}

SPRESULT CImpBase::LoadFromPhone()
{
	GS_DATA_HEADER Header;
	CHKRESULT(GetMagicType(Header));

	switch(Header.nMagicNum)
	{
	case GS19:
	case GS20:
	case GS21:
		CHKRESULT(LoadFromPhonePhaseCheckArea( &Header ));
		break;
	case GS30:
		CHKRESULT(LoadFromPhoneCusArea( &Header ));
		break;
	default:
		LogFmtStrA( SPLOGLV_ERROR, "Golden sample Magic Num(0x%x) is invalid, . Please check.", Header.nMagicNum );
		LogFmtStrA( SPLOGLV_ERROR, "This is not a valid golden sample." );
		return SP_E_SPAT_LOSS_LOAD_FROM_PHONE;
	}

	return SP_OK;
}

void CImpBase::ConvertRawDataToGS(int nStartAddr)
{
	int nLen = 0;
	if ( m_gs.common.nGsmCnt > 0 )
	{
		nLen = sizeof( m_gs.gsm[0] ) * m_gs.common.nGsmCnt;
		CopyMemory( m_gs.gsm, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}

	if ( m_gs.common.nWcdmaCnt > 0 )
	{
		nLen = sizeof( m_gs.wcdma[0] ) * m_gs.common.nWcdmaCnt;
		CopyMemory( m_gs.wcdma, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}

	if ( m_gs.common.usLteCnt > 0 )
	{
		nLen = m_gs.common.usLteCnt;
		m_arrLteInfo.resize( nLen );
		CopyMemory( &m_arrLteInfo[0], &m_szGSBuf[nStartAddr], nLen );
		m_gs.common.usLteCnt = (uint16)nLen;
		nStartAddr += nLen;
	}

	if ( m_gs.common.nTdCnt > 0 )
	{
		nLen = sizeof( m_gs.td[0] ) * m_gs.common.nTdCnt;
		CopyMemory( m_gs.td, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}
	if ( m_gs.common.nC2KCnt > 0 )
	{
		nLen = sizeof( m_gs.c2k[0] ) * m_gs.common.nC2KCnt;
		CopyMemory( m_gs.c2k, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}

	if ( m_gs.common.nDmrCnt > 0 )
	{
		nLen = sizeof( m_gs.dmr[0] ) * m_gs.common.nDmrCnt;
		CopyMemory( m_gs.dmr, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}
	if ( m_gs.common.nWlanCnt > 0 )
	{
		nLen = sizeof( m_gs.wlan[0] ) * m_gs.common.nWlanCnt;
		CopyMemory( m_gs.wlan, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}
	if ( m_gs.common.nBtCnt > 0 )
	{
		nLen = sizeof( m_gs.bt[0] ) * m_gs.common.nBtCnt;
		CopyMemory( m_gs.bt, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}
	if ( m_gs.common.nGpsCnt > 0 )
	{
		nLen = sizeof( m_gs.gps[0] ) * m_gs.common.nGpsCnt;
		CopyMemory( m_gs.gps, &m_szGSBuf[nStartAddr], nLen );
		nStartAddr += nLen;
	}
}

void CImpBase::LoadFromPhone_R6()
{
	int nStartAddr = 0;
	int nLen = sizeof( GS_COMMON_R6 );
	GS_COMMON_R6 common_r6;
	CopyMemory( &common_r6, &m_szGSBuf[nStartAddr], nLen );

	m_gs.common.u32TotalBytes = common_r6.u32TotalBytes;
	m_gs.common.nAntCount = common_r6.nAntCount;
	m_gs.common.nBtCnt = common_r6.nBtCnt;
	m_gs.common.nC2KCnt = 0;
	m_gs.common.nDmrCnt = common_r6.nDmrCnt;
	m_gs.common.nGpsCnt = common_r6.nGpsCnt;
	m_gs.common.nGsmCnt = common_r6.nGsmCnt;
	m_gs.common.nMagicNum = common_r6.nMagicNum;
	m_gs.common.nTdCnt = common_r6.nTdCnt;
	m_gs.common.nWcdmaCnt = common_r6.nWcdmaCnt;
	m_gs.common.nWlanCnt = common_r6.nWlanCnt;
	m_gs.common.ts = common_r6.ts;
	m_gs.common.usLteCnt = common_r6.usLteCnt;

	nStartAddr += nLen;
	ConvertRawDataToGS(nStartAddr);
}

void CImpBase::LoadFromPhone_R7()
{
	int nStartAddr = 0;
	int nLen = sizeof( m_gs.common );
	CopyMemory( &m_gs.common, &m_szGSBuf[nStartAddr], nLen );

	nStartAddr += nLen;
	ConvertRawDataToGS(nStartAddr);
}

void CImpBase::LoadFromPhone_R8(uint32 nTotalSize)
{
	uint32 nStartAddr = 0;
	int nLen = sizeof(GS_DATA_HEADER);
	nStartAddr += nLen;
	GS_RAT_HEAD ratHead;

	while(nStartAddr < nTotalSize)
	{
		CopyMemory(&ratHead, &m_szGSBuf[nStartAddr], sizeof(ratHead));
		nStartAddr += sizeof(ratHead);
		if ( ratHead.rat == (uint16)GS_GSM )
		{
			m_gs.common.nGsmCnt = (uint8)(ratHead.size / sizeof(m_gs.gsm[0]));
			CopyMemory( m_gs.gsm, &m_szGSBuf[nStartAddr], ratHead.size );
		}

		else if ( ratHead.rat == (uint16)GS_WCDMA )
		{
			m_gs.common.nWcdmaCnt = (uint8)(ratHead.size / sizeof(m_gs.wcdma[0]));
			CopyMemory( m_gs.wcdma, &m_szGSBuf[nStartAddr], ratHead.size );
		}

		else if ( ratHead.rat == (uint16)GS_LTE )
		{
			nLen = ratHead.size;
			m_arrLteInfo.resize( nLen );
			CopyMemory( &m_arrLteInfo[0], &m_szGSBuf[nStartAddr], nLen );
			m_gs.common.usLteCnt = (uint16)nLen;
		}

		else if ( ratHead.rat == (uint16)GS_NR )
		{
			nLen = ratHead.size;
			m_arrNrInfo.resize( nLen );
			CopyMemory( &m_arrNrInfo[0], &m_szGSBuf[nStartAddr], nLen );
			m_gs.common.usNrCnt = (uint16)nLen;
		}
		else if ( ratHead.rat == (uint16)GS_TD )
		{
			m_gs.common.nTdCnt = (uint8)(ratHead.size / sizeof(m_gs.td[0]));
			CopyMemory( m_gs.td, &m_szGSBuf[nStartAddr], ratHead.size );
		}
		else if ( ratHead.rat == (uint16)GS_C2K )
		{
			m_gs.common.nC2KCnt = (uint8)(ratHead.size / sizeof(m_gs.c2k[0]));
			CopyMemory( m_gs.c2k, &m_szGSBuf[nStartAddr], ratHead.size );
		}

		else if ( ratHead.size == (uint16)GS_DMR )
		{
			m_gs.common.nDmrCnt = (uint8)(ratHead.size / sizeof(m_gs.dmr[0]));
			CopyMemory( m_gs.dmr, &m_szGSBuf[nStartAddr], ratHead.size );
		}
		else if ( ratHead.rat == (uint16)GS_WLAN )
		{
			m_gs.common.nWlanCnt = (uint8)(ratHead.size / sizeof(m_gs.wlan[0]));
			CopyMemory( m_gs.wlan, &m_szGSBuf[nStartAddr], ratHead.size );
		}
		else if ( ratHead.rat == (uint16)GS_BT )
		{
			m_gs.common.nBtCnt = (uint8)(ratHead.size / sizeof(m_gs.bt[0]));
			CopyMemory( m_gs.bt, &m_szGSBuf[nStartAddr], ratHead.size );
		}
		else if ( ratHead.rat == (uint16)GS_GPS )
		{
			m_gs.common.nGpsCnt = (uint8)(ratHead.size / sizeof(m_gs.gps[0]));
			CopyMemory( m_gs.gps, &m_szGSBuf[nStartAddr], ratHead.size );
		}
		nStartAddr += ratHead.size;
	}
}

void CImpBase::CopyBufData( int nLen, GS_RAT rat, void* pData, uint32 &nTotalLen )
{
	GS_RAT_HEAD ratHead;
	ratHead.rat = (uint16)rat;
	ratHead.size = (uint16)nLen;
	CopyMemory(&m_szGSBuf[nTotalLen], &ratHead, sizeof(ratHead));
	nTotalLen += sizeof(ratHead);
	CopyMemory( &m_szGSBuf[nTotalLen], pData, nLen );
	nTotalLen += nLen;
}

SPRESULT CImpBase::SaveToPhone()
{
	LogFmtStrA( SPLOGLV_INFO, "Writing information into golden sample..." );
	GetLocalTime( &m_gs.common.ts );
	m_gs.common.nMagicNum = GS_VERSION;
	uint32 nTotalLen = 0; // 20180322 @JXP
	int nLen = sizeof( m_gs.common );

	GS_DATA_HEADER dataHead;
	dataHead.nMagicNum = m_gs.common.nMagicNum;
	dataHead.ts = m_gs.common.ts;
	dataHead.u32TotalBytes = 0;

	nLen = sizeof(dataHead);
	CopyMemory( &m_szGSBuf[nTotalLen], &dataHead, nLen );
	nTotalLen += nLen;
	GS_RAT_HEAD ratHead;
	if ( m_gs.common.nGsmCnt > 0 )
	{
		nLen = sizeof(m_gs.gsm[0]) * m_gs.common.nGsmCnt;
		CopyBufData(nLen, GS_GSM, m_gs.gsm, nTotalLen);
	}

	if ( m_gs.common.nWcdmaCnt > 0 )
	{
		nLen = sizeof( m_gs.wcdma[0] ) * m_gs.common.nWcdmaCnt;
		CopyBufData(nLen, GS_WCDMA, m_gs.wcdma, nTotalLen);
	}

	if ( m_arrLteInfo.size() > 0 )
	{
		nLen = m_arrLteInfo.size();
		CopyBufData(nLen, GS_LTE, &m_arrLteInfo[0], nTotalLen);
	}

	if ( m_gs.common.nTdCnt > 0 )
	{
		nLen = sizeof( m_gs.td[0] ) * m_gs.common.nTdCnt;
		CopyBufData(nLen, GS_TD, m_gs.td, nTotalLen);
	}
	if ( m_gs.common.nC2KCnt > 0 )
	{
		nLen = sizeof( m_gs.c2k[0] ) * m_gs.common.nC2KCnt;
		CopyBufData(nLen, GS_C2K, m_gs.c2k, nTotalLen);
	}
	if ( m_gs.common.nDmrCnt > 0 )
	{
		nLen = sizeof( m_gs.dmr[0] ) * m_gs.common.nDmrCnt;
		CopyBufData(nLen, GS_DMR, m_gs.dmr, nTotalLen);
	}

	if ( m_gs.common.nWlanCnt > 0 )
	{
		nLen = sizeof( m_gs.wlan[0] ) * m_gs.common.nWlanCnt;
		CopyBufData(nLen, GS_WLAN, m_gs.wlan, nTotalLen);
	}

	if ( m_gs.common.nBtCnt > 0 )
	{
		nLen = sizeof( m_gs.wlan[0] ) * m_gs.common.nBtCnt;
		CopyBufData(nLen, GS_BT, m_gs.bt, nTotalLen);
	}

	if ( m_gs.common.nGpsCnt > 0 )
	{
		nLen = sizeof( m_gs.wlan[0] ) * m_gs.common.nGpsCnt;
		CopyBufData(nLen, GS_GPS, m_gs.gps, nTotalLen);
	}
	if ( m_arrNrInfo.size() > 0 )
	{
		nLen = m_arrNrInfo.size();
		CopyBufData(nLen, GS_NR, &m_arrNrInfo[0], nTotalLen);
	}

	if (nTotalLen < MAX_GOLDEN_SAMPLE_SIZE)
	{
		CHKRESULT(SaveToPhaseCheckArea(&dataHead, nTotalLen));
	}
	else if ( nTotalLen > MAX_GOLDEN_SAMPLE_SIZE && nTotalLen < MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE )
	{
		dataHead.nMagicNum = GS30;
		CHKRESULT(SaveToCustomerArea(&dataHead, nTotalLen))
	}
	else
	{
		LogFmtStrA( SPLOGLV_ERROR, "The size of struct(%d) is larger than %dBytes. Please check.", nTotalLen, MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE );
		assert( 0 );
		return SP_E_SPAT_LOSS_SIZE_INVALID;
	}

	return SP_OK;
}

BOOL CImpBase::CheckLossDelta(const char* czModeName, RF_CABLE_LOSS_UNIT_EX* pOldLoss, RF_CABLE_LOSS_UNIT_EX* pCurLoss)
{

	static const LPCSTR ANT_NAME[MAX_RF_ANT] = {"Primary","Diversity","RF_ANT_3rd","RF_ANT_4th","RF_ANT_5th","RF_ANT_6th","RF_ANT_7th","RF_ANT_8th"};

	static const LPCSTR IO_NAME[MAX_RF_IO] = {"TX", "RX"};

	if ( pOldLoss->nCount != pCurLoss->nCount )
	{
		LogFmtStrA( SPLOGLV_ERROR, "The count of loss unit is mismatch." );
		return FALSE;
	}
	for ( int i = 0; i < pOldLoss->nCount; i++ )
	{
		for ( int j = RF_ANT_1st; j < MAX_RF_ANT; j++ )
		{
			for ( int k = RF_IO_TX; k < MAX_RF_IO; k++ )
			{
				double dFabsDelta = fabs( pOldLoss->arrPoint[i].dLoss[j][k] - pCurLoss->arrPoint[i].dLoss[j][k] );
				NOTIFY( "Loss Check", LEVEL_ITEM, -m_dLossCheckSpec, dFabsDelta, m_dLossCheckSpec, NULL, -1, "db", "Mode = %s;ANT = %s;IO = %s;Freq = %.2f", czModeName, ANT_NAME[j], IO_NAME[k], pOldLoss->arrPoint[i].dFreq);
				if ( dFabsDelta > m_dLossCheckSpec )
				{
					LogFmtStrA( SPLOGLV_ERROR, "Mode = %s, ANT = %s, IO = %s, Freq = %.2f, 1:%.2f, 2:%.2f, Delta:%.2f > %.2f.", czModeName, ANT_NAME[j], IO_NAME[k], pOldLoss->arrPoint[i].dFreq, pOldLoss->arrPoint[i].dLoss[j][k], pCurLoss->arrPoint[i].dLoss[j][k], dFabsDelta, m_dLossCheckSpec);
					LogFmtStrA( SPLOGLV_ERROR, "The delta of tow loss is out of range." );
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

SPRESULT CImpBase::UpgradeLoss()
{
	RF_CABLE_LOSS_COMB_EX lossCom;
	ICallback* pCallBack = GetCallback();

	if ( m_gs.common.nGsmCnt > 0 && m_bGsmSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "GSM", &m_lossOrg.gsmLoss, &m_lossVal.gsmLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_GSM;
		m_lossVal.gsmLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.gsmLoss, sizeof(m_lossVal.gsmLoss));
		pCallBack->UiMsgUpdateLoss( &lossCom );
	}

	if ( m_gs.common.nWcdmaCnt > 0 && m_bWcdmaSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "WCDMA", &m_lossOrg.wcdmaLoss, &m_lossVal.wcdmaLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_WCDMA;
		m_lossVal.wcdmaLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.wcdmaLoss, sizeof(m_lossVal.wcdmaLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}

	if ( m_lossVal.lteLoss.nCount > 0 && m_bLteSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "LTE", &m_lossOrg.lteLoss, &m_lossVal.lteLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_LTE;
		m_lossVal.lteLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.lteLoss, sizeof(m_lossVal.lteLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	if ( m_lossVal.nrLoss.nCount > 0 && m_bNrSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "NR", &m_lossOrg.nrLoss, &m_lossVal.nrLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_NR;
		m_lossVal.nrLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.nrLoss, sizeof(m_lossVal.nrLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}

	if ( m_lossVal.tdLoss.nCount > 0 && m_bTdSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "TDSCDMA", &m_lossOrg.tdLoss, &m_lossVal.tdLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_TD;
		m_lossVal.tdLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.tdLoss, sizeof(m_lossVal.tdLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}

	if ( m_lossVal.cdmaLoss.nCount > 0 && m_bC2KSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "CDMA", &m_lossOrg.cdmaLoss, &m_lossVal.cdmaLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_C2K;
		m_lossVal.cdmaLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.cdmaLoss, sizeof(m_lossVal.cdmaLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}

	if ( m_lossVal.otherLoss.nCount > 0 && m_bDMRCalSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "DMR",&m_lossOrg.otherLoss, &m_lossVal.otherLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_DMR;
		m_lossVal.otherLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.otherLoss, sizeof(m_lossVal.otherLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	if ( m_lossVal.otherLoss.nCount > 0 && m_bDMRFTSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "DMRFT",&m_lossOrg.otherLoss, &m_lossVal.otherLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_DMR;
		m_lossVal.otherLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.otherLoss, sizeof(m_lossVal.otherLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	if ( m_lossVal.wlanLoss.nCount > 0 && m_bWlanSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "Wlan",&m_lossOrg.wlanLoss, &m_lossVal.wlanLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_WLAN;
		m_lossVal.wlanLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.wlanLoss, sizeof(m_lossVal.wlanLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	if ( m_lossVal.btLoss.nCount > 0 && m_bBtSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "BT", &m_lossOrg.btLoss, &m_lossVal.btLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_BT;
		m_lossVal.btLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.btLoss, sizeof(m_lossVal.btLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	if ( m_lossVal.gpsLoss.nCount > 0 && m_bGpsSelected )
	{
		if ( m_bLossCheck )
		{
			if ( !CheckLossDelta( "Gps", &m_lossOrg.gpsLoss, &m_lossVal.gpsLoss ) )
			{
				return SP_E_SPAT_LOSS_CONSISTENCE_OUT_OF_RANGE;
			}
		}
		lossCom.nModeID = LOSS_MODE_GPS;
		m_lossVal.gpsLoss.LossType = BAND_TYPE;
		memcpy_s(&lossCom.tLossUnit, sizeof(lossCom.tLossUnit), &m_lossVal.gpsLoss, sizeof(m_lossVal.gpsLoss));
		pCallBack->UiMsgUpdateLoss(&lossCom);
	}
	return m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, ( LPCVOID )&m_lossVal );
}

SPRESULT CImpBase::ChangeMode( RM_MODE_ENUM eMode )
{
	return m_pModeSwitch->Change( eMode );
}

void CImpBase::ZeroLossValue(RF_CABLE_LOSS_EX& loss )
{
	/// GSM
	if ( m_gs.common.nGsmCnt > 0 )
	{
		for ( int i = 0; i < loss.gsmLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.gsmLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.gsmLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}


	/// TD
	if ( m_gs.common.nTdCnt > 0 )
	{
		for ( int i = 0; i < loss.tdLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.tdLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.tdLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}


	/// WCDMA
	if ( m_gs.common.nWcdmaCnt > 0 )
	{
		for ( int i = 0; i < loss.wcdmaLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.wcdmaLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.wcdmaLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}

	///C2K
	if ( m_gs.common.nC2KCnt > 0 )
	{
		for ( int i = 0; i < loss.cdmaLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.cdmaLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.cdmaLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}

	/// LTE
	if ( m_gs.common.usLteCnt > 0 )
	{
		for ( int i = 0; i < loss.lteLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.lteLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.lteLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}
	///DMR
	if ( m_gs.common.nDmrCnt > 0 )
	{
		for ( int i = 0; i < loss.otherLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.otherLoss.arrPoint[i].dLoss[j][RF_IO_TX]  = 0;
				loss.otherLoss.arrPoint[i].dLoss[j][RF_IO_RX]  = 0;
			}
		}
	}

	/// WCN
	if ( m_gs.common.nWlanCnt > 0 )
	{
		for ( int i = 0; i < loss.wlanLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.wlanLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.wlanLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}
	if ( m_gs.common.nBtCnt > 0 )
	{
		for ( int i = 0; i < loss.btLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.btLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.btLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}
	if ( m_gs.common.nGpsCnt > 0 )
	{
		for ( int i = 0; i < loss.gpsLoss.nCount; i++ )
		{
			for ( int j = 0; j < MAX_RF_ANT; j++ )
			{
				loss.gpsLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.gpsLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}
		}
	}

}

SPRESULT CImpBase::CheckLoss( double dLoss, string strMode )
{
	if (dLoss < 0.0)
	{
		if ( string::npos == m_strNegativeLossDetected.find(strMode))
		{
			m_strNegativeLossDetected += strMode;
			m_strNegativeLossDetected += ",";
		}
	}

	if ( dLoss <= m_dLossValLower || dLoss > m_dLossValUpper )
	{
		LogFmtStrA( SPLOGLV_ERROR, "Loss value(%.2f) is out of range(%.2f, %.2f)", dLoss, m_dLossValLower, m_dLossValUpper );
		return SP_E_SPAT_LOSS_OUT_OF_RANGE;
	}
	return SP_OK;
}

void CImpBase::GetLossFmtStr( SP_MODE_INFO eMode, std::wstring& strFmt, BOOL bExp, BOOL bTx)
{
	WCHAR szTmp[1024] = {0};
	strFmt.clear();
	switch (eMode)
	{
	case SP_GSM:
		if(bExp)
		{
			strFmt = L"Band:%d,Arfcn:%d,Freq:%.2lf,Data:";
		}
		else
		{
			strFmt = L"Band:%d,Arfcn:%d,Freq:%lf,Data:";
		}
		for(int j=0; j<DUL_ANT; j++)
		{
			if(bExp)
			{
				swprintf_s(szTmp, L"Ant%d{Loss:%%d,PCL:%%d,Power:%%.2lf} ", j+1);
			}
			else
			{
				swprintf_s(szTmp, L"Ant%d{Loss:%%d,PCL:%%d,Power:%%lf} ", j+1);
			}

			strFmt += szTmp;
		}
		break;
	case SP_TDSCDMA:
	case SP_WCDMA:
	case SP_C2K:
		if(bExp)
		{
			strFmt = L"Band:%d,Arfcn:%d,Freq:%.2lf,Data:";
		}
		else
		{
			strFmt = L"Band:%d,Arfcn:%d,Freq:%lf,Data:";
		}
		for(int j=0; j<DUL_ANT; j++)
		{
			if(bExp)
			{
				swprintf_s(szTmp, L"Ant%d{Loss:%%d,Index:%%d,Power:%%.2lf} ", j+1);
			}
			else
			{
				swprintf_s(szTmp, L"Ant%d{Loss:%%d,Index:%%d,Power:%%lf} ", j+1);
			}
			strFmt += szTmp;
		}
		break;
	case SP_LTE:
		if(bTx)
		{
			if(bExp)
			{
				strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%.2lf,Path:%d,Data:Ant%d{TxPower:%.2lf,TxFactor:%d}";
			}
			else
			{
				strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%lf,Path:%d,Data:Ant%d{TxPower:%lf,TxFactor:%d}";
			}
		}
		else
		{
			if(bExp)
			{
				strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%.2lf,Path:%d,Data:Ant%d{RxRssi:%.2lf,RxCellPower:%.2lf,RxIndex:%d}";
			}
			else
			{
				strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%lf,Path:%d,Data:Ant%d{RxRssi:%lf,RxCellPower:%lf,RxIndex:%d}";
			}
		}
		break;
	case SP_NR:
		if (bExp)
		{
			strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%.2lf,Path:%d,Data:RfAnt%d Ant%d {nParam:%d, dPower:%.2lf, dResult:%.2lf}";
		}
		else
		{
			strFmt = L"Index:%d,Band:%d,Arfcn:%d,Freq:%lf,Path:%d,Data:RfAnt%d Ant%d {nParam:%d, dPower:%lf, dResult:%lf}";
		}
		break;
	case SP_WIFI:
	case SP_BT:
	case SP_GPS:
		strFmt = L"Chan:%d,Protocol:%d,Data:";
		for(int j=0; j<WCN_ANT; j++)
		{
			if(bExp)
			{
				swprintf_s(szTmp, L" Ant%d{Path:%%d,Power:%%.2lf}", j+1);
			}
			else
			{
				swprintf_s(szTmp, L" Ant%d{Path:%%d,Power:%%lf}", j+1);
			}
			strFmt += szTmp;
		}
		break;
	default:
		break;
	}
	return;
}

SPRESULT CImpBase::GetMagicType( GS_DATA_HEADER &Header )
{
	LogFmtStrA( SPLOGLV_INFO, "Reading golden sample information..." );
	//Get Magic Number
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP05, &Header, sizeof( Header ), TIMEOUT_20S ) );

	m_gs.common.nMagicNum = Header.nMagicNum;
	m_gs.common.ts = Header.ts;
	m_gs.common.u32TotalBytes = Header.u32TotalBytes;

	if ( Header.u32TotalBytes < MAX_GOLDEN_SAMPLE_SIZE )
	{
		LogFmtStrA( SPLOGLV_INFO, "Golden data restored in phasecheck area!" );
	}
	else if (Header.u32TotalBytes < MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE && Header.u32TotalBytes > MAX_GOLDEN_SAMPLE_SIZE)
	{
	   LogFmtStrA( SPLOGLV_INFO, "Golden data restored in customer area!" );
	}
	else
	{
		LogFmtStrA( SPLOGLV_ERROR, "The size of struct(%d) is not right.  Please check.", Header.u32TotalBytes );
		assert( 0 );
		return SP_E_SPAT_LOSS_SIZE_INVALID;
	}

	return SP_OK;
}

SPRESULT CImpBase::LoadFromPhonePhaseCheckArea( GS_DATA_HEADER *pHeader )
{
	LogFmtStrA( SPLOGLV_INFO, "Reading golden sample information..." );
	if (NULL == pHeader)
	{
		LogFmtStrA( SPLOGLV_ERROR, "Input param pointer is null!" );
		return SP_E_INVALID_PARAMETER;
	}

	//m_gs.common.nMagicNum = pHeader->nMagicNum;
	//m_gs.common.ts = pHeader->ts;
	//m_gs.common.u32TotalBytes = pHeader->u32TotalBytes;
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP05, ( LPVOID )m_szGSBuf, pHeader->u32TotalBytes, TIMEOUT_20S ) );

	switch(pHeader->nMagicNum)
	{
	case GS19:
		LoadFromPhone_R6();
		break;
	case GS20:
		LoadFromPhone_R7();
		break;
	case GS21:
		LoadFromPhone_R8(pHeader->u32TotalBytes);
		break;
	default:
		LogFmtStrA( SPLOGLV_ERROR, "Golden sample Magic Num(0x%x) is invalid, . Please check.", pHeader->nMagicNum );
		LogFmtStrA( SPLOGLV_ERROR, "This is not a valid golden sample." );
		return SP_E_SPAT_LOSS_LOAD_FROM_PHONE;
	}

	/// save detail information to log file.
	LogFmtStrA( SPLOGLV_INFO, "Golden sample information loaded:" );
	LogFmtStrA( SPLOGLV_INFO, "Make Time:%04d-%02d-%02d %02d:%02d:%02d",
		pHeader->ts.wYear, pHeader->ts.wMonth, pHeader->ts.wDay,
		pHeader->ts.wHour, pHeader->ts.wMinute, pHeader->ts.wSecond );
	return SP_OK;
}

SPRESULT CImpBase::LoadFromPhoneCusArea( GS_DATA_HEADER *pHeader )
{
	LogFmtStrA( SPLOGLV_INFO, "Reading golden sample information..." );
	if (NULL == pHeader)
	{
		LogFmtStrA( SPLOGLV_ERROR, "Input param pointer is null!" );
		return SP_E_INVALID_PARAMETER;
	}

	//m_gs.common.nMagicNum = pHeader->nMagicNum;
	//m_gs.common.ts = pHeader->ts;
	//m_gs.common.u32TotalBytes = pHeader->u32TotalBytes;
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP25, ( LPVOID )m_szGSBuf, pHeader->u32TotalBytes, TIMEOUT_20S ) );

	switch(pHeader->nMagicNum)
	{
	case GS19:
		LoadFromPhone_R6();
		break;
	case GS20:
		LoadFromPhone_R7();
		break;
	case GS21:
	case GS30:
		LoadFromPhone_R8(pHeader->u32TotalBytes);
		break;
	default:
		LogFmtStrA( SPLOGLV_ERROR, "Golden sample Magic Num(0x%x) is invalid, . Please check.", pHeader->nMagicNum );
		LogFmtStrA( SPLOGLV_ERROR, "This is not a valid golden sample." );
		return SP_E_SPAT_LOSS_LOAD_FROM_PHONE;
	}

	/// save detail information to log file.
	LogFmtStrA( SPLOGLV_INFO, "Golden sample information loaded:" );
	LogFmtStrA( SPLOGLV_INFO, "Make Time:%04d-%02d-%02d %02d:%02d:%02d",
		pHeader->ts.wYear, pHeader->ts.wMonth, pHeader->ts.wDay,
		pHeader->ts.wHour, pHeader->ts.wMinute, pHeader->ts.wSecond );
	return SP_OK;
}

SPRESULT CImpBase::SaveToPhaseCheckArea( GS_DATA_HEADER *pHeader, int nTotalLen )
{
	if( NULL == pHeader)
	{
		return SP_E_INVALID_PARAMETER;
	}

	pHeader->u32TotalBytes = nTotalLen;  // 20180322 @JXP
	CopyMemory( m_szGSBuf, pHeader, sizeof( GS_DATA_HEADER ) );
	CHKRESULT( SP_SavePhaseCheck( m_hDUT, SP05, ( LPVOID )m_szGSBuf, nTotalLen, TIMEOUT_60S ) );

	/// Verify
	GS_DATA_HEADER headerVerify;
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP05, &headerVerify, sizeof( headerVerify ), TIMEOUT_60S ) );
	if ( headerVerify.u32TotalBytes != (uint32)nTotalLen )
	{
		LogFmtStrA( SPLOGLV_ERROR, "Verify from phone failed." );
		return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
	}

	CHAR szBufRead[MAX_GOLDEN_SAMPLE_SIZE] = {0};
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP05, ( LPVOID )szBufRead, nTotalLen, TIMEOUT_60S ) );

	if ( 0 != memcmp( ( const void* )szBufRead, ( const void* )&m_szGSBuf, nTotalLen ) )
	{
		LogFmtStrA( SPLOGLV_ERROR, "Verify from phone failed." );
		return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
	}

	return SP_OK;
}

SPRESULT CImpBase::SaveToCustomerArea( GS_DATA_HEADER *pHeader, int nTotalLen )
{
	if( NULL == pHeader)
	{
		return SP_E_INVALID_PARAMETER;
	}

	pHeader->u32TotalBytes = nTotalLen;  // 20180322 @JXP
	CopyMemory( m_szGSBuf, pHeader, sizeof( GS_DATA_HEADER ) );
	CHKRESULT( SP_SavePhaseCheck( m_hDUT, SP25, ( LPVOID )m_szGSBuf, nTotalLen, TIMEOUT_60S ) );
	CHKRESULT( SP_SavePhaseCheck( m_hDUT, SP05, ( LPVOID )pHeader, sizeof( GS_DATA_HEADER ), TIMEOUT_60S ) );

	/// Verify
	GS_DATA_HEADER headerVerify;
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP05, &headerVerify, sizeof( headerVerify ), TIMEOUT_60S ) );
	if ( headerVerify.u32TotalBytes != (uint32)nTotalLen )
	{
		LogFmtStrA( SPLOGLV_ERROR, "Verify from phone failed." );
		return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
	}

	CHAR szBufRead[MAX_GOLDEN_SAMPLE_CUSTOMER_SIZE] = {0};
	CHKRESULT( SP_LoadPhaseCheck( m_hDUT, SP25, ( LPVOID )szBufRead, nTotalLen, TIMEOUT_60S ) );

	if ( 0 != memcmp( ( const void* )szBufRead, ( const void* )&m_szGSBuf, nTotalLen ) )
	{
		LogFmtStrA( SPLOGLV_ERROR, "Verify from phone failed." );
		return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
	}

	return SP_OK;
}
