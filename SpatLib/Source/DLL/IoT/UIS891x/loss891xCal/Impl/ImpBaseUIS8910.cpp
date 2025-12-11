#include "StdAfx.h"
#include "ImpBaseUIS8910.h"
#include <assert.h>
#include "ModeSwitch.h"
#include "LteUtility.h"

using namespace std;

extern PC_LTE_NV_UIS8910_DATA_T g_nvTransBuf;

/////////////////////////////////////////////////////////////////////////
#if 0
CImpBaseUIS8910::CImpBaseUIS8910(void)
: m_pGsmFunc(NULL)
, m_pTdFunc(NULL)
, m_pWcdmaFunc(NULL)
, m_pLteFunc(NULL)
, m_bWcdmaSelected(FALSE)
, m_bTdSelected(FALSE)
, m_bGsmSelected(FALSE)
, m_bDMRFTSelected(FALSE)
, m_bDMRCalSelected(FALSE)
//, m_bLteSelected(FALSE)
, m_nLossValUpper(0)
, m_dWcdmaTargetPwr(0)
#endif
CImpBaseUIS8910::CImpBaseUIS8910(void)
: m_pGsmFunc(NULL)
, m_pLteFunc(NULL)
, m_pWCNFunc(NULL)
, m_bWcdmaSelected(FALSE)
, m_bTdSelected(FALSE)
, m_bGsmSelected(FALSE)
, m_bDMRFTSelected(FALSE)
, m_bDMRCalSelected(FALSE)
, m_bLteSelected(FALSE)
, m_dLossValLower(0)
, m_dLossValUpper(0)
, m_dWcdmaTargetPwr(0)
, m_Uetype(0)
, m_bLossCheck( FALSE )
, m_dLossCheckSpec( 0.3 )
{
    SetupDUTRunMode(RM_LTE_CALIBRATION_MODE);
    ZeroMemory(&m_gs, sizeof(m_gs));
    ZeroMemory(&m_lossVal, sizeof(m_lossVal));
    ZeroMemory(&m_lossOrg,sizeof(m_lossOrg));
    ZeroMemory(m_bWcdmaBandNum, sizeof(m_bWcdmaBandNum));
    ZeroMemory(m_bTdBandNum, sizeof(m_bTdBandNum));
    ZeroMemory(m_bGsmBandNum, sizeof(m_bGsmBandNum));
    ZeroMemory(m_bLteBandNum, sizeof(m_bLteBandNum));
    ZeroMemory(m_szGSBuf, sizeof(m_szGSBuf));
	ZeroMemory(m_bWlanBandNum, sizeof(m_bWlanBandNum));
	ZeroMemory(m_bBTBandNum, sizeof(m_bBTBandNum));
	m_bGPSBandNum = FALSE;
	ZeroMemory(m_eWcdmaBandAnt,sizeof(m_eWcdmaBandAnt));
	m_dTdTargetPwr = 0;
	m_nDmrFreqCnt = 0;
	m_dDmrTargetPwr = 0;
	m_nDmrPcl = 0;
	m_bWcnAnt = FALSE;
	m_dWlanRefLvl = 0;
	m_dBtRefLvl = 0;
	m_bWlanSelected = FALSE;
	m_bBtSelected = FALSE;
	m_bGpsSelected = FALSE;
    m_strNegativeLossDetected = "";
    m_vecLossLimit.clear();
    m_dLossValLower_bk = 0;
    m_dLossValUpper_bk = 0;
}

CImpBaseUIS8910::~CImpBaseUIS8910(void)
{
}

SPRESULT CImpBaseUIS8910::__FinalAction(void)
{
    //m_pGsmFunc->Release();
    //m_pTdFunc->Release();
    //m_pWcdmaFunc->Release();

	if(m_pGsmFunc == NULL)
	{
		LogRawStrA(SPLOGLV_ERROR, "Invalid pointer of test step.");
		return SP_E_INVALID_PARAMETER;
	}
	else
	{
        m_pGsmFunc->Release();
	}

	if(m_pLteFunc == NULL)
	{
		LogRawStrA(SPLOGLV_ERROR, "Invalid pointer of test step.");
		return SP_E_INVALID_PARAMETER;
	}
	else
	{
        m_pLteFunc->Release();
	}

    if (m_pWCNFunc == NULL)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid pointer of test step.");
        return SP_E_INVALID_PARAMETER;
    }
    else
    {
        m_pWCNFunc->Release();
    }

