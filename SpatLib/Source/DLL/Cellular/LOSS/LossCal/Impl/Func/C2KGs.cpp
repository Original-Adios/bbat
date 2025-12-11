#include "StdAfx.h"
#include "C2KGs.h"
#include "ImpBase.h"
#include "assert.h"
#include <algorithm>
#include "CdmaUtility.h"

CC2KGs::CC2KGs(CImpBase *pSpat)
	: ILossFunc(pSpat)
	, m_bApt(FALSE)
{
	ZeroMemory(m_dC2KTargetPwr, sizeof(m_dC2KTargetPwr));
	ZeroMemory(m_nIndex, sizeof(m_nIndex));
	ZeroMemory(m_nPaLvl, sizeof(m_nPaLvl));
}

CC2KGs::~CC2KGs(void)
{
}

SPRESULT CC2KGs::InitData()
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

SPRESULT CC2KGs::LoadNV()
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

	PC_C2K_CAL_BANDLIST_RSP_T stBandListRsp;
	ZeroMemory(&stBandListRsp, sizeof(PC_C2K_CAL_BANDLIST_RSP_T));
	CHKRESULT(SP_C2K_CAL_BandList_Query(m_pImp->m_hDUT, &stBandListRsp));
	int nBandFlag = 0;
	CopyMemory(&nBandFlag, &stBandListRsp, sizeof(uint32));

	stNvReq.eNvType = NVM_C2K_DOWNLOAD_OPTIMIZE_TX_COMPEN_BANDX;
	CHKRESULT(SP_C2K_Nv_Read( m_pImp->m_hDUT, &stNvReq, &stNvData));
	Nvm_all_tx_compensation_band_sequence_uint stNvmOptTxCompBandSeq[CDMA_MAX_NV_BAND_NUM];
	CopyMemory(stNvmOptTxCompBandSeq, &stNvData.nData[0], stNvData.DataSize);

	for (int i = BI_C_BC0; i < BI_C_MAX_BAND; i++)
	{
		if (!m_pImp->m_bC2KBandNum[i])
		{
			continue;
		}
		if(!nBandFlag>>i &0x1)
		{
			MessageBoxA(NULL, "please check band!!", "LOSS Cal", MB_OK);
			return SP_E_FAIL;
		}
		for (int j = 0; j < CDMA_MAX_NV_BAND_NUM; j++)
		{
			if (CCdmaUtility::m_BandInfo[i].nNumeral == stNvmOptTxCompBandSeq[j].band_num_network)
			{
				//////////////////////////////////////////////////////////////////////////
				//Read CalInfo
				stNvReq.eNvType = NVM_C2K_CAL_DATA_CAL_INFO_BANDX;
				stNvReq.BandId = (uint8)j;
				ZeroMemory(&stNvData, sizeof(stNvData));
				Nvm_Cal_Info_Band stNvmCalInfo;
				CHKRESULT(SP_C2K_Nv_Read( m_pImp->m_hDUT, &stNvReq, &stNvData));
				CopyMemory(&stNvmCalInfo, &stNvData.nData[0], sizeof(Nvm_Cal_Info_Band));

				int nNvmTarPwrIndex = 0;
				for (int nPa=0; nPa<CDMA_MAX_PA_LVL; nPa++)
				{
					double dMinPwr = stNvmCalInfo.ul_pamode_gain_conf[nPa].ul_gain_min_power;
					double dMaxPwr = stNvmCalInfo.ul_pamode_gain_conf[nPa].ul_gain_max_power;
					int nPaIndexRngHigh = stNvmCalInfo.ul_pamode_gain_conf[nPa].ul_gain_start_index;

					if (dMinPwr <= m_pImp->m_dC2KTargetPwrConf && dMaxPwr >= m_pImp->m_dC2KTargetPwrConf)
					{
						nNvmTarPwrIndex = (int)(dMaxPwr - m_pImp->m_dC2KTargetPwrConf) + nPaIndexRngHigh;
						m_nPaLvl[i] = nPa;
						break;
					}	
				}
				//////////////////////////////////////////////////////////////////////////
				//Read Apc Cal result
				stNvReq.eNvType = NVM_C2K_CAL_DATA_APC_BANDX;
				stNvReq.BandId = (uint8)j;
				ZeroMemory(&stNvData, sizeof(stNvData));
				Nvm_Cal_Apc_Band stNvmCalTxRlst;
				CHKRESULT(SP_C2K_Nv_Read( m_pImp->m_hDUT, &stNvReq, &stNvData));
				CopyMemory(&stNvmCalTxRlst, &stNvData.nData[0], sizeof(Nvm_Cal_Apc_Band));
				m_nIndex[i] = stNvmCalTxRlst.ApcRlst.ApcCtrlWordFreq[0].freqx_apc_control_word[nNvmTarPwrIndex];
				//m_dC2KTargetPwr[i] = stCalConfig.apt_config.AptBand[i].PaLvl[0].AptPaLvlParam[0].start_power;
				m_dC2KTargetPwr[i] = m_pImp->m_dC2KTargetPwrConf;
			}
		}
	}
	
	return SP_OK;
}

