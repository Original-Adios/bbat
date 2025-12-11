#include "StdAfx.h"
#include "GsmClc.h"
#include "ImpBaseUIS8910.h"
#include "gsmUtility.h"

CGsmClc::CGsmClc(CImpBaseUIS8910 *pSpat)
: ILossFunc_UIS8910(pSpat)
, m_pImpcal(pSpat)
{
    m_pFuncal = NULL;
}

CGsmClc::~CGsmClc(void)
{
}

SPRESULT CGsmClc::Run()
{
    CONST double dDelta = 0.0f;//根据实际测试补偿loss结果

    CHKRESULT(InitCal());
    RF_CABLE_LOSS_UNIT_EX*pLoss = &(m_pImp->m_lossVal.gsmLoss);//{0};
    ZeroMemory(pLoss, sizeof(*pLoss));
    int nCnt = m_pImp->m_gs.common.nGsmCnt;
    for (int i = 0; i < nCnt; i++)
    {
        int nBand   = m_pImp->m_gs.gsm[i].nBand;
        int nArfcn  = m_pImp->m_gs.gsm[i].nArfcn;
        int nPcl    = m_pImp->m_gs.gsm[i].data[0].nPCL;
        double dExp = m_pImp->m_gs.gsm[i].data[0].nExpPwr / 100.0;
        double dPwr = 0.0;
        CHKRESULT(MeaPower((SP_BAND_INFO)nBand, nArfcn, nPcl, dPwr));
        pLoss->arrPoint[i].nBand = nBand;
        pLoss->arrPoint[i].uArfcn[RF_IO_TX] = nArfcn;
        pLoss->arrPoint[i].uArfcn[RF_IO_RX] = nArfcn;
        pLoss->arrPoint[i].dFreq[RF_IO_TX] = CgsmUtility::Arfcn2MHz((SP_BAND_INFO)nBand, TRUE, nArfcn);
        pLoss->arrPoint[i].dFreq[RF_IO_RX] = pLoss->arrPoint[i].dFreq[RF_IO_TX];
        //for (int j = 0; j < DUL_ANT; j++)
        {
            //pLoss->arrPoint[i].dLoss[j][RF_IO_TX] = dExp - dPwr + dDelta;
            //pLoss->arrPoint[i].dLoss[j][RF_IO_RX] = pLoss->arrPoint[i].dLoss[j][RF_IO_TX];
            pLoss->arrPoint[i].dLoss[0][RF_IO_TX] = dExp - dPwr + dDelta;
            pLoss->arrPoint[i].dLoss[0][RF_IO_RX] = pLoss->arrPoint[i].dLoss[0][RF_IO_TX];
        }
        pLoss->nCount++;
        m_pImp->GetLossLimit(pLoss->arrPoint[i].dFreq[RF_IO_TX], m_pImp->m_dLossValLower, m_pImp->m_dLossValUpper);
        m_pImp->NOTIFY("GSM Loss", LEVEL_ITEM, m_pImp->m_dLossValLower, pLoss->arrPoint[i].dLoss[0][RF_IO_TX], m_pImp->m_dLossValUpper, CgsmUtility::GSM_BAND_NAME[nBand], nArfcn, "db", "PCL = %d", nPcl);
        CHKRESULT(m_pImp->CheckLoss(pLoss->arrPoint[i].dLoss[0][RF_IO_TX], "GSM"));
    }
 
    CHKRESULT(UninitCal());
    return SP_OK;
}



SPRESULT CGsmClc::MeaPower( SP_BAND_INFO nBand, int nArfcn, int nPcl , double &dPwr)
{
    dPwr = INVALID_NEGATIVE_DOUBLE_VALUE;

	m_pFuncal->SetBCHChannel((char)nBand,nArfcn);
	m_pFuncal->SetTCHChannel((char)nBand,(unsigned short)nArfcn);
    CHKRESULT(TxOn(nBand, nArfcn, nPcl, TRUE));

#if 0
    U_RF_PARAM param;
    param.eBand = SP_BAND_INFO(nBand);
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_BAND_INFO, param));
    param.nUlChan = nArfcn;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_CHAN, param));

    double dExp = CgsmUtility::PCL2dBm(nBand, nPcl);
    param.dExpPwr = dExp;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
#endif

	m_pFuncal->SetTCHPcl((char)nBand,(char)nPcl);

    Sleep(100);