#if 0
    if (m_pTdFunc != NULL)
    {
        delete m_pTdFunc;
        m_pTdFunc = NULL;
    }
    if (m_pWcdmaFunc != NULL)
    {
        delete m_pWcdmaFunc;
        m_pWcdmaFunc = NULL;
    }
    if (m_pLteFunc != NULL)
    {
        delete m_pLteFunc;
        m_pLteFunc = NULL;
    }

	if (m_pDMRCalFunc != NULL)
	{
		delete m_pDMRCalFunc;
		m_pDMRCalFunc = NULL;
	}

	if (m_pDMRFtFunc != NULL)
	{
		delete m_pDMRFtFunc;
		m_pDMRFtFunc = NULL;
	}
#endif

	if(m_pWCNFunc != NULL)
	{
		delete m_pWCNFunc;
		m_pWCNFunc = NULL;
	}

    if (m_pGsmFunc != NULL)
    {
        delete m_pGsmFunc;
        m_pGsmFunc = NULL;
    }

	if (m_pLteFunc != NULL)
    {
        delete m_pLteFunc;
        m_pLteFunc = NULL;
    }
    return SP_OK;
}


SPRESULT CImpBaseUIS8910::__PollAction(void)
{   
    CHKRESULT_WITH_NOTIFY(m_pRFTester->GetProperty(DP_CABLE_LOSS_EX, 0, (LPVOID)&m_lossVal),
        "Get instrument property");

    return SP_OK;
}


SPRESULT CImpBaseUIS8910::LoadFromPhone()
{
    LogFmtStrA(SPLOGLV_INFO, "Reading golden sample information...");

    unsigned short usGoldSampleLen = 0;
    if (m_Uetype == 0x8850)
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8850;
    }
    else
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
    }

    Sleep(5000);//wait for UE boot complete

	GS_COMMON *header;
    
    ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
	g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_READ;
	g_nvTransBuf.header.Position = 0;
	g_nvTransBuf.header.DataSize = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
	CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));
    memcpy(&m_szGSBuf[0], &g_nvTransBuf.nData[0], MAX_GOLDEN_SAMPLE_SIZE_UIS8910);

    if (usGoldSampleLen > MAX_GOLDEN_SAMPLE_SIZE_UIS8910)
    {
        ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
        g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_READ;
        g_nvTransBuf.header.Position = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        g_nvTransBuf.header.DataSize = usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));
        memcpy(&m_szGSBuf[MAX_GOLDEN_SAMPLE_SIZE_UIS8910], &g_nvTransBuf.nData[0], usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910);
    }

	header = (GS_COMMON *) &m_szGSBuf[0];

    if (header->u32TotalBytes > usGoldSampleLen)
    {
        LogFmtStrA(SPLOGLV_ERROR, "The size of struct(%d) is larger than %dBytes.  Please check.", header->u32TotalBytes, usGoldSampleLen);
        assert(0);
        return SP_E_SPAT_LOSS_SIZE_INVALID;
    }

    int nStartAddr = 0;
    int nLen = sizeof(m_gs.common);
    CopyMemory(&m_gs.common, &m_szGSBuf[nStartAddr], nLen);
    nStartAddr += nLen;
    if (m_gs.common.nGsmCnt > 0)
    {
        nLen = sizeof(m_gs.gsm[0])*m_gs.common.nGsmCnt;
        CopyMemory(m_gs.gsm, &m_szGSBuf[nStartAddr], nLen);
        nStartAddr += nLen;
    }

    if (m_gs.common.nWcdmaCnt > 0)
    {
        nLen = sizeof(m_gs.wcdma[0])*m_gs.common.nWcdmaCnt;
        CopyMemory(m_gs.wcdma, &m_szGSBuf[nStartAddr], nLen);
        nStartAddr += nLen;
    }

    if (m_gs.common.usLteCnt > 0)
    {
        nLen = m_gs.common.usLteCnt;
        std::vector<uint8> arrInfo;
        arrInfo.resize(nLen);
        CopyMemory(&arrInfo[0], &m_szGSBuf[nStartAddr], nLen);
        nStartAddr += nLen;  
        m_pLteFunc->Deserialization(&arrInfo);
    }

    if (m_gs.common.nTdCnt > 0)
    {
        nLen = sizeof(m_gs.td[0])*m_gs.common.nTdCnt;
        CopyMemory(m_gs.td, &m_szGSBuf[nStartAddr], nLen);
        nStartAddr += nLen;
    }

	if (m_gs.common.nDmrCnt > 0)
	{
		nLen = sizeof(m_gs.dmr[0])*m_gs.common.nDmrCnt;
		CopyMemory(m_gs.dmr, &m_szGSBuf[nStartAddr], nLen);
		nStartAddr += nLen;
	}
	if (m_gs.common.nWlanCnt > 0)
	{
		nLen = sizeof(m_gs.wlan[0])*m_gs.common.nWlanCnt;
		CopyMemory(m_gs.wlan, &m_szGSBuf[nStartAddr], nLen);
		nStartAddr += nLen;
	}
	if (m_gs.common.nBtCnt > 0)
	{
		nLen = sizeof(m_gs.bt[0])*m_gs.common.nBtCnt;
		CopyMemory(m_gs.bt, &m_szGSBuf[nStartAddr], nLen);
		nStartAddr += nLen;
	}
	if (m_gs.common.nGpsCnt > 0)
	{
		nLen = sizeof(m_gs.gps[0])*m_gs.common.nGpsCnt;
		CopyMemory(m_gs.gps, &m_szGSBuf[nStartAddr], nLen);
		nStartAddr += nLen;
	}

    if (GS19 != m_gs.common.nMagicNum)
    {
        LogFmtStrA(SPLOGLV_ERROR, "This is not a valid golden sample.");
        return SP_E_SPAT_LOSS_LOAD_FROM_PHONE;
    }

    /// save detail information to log file.
    LogFmtStrA(SPLOGLV_INFO, "Golden sample information loaded:");
    LogFmtStrA(SPLOGLV_INFO, "Make Time:%04d-%02d-%02d %02d:%02d:%02d",
        m_gs.common.ts.wYear, m_gs.common.ts.wMonth, m_gs.common.ts.wDay, 
        m_gs.common.ts.wHour, m_gs.common.ts.wMinute, m_gs.common.ts.wSecond);    

    return SP_OK;
}