SPRESULT CC2KGs::InitSeq(PC_C2K_CAL_TX_TUNE_REQ_T *pSeq)
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

SPRESULT CC2KGs::Run()
{
    ConfigFreq();
    CHKLOSSRESULT_WITH_NOTIFY(InitData(), "Initialize data");
    
    m_pImp->m_gs.common.nC2KCnt = 0;

    PC_C2K_CAL_TX_TUNE_REQ_T tReq;
    CHKLOSSRESULT_WITH_NOTIFY(InitSeq(&tReq), "Initialize sequence");
    

	std::list<double>::iterator it = m_listFreq.begin();
	for(;it != m_listFreq.end(); ++it)
    {
        double dFreq = *it;
        
        BOOL bFind = FALSE;
        SP_BAND_INFO bi = BI_C_BC0;
        for (int j = 0; j < BI_C_MAX_BAND; j++)
        { 
            if (!m_pImp->m_bC2KBandNum[j])
            {
                continue;
            }

            bi = (SP_BAND_INFO)j;
            double dLow = CCdmaUtility::g_C_BAND_FREQ[bi][0];;
            double dHigh = CCdmaUtility::g_C_BAND_FREQ[bi][1];;
            if (IN_RANGE(dLow, dFreq, dHigh))
            {
                bi = SP_BAND_INFO(j);
                bFind = TRUE;
                break;
            }
        }

        if (!bFind)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "The frequency table is invalid");
            m_pImp->NOTIFY("Check frequency", LEVEL_UI, 1, 0, 1);
            return SP_E_SPAT_LOSS_FREQ_TABLE_IVALID;
        }

        U_RF_PARAM param;
        param.eBand = bi;
        CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param),
            "Set band information");

        m_pImp->m_gs.c2k[m_pImp->m_gs.common.nC2KCnt].nFreq = (uint16)(CCdmaUtility::MHz2Chan(bi, dFreq));
        //m_pImp->m_gs.c2k[m_pImp->m_gs.common.nC2KCnt].data[0].nLoss = nLoss;
        m_pImp->m_gs.c2k[m_pImp->m_gs.common.nC2KCnt].data[0].nBand = (uint8)bi;

        tReq.Band = (uint16)bi;
        tReq.Arfcn = CCdmaUtility::MHz2Chan(bi, dFreq);  
		tReq.PaMode = (uint16)m_nPaLvl[bi];

        param.dUlFreq = dFreq;
        CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_FREQ, param),
            "Set uplink frequency");

		uint16 nIndex = m_nIndex[bi] & 0x3FF;
		double dPwr = 0.0;
		tReq.AptVal = m_nIndex[bi]>>10;
		int nCnt = 0;
		BOOL bFineTune = FALSE;
		do 
		{
			tReq.TxGainIndex = nIndex;
			tReq.OpType = RF_C2K_OP_ON;
			PC_C2K_CAL_TX_TUNE_RSP_T tRsp;
			ZeroMemory(&tRsp, sizeof(PC_C2K_CAL_TX_TUNE_RSP_T));
			CHKLOSSRESULT_WITH_NOTIFY(SP_C2K_CAL_TxTune(m_pImp->m_hDUT, &tReq, &tRsp), "TX ON");
			Sleep(100);

			param.dExpPwr = m_dC2KTargetPwr[bi];
			CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));

			CHKLOSSRESULT_WITH_NOTIFY(m_pImp->m_pRFTester->InitTest(TI_PWR, NULL),
				"Initialize power test");                

			PWR_RLT pwrRlt;           
			if (SP_OK != m_pImp->m_pRFTester->FetchResult(TI_PWR, &pwrRlt))
			{
				dPwr = INVALID_NEGATIVE_DOUBLE_VALUE;
				break;
			}
			m_pImp->LogFmtStrA(SPLOGLV_INFO, "[C2KLoss]Arfcn=%d, Apt=%d, index=%d, power=%.2f",tReq.Arfcn, tReq.AptVal,tReq.TxGainIndex, pwrRlt.dAvgPwr);
			tReq.OpType = RF_C2K_OP_OFF;
			CHKLOSSRESULT_WITH_NOTIFY(SP_C2K_CAL_TxTune(m_pImp->m_hDUT, &tReq, &tRsp), "TX OFF");
			Sleep(100);
			if (!bFineTune)
			{
				if (fabs(pwrRlt.dAvgPwr - m_dC2KTargetPwr[bi])<= 0.125 )
				{
					bFineTune = TRUE;
				}
				else
				{
					nIndex = (uint16) (nIndex + (pwrRlt.dAvgPwr - m_dC2KTargetPwr[bi]) * 8 + 0.5);
					continue;
				}
			}
			dPwr += pwrRlt.dAvgPwr;
			nCnt++;
			if (nCnt == 3)
			{
				break;
			}
		} while ( nIndex <= 749);

        dPwr = dPwr/3;

        m_pImp->NOTIFY("C2K GS",LEVEL_REPORT|LEVEL_UI,0,dPwr,30,CCdmaUtility::m_BandInfo[bi].NameA,
			tReq.Arfcn,"dBm","Index:%3d", nIndex); 
		if (dPwr > 30 || dPwr < 0 || nIndex > 749)
		{
			return SP_E_SPAT_OUT_OF_RANGE;
		}
        m_pImp->m_gs.c2k[m_pImp->m_gs.common.nC2KCnt].data[0].nExpPwr = (uint16)(dPwr*100);
        m_pImp->m_gs.c2k[m_pImp->m_gs.common.nC2KCnt].data[0].nIndex = (nIndex & 0x3ff) + (tReq.AptVal << 10);
        m_pImp->m_gs.common.nC2KCnt++;
    }
            
    return SP_OK;
}

void CC2KGs::ConfigFreq()
{
	m_listFreq.clear();
    for (int i = 0; i < BI_C_MAX_BAND; i++)
    { 
        if (!m_pImp->m_bC2KBandNum[i])
        {
            continue;
        }
		SP_BAND_INFO bi = (SP_BAND_INFO)i;
		double dFreq = CCdmaUtility::g_C_BAND_FREQ[bi][0];
		m_listFreq.push_back(dFreq);

		dFreq = CCdmaUtility::g_C_BAND_FREQ[bi][1];
		m_listFreq.push_back(dFreq);

		dFreq = ( CCdmaUtility::g_C_BAND_FREQ[bi][0] +  CCdmaUtility::g_C_BAND_FREQ[bi][1])/2;
		m_listFreq.push_back(dFreq);


    }
    m_listFreq.sort();
}

SPRESULT CC2KGs::Init()
{
	return SP_OK;
}

SPRESULT CC2KGs::Release()
{
	return SP_OK;
}