#if 0
    SPRESULT spRes = SP_OK;
    PWR_RLT txp;
    ZeroMemory((void *)&txp, sizeof(PWR_RLT));
    for ( int i = 0; i < 5; i++)
    {
        CHKRESULT(m_pImp->m_pRFTester->InitTest(TI_PWR, NULL));

        spRes = m_pImp->m_pRFTester->FetchResult(TI_PWR, (LPVOID)&txp);

        if (SP_E_RF_OVER_RANGE == spRes)
        {
            dExp += 5.0;
            param.dExpPwr = dExp;
            CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
            continue;
        }
        else if (SP_E_RF_UNDER_RANGE == spRes )
        {
            dExp -= 5.0;
            param.dExpPwr = dExp;
            CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_EXP_PWR, param));
            continue;
        }
        else
        {
            break;
        }
    }
#endif

	if(m_pFuncal->GetTxOffsetPower((char)nBand,nPcl,&dPwr) != SP_OK)
	{
		m_pImp->LogFmtStrA(SPLOGLV_INFO,"nBand=%d\nArfcn=%d\nPcl=%d\t%6.2f dBm", nBand,nArfcn, nPcl, dPwr);
		//m_apclog.Open(m_pImp, _T("GSM\\AFC_APC_AGC.csv"), OPEN_ALWAYS);
		//m_apclog.WriteStringA("nArfcn=%d\nPcl=%d\t%6.2f dBm ", nArfcn, nPcl, dPwr);
		//m_apclog.Close();
		return SP_E_FAIL;
	}

    CHKRESULT(TxOn(nBand, nArfcn, nPcl, FALSE));

    //CHKRESULT(m_pImp->m_pRFTester->EndTest(TI_PWR));

    //dPwr = txp.dAvgPwr;

    return SP_OK;
}

SPRESULT CGsmClc::InitCal()
{
	long m_first_time_ms = 0,m_end_time_ms = 0;

    CFunUIS8910::FinalFun(&m_pFuncal); 
    m_pFuncal = CFunUIS8910::InitFun(m_pImpcal);

	CHKRESULT(m_pFuncal->EnterFun(TM_NON_SIGNAL, SUB_CAL_COMMON));
	CHKRESULT(m_pFuncal->SetAnt(GSM_MAIN));
#if 0
    SP_BAND_INFO nBand = BI_EGSM;
    int nArfcn = 975;
    int nPcl = 5;

    INIT_PARAM  rf;
    rf.eBand        = nBand;
    rf.dBcchPower   = -70.0;
    rf.dCellPower   = -70.0;
    rf.nDlChan      = (uint16)nArfcn;
    rf.nUlChan      = (uint16)nArfcn;
    rf.dTargetPower = CgsmUtility::PCL2dBm(BI_EGSM, nPcl);
    CHKRESULT(m_pImp->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, &rf));
    U_RF_PARAM param;
    param.nUlChan = nArfcn;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_UPLINK_CHAN, param));
    param.nPcl = (uint16)nPcl;
    CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_PCL, param));

	RF_PORT rfPort = {RF_ANT_1st, RS_INOUT};
	param.pPort = &rfPort;
	CHKRESULT(m_pImp->m_pRFTester->SetParameter(PT_RF_PORT, param));
#endif

	//仪表初始化
	m_first_time_ms=GetTickCount();
	CHKRESULT(m_pFuncal->CalibInit());
	m_end_time_ms=GetTickCount();
	if((m_end_time_ms - m_first_time_ms) < 3000)
	{
		Sleep(3000 - (m_end_time_ms - m_first_time_ms));
	}

    /// Setup DUT
    CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
    Sleep(100);
    CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, TRUE));

	//测试准备
	CHKRESULT(m_pFuncal->CalibPrepare()); 

    //PC_TX_PARAM_T pc;
    //pc.training_sequence = 0;
    //pc.coding_scheme     = TOOL_TX_CODE_MCS1;
    //pc.data_type         = DSP_TX_TYPE_RANDOM;
    //pc.puncture_type     = 0;
    //SP_gsmSetTxParam(m_pImp->m_hDUT, pc);

    return SP_OK;
}

SPRESULT CGsmClc::TxOn( SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn )
{
    if (bOn)
    {
		if (m_pFuncal->mode_tx_pcl((uint8)nBand, (uint16)nArfcn, (uint8)nPcl, 0) != SP_OK)
		{
			m_pImp->LogFmtStrA(SPLOGLV_INFO,"set tx pcl power is fail!");
			return SP_E_FAIL;
		}
        Sleep(200);
    }
    else
    {
		if (m_pFuncal->mode_stop() != SP_OK)
		{
			m_pImp->LogFmtStrA(SPLOGLV_INFO,"stop tx pcl power is fail!");
			return SP_E_FAIL;
		}
    }

    return SP_OK;
}

SPRESULT CGsmClc::UninitCal()
{
    //CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
	m_pFuncal->LeaveFun();
    CFunUIS8910::FinalFun(&m_pFuncal);
    return SP_OK;
}

SPRESULT CGsmClc::Init()
{
    return SP_OK;
}

SPRESULT CGsmClc::Release()
{
    return SP_OK;
}