SPRESULT CImpBaseUIS8910::SaveToPhone()
{
    LogFmtStrA(SPLOGLV_INFO, "Writing information into golden sample...");

    unsigned short usGoldSampleLen = 0;
    if (m_Uetype == 0x8850)
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8850;
    }
    else
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
    }

    GetLocalTime(&m_gs.common.ts);
    m_gs.common.nMagicNum = GS19;
    uint32 nTotalLen = 0; // 20180322 @JXP
    int nLen = sizeof(m_gs.common);
    std::vector<uint8> arrInfo;

    if (m_bLteSelected)
    {
        m_pLteFunc->Serialization(&arrInfo);
        m_gs.common.usLteCnt = (uint16)arrInfo.size();
    }
    else
    {
        m_gs.common.usLteCnt = 0;
    }

    CopyMemory(&m_szGSBuf[nTotalLen], &m_gs.common, nLen);
    nTotalLen += nLen;
    if (m_gs.common.nGsmCnt > 0)
    {
        nLen = sizeof(m_gs.gsm[0])*m_gs.common.nGsmCnt;
        CopyMemory(&m_szGSBuf[nTotalLen], m_gs.gsm, nLen);
        nTotalLen += nLen;
    }

    if (m_gs.common.nWcdmaCnt > 0)
    {
        nLen = sizeof(m_gs.wcdma[0])*m_gs.common.nWcdmaCnt;
        CopyMemory(&m_szGSBuf[nTotalLen], m_gs.wcdma, nLen);
        nTotalLen += nLen;
    }

    if (m_gs.common.usLteCnt > 0)
    {
        CopyMemory(&m_szGSBuf[nTotalLen], &arrInfo[0], m_gs.common.usLteCnt);
        nTotalLen += m_gs.common.usLteCnt;
    }

    if (m_gs.common.nTdCnt > 0)
    {
        nLen = sizeof(m_gs.td[0])*m_gs.common.nTdCnt;
        CopyMemory(&m_szGSBuf[nTotalLen], m_gs.td, nLen);
        nTotalLen += nLen;
    }

	if (m_gs.common.nDmrCnt > 0)
	{
		nLen = sizeof(m_gs.dmr[0])*m_gs.common.nDmrCnt;
		CopyMemory(&m_szGSBuf[nTotalLen], m_gs.dmr, nLen);
		nTotalLen += nLen;
	}

	if (m_gs.common.nWlanCnt > 0)
	{
		nLen = sizeof(m_gs.wlan[0])*m_gs.common.nWlanCnt;
		CopyMemory(&m_szGSBuf[nTotalLen], m_gs.wlan, nLen);
		nTotalLen += nLen;
	}

	if (m_gs.common.nBtCnt > 0)
	{
		nLen = sizeof(m_gs.wlan[0])*m_gs.common.nBtCnt;
		CopyMemory(&m_szGSBuf[nTotalLen], m_gs.bt, nLen);
		nTotalLen += nLen;
	}

	if (m_gs.common.nGpsCnt > 0)
	{
		nLen = sizeof(m_gs.wlan[0])*m_gs.common.nGpsCnt;
		CopyMemory(&m_szGSBuf[nTotalLen], m_gs.gps, nLen);
		nTotalLen += nLen;
	}

    if (nTotalLen > usGoldSampleLen)
    {
        LogFmtStrA(SPLOGLV_ERROR, "The size of struct(%d) is larger than %dBytes. Please check.", nTotalLen, usGoldSampleLen);
        assert(0);
        return SP_E_SPAT_LOSS_SIZE_INVALID;
    }
    
    m_gs.common.u32TotalBytes = nTotalLen;  // 20180322 @JXP
    CopyMemory(m_szGSBuf, &m_gs.common, sizeof(m_gs.common));
    //CHKRESULT(SP_SavePhaseCheck(m_hDUT, SP05, (LPVOID)m_szGSBuf, nTotalLen, TIMEOUT_60S));

    ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
	g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_WRITE;
	g_nvTransBuf.header.Position = 0;
	g_nvTransBuf.header.DataSize = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
	CopyMemory(&g_nvTransBuf.nData[0], &m_szGSBuf[0], MAX_GOLDEN_SAMPLE_SIZE_UIS8910);
	CHKRESULT(SP_lteSaveNV_UIS8910(m_hDUT, &g_nvTransBuf));

    if (usGoldSampleLen > MAX_GOLDEN_SAMPLE_SIZE_UIS8910)
    {
        ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
        g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_WRITE;
        g_nvTransBuf.header.Position = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        g_nvTransBuf.header.DataSize = usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        CopyMemory(&g_nvTransBuf.nData[0], &m_szGSBuf[MAX_GOLDEN_SAMPLE_SIZE_UIS8910], usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910);
        CHKRESULT(SP_lteSaveNV_UIS8910(m_hDUT, &g_nvTransBuf));
    }

