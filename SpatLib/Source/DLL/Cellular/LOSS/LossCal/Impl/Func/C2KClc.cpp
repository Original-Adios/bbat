#include "StdAfx.h"
#include "C2KClc.h"
#include "ImpBase.h"
#include "assert.h"
#include "CdmaUtility.h"

CC2KClc::CC2KClc(CImpBase *pSpat)
: ILossFunc(pSpat)
, m_bApt(FALSE)
{
}

CC2KClc::~CC2KClc(void)
{
}

SPRESULT CC2KClc::InitData()
{
	CHKRESULT(m_pImp->m_pRFTester->SetNetMode(NM_CDMA));
	CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, NULL));
	RF_PORT rfPort = {RF_ANT_1st, RS_IN};
	U_RF_PARAM param;
	param.pPort = &rfPort;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));	
	CHKRESULT(SP_C2K_CAL_Active(m_pImp->m_hDUT, TRUE));
	CHKRESULT(LoadNV());
    return SP_OK;
}

SPRESULT CC2KClc::LoadNV()
{
	PC_C2K_RF_NV_DATA_REQ_CMD_T stNvReq;
	ZeroMemory(&stNvReq, sizeof(stNvReq));
	PC_C2K_RF_NV_DATA_PARAM_T	stNvData;
	ZeroMemory(&stNvData, sizeof(stNvData));

	Nvm_Download_Optimize_Cal_Config stCalConfig;
	stNvReq.eNvType = NVM_C2K_DOWNLOAD_OPTIMIZE_CAL_CONFIG;
	CHKRESULT(SP_C2K_Nv_Read( m_pImp->m_hDUT, &stNvReq, &stNvData));
	CopyMemory(&stCalConfig, &stNvData.nData[0], stNvData.DataSize);
	m_bApt = stCalConfig.hdt_config.hdt_enable;

    return SP_OK;
}

SPRESULT CC2KClc::InitSeq(PC_C2K_CAL_TX_TUNE_REQ_T *pSeq)
{
	if (NULL == pSeq)
	{
		assert(0);
		return SP_E_SPAT_INVALID_POINTER;
	}

	ZeroMemory(pSeq, sizeof(PC_C2K_CAL_TX_TUNE_REQ_T));
	if (m_bApt)
	{
		pSeq->CmdMask = TX_CMD_MASK_ENABLE_PA_DCDC | TX_CMD_MASK_DCDC_CTRL_TYPE | TX_CMD_MASK_ENABLE_PDET;
	}
	else
	{
		pSeq->CmdMask = TX_CMD_MASK_ENABLE_PA_DCDC | TX_CMD_MASK_DCDC_CTRL_TYPE;
	}

	pSeq->PaMode = 0;
	pSeq->OpType = RF_C2K_OP_ON;

    return SP_OK;
}

SPRESULT CC2KClc::Run()
{
    CHKLOSSRESULT_WITH_NOTIFY(InitData(), "Initialize data");    
    PC_C2K_CAL_TX_TUNE_REQ_T tReq;
    CHKLOSSRESULT_WITH_NOTIFY(InitSeq(&tReq), "Initialize sequence");

    RF_CABLE_LOSS_UNIT_EX* pLoss = &(m_pImp->m_lossVal.cdmaLoss);//{0}; 
    ZeroMemory(pLoss, sizeof(*pLoss));
    for (int i = 0; i < m_pImp->m_gs.common.nC2KCnt; i++)
    {
		SP_BAND_INFO nBand = (SP_BAND_INFO)m_pImp->m_gs.c2k[i].data[0].nBand;

		U_RF_PARAM param;
		param.eBand = nBand;
		CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
            "Set band information");

        uint16 Arfcn = m_pImp->m_gs.c2k[i].nFreq;
        pLoss->arrPoint[i].dFreq[RF_IO_TX] = CCdmaUtility::Chan2MHz(nBand, Arfcn);
        
        param.dUlFreq = CCdmaUtility::Chan2MHz(nBand, Arfcn);
        CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
            "Set uplink frequency");

		tReq.TxGainIndex = m_pImp->m_gs.c2k[i].data[0].nIndex & 0x3ff;
		tReq.AptVal = m_pImp->m_gs.c2k[i].data[0].nIndex >> 10;

        tReq.Band = (uint16)nBand;
        tReq.Arfcn = Arfcn;

        double dTotalPwr = 0;
        int16 nTargetPwr = (int16)m_pImp->m_gs.c2k[i].data[0].nExpPwr;
        double dTargetPwr = nTargetPwr/100.0;
        for (int j = 0; j < 3; j++)
        {
            tReq.OpType = RF_C2K_OP_ON;
			PC_C2K_CAL_TX_TUNE_RSP_T tRsp;
			ZeroMemory(&tRsp, sizeof(PC_C2K_CAL_TX_TUNE_RSP_T));
			CHKLOSSRESULT_WITH_NOTIFY(SP_C2K_CAL_TxTune(m_pImp->m_hDUT, &tReq, &tRsp), "TX ON");
			Sleep(100);

            param.dExpPwr = dTargetPwr;
			CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));

            CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, NULL),
                "Initialize power test");                

            PWR_RLT pwrRlt;           
			if (SP_OK != m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt))
			{
				pwrRlt.dAvgPwr = INVALID_NEGATIVE_DOUBLE_VALUE;
				break;
			}  
			m_pImp->LogFmtStrA(SPLOGLV_INFO, "[C2KLoss]Arfcn=%d, Apt=%d, index=%d, power=%.2f",tReq.Arfcn, tReq.AptVal,tReq.TxGainIndex, pwrRlt.dAvgPwr);
            dTotalPwr += pwrRlt.dAvgPwr;

			tReq.OpType = RF_C2K_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_C2K_CAL_TxTune(m_pImp->m_hDUT, &tReq, &tRsp), "TX OFF");
            Sleep( 100 );
        }
        
        double dPwr = dTotalPwr / 3;

        for (int j = 0; j < MUL_ANT; j++)
        {
            pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dTargetPwr - dPwr + m_pImp->m_dPreLoss;
			pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
        }
        pLoss->nCount++;
        m_pImp->NOTIFY("C2K Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper, 
            CCdmaUtility::m_BandInfo[nBand].NameA, tReq.Arfcn, "dB", "Index = %3d", tReq.TxGainIndex);
        CHKLOSSRESULT_WITH_NOTIFY(m_pImp->CheckLoss( pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "C2K"), "Check loss");
    }
    
    return SP_OK;
}

SPRESULT CC2KClc::TunePwr(double dPwr, SPRESULT spRlt, PWR_RLT &pwrRlt)
{
    int j;
    for(j = 0; j < 10; j++)
    {
        if (spRlt == SP_E_RF_OVER_RANGE)
        {
            dPwr += 10;
        }
        else if (spRlt == SP_E_RF_UNDER_RANGE)
        {
            dPwr -= 10;
        }
        else if (spRlt == SP_OK)
        {
            return SP_OK;
        }
        else
        {
            return SP_E_SPAT_LOSS_TUNE_PWR;
        }
        CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR, (LPCVOID)&dPwr)); 
        spRlt = m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt);                
    }

    if (j == 10)
    {
        return SP_E_SPAT_LOSS_TUNE_PWR;
    }

    return SP_OK;
}

SPRESULT CC2KClc::Init()
{
    return SP_OK;
}

SPRESULT CC2KClc::Release()
{
    return SP_OK;
}