#if 0
    /// Verify 
    GS_COMMON header;
    CHKRESULT(SP_LoadPhaseCheck(m_hDUT, SP05, &header, sizeof(header), TIMEOUT_60S));
    if (header.u32TotalBytes != nTotalLen)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Verify from phone failed.");
        return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
    }

    CHAR szBufRead[MAX_GOLDEN_SAMPLE_SIZE] = {0};
    CHKRESULT(SP_LoadPhaseCheck(m_hDUT, SP05, (LPVOID)szBufRead, nTotalLen, TIMEOUT_60S));

    if (0 != memcmp((const void*)szBufRead, (const void*)&m_szGSBuf, nTotalLen))
    {
        LogFmtStrA(SPLOGLV_ERROR, "Verify from phone failed.");
        return SP_E_SPAT_LOSS_SAVE_TO_PHONE;
    }
#endif

    return SP_OK;
}

BOOL CImpBaseUIS8910::CheckLossDelta(const char* czModeName, RF_CABLE_LOSS_UNIT_EX* pOldLoss, RF_CABLE_LOSS_UNIT_EX* pCurLoss )
{

    static const LPCSTR ANT_NAME[MAX_RF_ANT] = {"Primary","Diversity","RF_ANT_3rd","RF_ANT_4th"};
    static const LPCSTR IO_NAME[MAX_RF_IO] = {"TX", "RX"};

    if ( pOldLoss->nCount != pCurLoss->nCount )
    {
        LogFmtStrA( SPLOGLV_ERROR, "The count of loss unit is mismatch." );
        return FALSE;
    }
    for ( int i = 0; i < pOldLoss->nCount; i++ )
    {
        //for ( int j = RF_ANT_1st; j < MAX_RF_ANT; j++ )
        for ( int j = RF_ANT_1st; j < RF_ANT_2nd; j++ )
        {
            for ( int k = RF_IO_TX; k < MAX_RF_IO; k++ )
            {
                double dFabsDelta = fabs( pOldLoss->arrPoint[i].dLoss[j][k] - pCurLoss->arrPoint[i].dLoss[j][k] );

                if ( ((int)(dFabsDelta*100)) > ((int)(m_dLossCheckSpec*100)) )
                {
                    NOTIFY( "Loss Check", LEVEL_ITEM, -m_dLossCheckSpec, dFabsDelta, m_dLossCheckSpec, NULL, -1, "db", "Mode = %s;ANT = %s;IO = %s;Freq = %.2f", czModeName, ANT_NAME[j], IO_NAME[k], pOldLoss->arrPoint[i].dFreq);
                    LogFmtStrA( SPLOGLV_ERROR, "Mode = %s, ANT = %s, IO = %s, Freq = %.2f, 1:%.2f, 2:%.2f, Delta:%.2f > %.2f.", czModeName, ANT_NAME[j], IO_NAME[k], pOldLoss->arrPoint[i].dFreq, pOldLoss->arrPoint[i].dLoss[j][k], pCurLoss->arrPoint[i].dLoss[j][k], dFabsDelta, m_dLossCheckSpec);
                    LogFmtStrA( SPLOGLV_ERROR, "The delta of tow loss is out of range." );
                    return FALSE;
                }
            }
        }
    }
    
    return TRUE;
}

SPRESULT CImpBaseUIS8910::UpgradeLoss()
{
    RF_CABLE_LOSS_COMB_EX lossCom;
    ICallback* pCallBack = GetCallback();

	enum E_LOSS_MODE
	{
		GSM = 0,
		WCDMA,
		TD,
		C2K,
		LTE,
		NR,
		WLAN,
		BT,
		GPS,
		DMR,
	};

    if (m_gs.common.nGsmCnt > 0 && m_bGsmSelected)
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
        pCallBack->UiMsgUpdateLoss(&lossCom);
    } 

    if (m_gs.common.nWcdmaCnt > 0 && m_bWcdmaSelected)
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

    if (m_lossVal.lteLoss.nCount > 0 && m_bLteSelected)
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

    if (m_lossVal.tdLoss.nCount > 0 && m_bTdSelected)
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

	if (m_lossVal.otherLoss.nCount > 0 && m_bDMRCalSelected)
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
	if (m_lossVal.otherLoss.nCount > 0 && m_bDMRFTSelected)
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
	if (m_lossVal.wlanLoss.nCount > 0 && m_bWlanSelected)
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
	if (m_lossVal.btLoss.nCount > 0 && m_bBtSelected)
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
	if (m_lossVal.gpsLoss.nCount > 0 && m_bGpsSelected)
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
    return m_pRFTester->SetProperty(DP_CABLE_LOSS_EX, 0, (LPCVOID)&m_lossVal);
}

SPRESULT CImpBaseUIS8910::ChangeMode(RM_MODE_ENUM eMode)
{
    return m_pModeSwitch->Change(eMode);
}

void CImpBaseUIS8910::ZeroLossValue(RF_CABLE_LOSS_EX& loss)
{
    /// GSM
    if (m_gs.common.nGsmCnt > 0)
    {
        for (int i = 0; i < loss.gsmLoss.nCount; i++)
        {
            for (int j = 0; j < MAX_RF_ANT; j++)
            {
                loss.gsmLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.gsmLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
            }        
        }
    }
    
    
    /// TD
    if (m_gs.common.nTdCnt > 0)
    {
        for (int i = 0; i < loss.tdLoss.nCount; i++)
        {
            for (int j = 0; j < MAX_RF_ANT; j++)
            {
                loss.tdLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.tdLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
            }        
        }
    }
    

    /// WCDMA
    if (m_gs.common.nWcdmaCnt > 0)
    {
        for (int i = 0; i < loss.wcdmaLoss.nCount; i++)
        {
            for (int j = 0; j < MAX_RF_ANT; j++)
            {
                loss.wcdmaLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.wcdmaLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
            }        
        }
    }
    

    /// LTE
    if (m_gs.common.usLteCnt > 0)
    {
        for (int i = 0; i < loss.lteLoss.nCount; i++)
        {
            for (int j = 0; j < MAX_RF_ANT; j++)
            {
                loss.lteLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.lteLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
            }        
        }
    }
	///DMR
	if (m_gs.common.nDmrCnt > 0)
	{
		for (int i = 0; i < loss.otherLoss.nCount; i++)
		{
			for (int j = 0; j < MAX_RF_ANT; j++)
			{
				loss.otherLoss.arrPoint[i].dLoss[j][RF_IO_TX]  = 0;
				loss.otherLoss.arrPoint[i].dLoss[j][RF_IO_RX]  = 0;
			}        
		}
	}

	/// WCN
	if (m_gs.common.nWlanCnt > 0)
	{
		for (int i = 0; i < loss.wlanLoss.nCount; i++)
		{
			for (int j = 0; j < MAX_RF_ANT; j++)
			{
				loss.wlanLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.wlanLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}        
		}
	}
	if (m_gs.common.nBtCnt > 0)
	{
		for (int i = 0; i < loss.btLoss.nCount; i++)
		{
			for (int j = 0; j < MAX_RF_ANT; j++)
			{
				loss.btLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.btLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}        
		}
	}
	if (m_gs.common.nGpsCnt > 0)
	{
		for (int i = 0; i < loss.gpsLoss.nCount; i++)
		{
			for (int j = 0; j < MAX_RF_ANT; j++)
			{
				loss.gpsLoss.arrPoint[i].dLoss[j][RF_IO_TX] = 0;
				loss.gpsLoss.arrPoint[i].dLoss[j][RF_IO_RX] = 0;
			}        
		}
	}
	   
}

SPRESULT CImpBaseUIS8910::CheckLoss(double dLoss, string strMode)
{
    if (dLoss < 0.0)
    {
        if ( string::npos == m_strNegativeLossDetected.find(strMode))
        {
            m_strNegativeLossDetected += strMode;
            m_strNegativeLossDetected += ",";
        }
    }

    if (dLoss <= m_dLossValLower || dLoss > m_dLossValUpper)
    {
        LogFmtStrA(SPLOGLV_ERROR, "Loss value(%.2f) is out of range(%.2f, %.2f)", dLoss, m_dLossValLower, m_dLossValUpper);
        return SP_E_SPAT_LOSS_OUT_OF_RANGE;
    }
    return SP_OK;
}

void CImpBaseUIS8910::LossLowerLimitSearch(int iStartIndex, const double& upperLimit, double& lowerLimit)
{
    bool bFind = false;
    for (; iStartIndex >= 0; iStartIndex--)
    {
        if (upperLimit > m_vecLossLimit[iStartIndex].dLossUpperLimit)
        {
            lowerLimit = m_vecLossLimit[iStartIndex].dLossUpperLimit;
            bFind = true;
            break;
        }
    }
    if (!bFind)
    {
        lowerLimit = m_dLossValLower_bk;
    }
}


void CImpBaseUIS8910::GetLossLimit(const double dFreq, double& lowerLimit, double& upperLimit)
{
    if (m_vecLossLimit.size() == 0)
    {
        lowerLimit = m_dLossValLower_bk;
        upperLimit = m_dLossValUpper_bk;
        return;
    }
    bool bLastSec = true;
    int iVecLossLimitSize = m_vecLossLimit.size();
    for (int i=0;i< iVecLossLimitSize;i++)
    {
        if (dFreq<= m_vecLossLimit[i].dFreq)
        {
            if (i == 0)
            {
                lowerLimit = m_dLossValLower_bk;
                upperLimit = m_vecLossLimit[i].dLossUpperLimit;
            }
            else
            {
                upperLimit = m_vecLossLimit[i].dLossUpperLimit;
                LossLowerLimitSearch(i-1,upperLimit,lowerLimit);
            }
            bLastSec = false;
            break;
        }
    }
    if(bLastSec)
    {
        upperLimit = m_dLossValUpper_bk;
        LossLowerLimitSearch(m_vecLossLimit.size() - 1,upperLimit,lowerLimit);
    }
}
