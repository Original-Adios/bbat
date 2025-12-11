#include "StdAfx.h"
#include "GsmGs.h"
#include "ImpBaseUIS8910.h"
#include "gsmUtility.h"
#include <list>
#include <algorithm>
using namespace std;

CGsmGs::CGsmGs(CImpBaseUIS8910 *pSpat)
: ILossFunc_UIS8910(pSpat)
, m_pImpcal(pSpat)
{
    m_pFuncal = NULL;
}

CGsmGs::~CGsmGs(void)
{
}

SPRESULT CGsmGs::Run()
{
    ConfigFreq();
	m_pImp->m_gs.common.nGsmCnt = 0;
	CHKRESULT(InitCal());
	BOOL bFindAnyBand = FALSE;
	list<double>::iterator it = m_listFreq.begin();
	for(;it != m_listFreq.end(); ++it)
    {
        double dFreq = *it;
        bFindAnyBand = FALSE;
        int nTargetBand = BI_GSM_850;
        for (int nBand = BI_GSM_850; nBand < BI_GSM_MAX_BAND; nBand++)
        {
            if (IN_RANGE(CgsmUtility::nFreq[nBand][0][0],
                dFreq,
                CgsmUtility::nFreq[nBand][0][1]))
            {
                bFindAnyBand = TRUE;
                nTargetBand = nBand;
                break;
            }
        }

        if (!bFindAnyBand)
        {
            m_pImp->LogFmtStrA(SPLOGLV_ERROR, "The frequency table is invalid");
            return SP_E_SPAT_LOSS_FREQ_TABLE_IVALID;
        }

        double dPwr = 0.0;
        int nArfcn = CgsmUtility::MHz2Arfch((SP_BAND_INFO)nTargetBand,  TRUE , dFreq);
		MeaPower((SP_BAND_INFO)nTargetBand, nArfcn, m_pImp->m_nGsmPcl[nTargetBand], dPwr);

        m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].nBand = (int16)nTargetBand;
        m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].nArfcn = (int16)nArfcn;
        for (int j = 0; j < DUL_ANT; j++ )
        {
            m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].data[j].nExpPwr = (int16)(dPwr * 100);
            m_pImp->m_gs.gsm[m_pImp->m_gs.common.nGsmCnt].data[j].nPCL    = (uint8)m_pImp->m_nGsmPcl[nTargetBand];
        }
        m_pImp->m_gs.common.nGsmCnt++;
        m_pImp->NOTIFY("GSM GS", LEVEL_ITEM, 0, dPwr, 35, CgsmUtility::GSM_BAND_NAME[nTargetBand], nArfcn, "dBm", "PCL = %d", m_pImp->m_nGsmPcl[nTargetBand]);
		if (dPwr > 35.0 || dPwr < 0)
		{
			return SP_E_SPAT_OUT_OF_RANGE;
		}
    }
    CHKRESULT(UninitCal());

    if (!bFindAnyBand)
    {
        return SP_E_SPAT_LOSS_FREQ_IVALID;
    }
    return SP_OK;
}

void CGsmGs::ConfigFreq()
{
	m_listFreq.clear();
    for (int i = 0; i < BI_GSM_MAX_BAND; i++)
    { 
        if (!m_pImp->m_bGsmBandNum[i])
        {
            continue;
        }
        double dLow = CgsmUtility::nFreq[i][0][0];
        double dHigh = CgsmUtility::nFreq[i][0][1];
        double dMid = (dLow + dHigh) / 2;

        m_listFreq.push_back(dLow);
        m_listFreq.push_back(dMid);
        m_listFreq.push_back(dHigh);
    }
    m_listFreq.sort();
}

SPRESULT CGsmGs::MeaPower( SP_BAND_INFO nBand, int nArfcn, int nPcl , double &dPwr)
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

	if (spRes == SP_OK)
	{
		dPwr = txp.dAvgPwr;
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

    return SP_OK;
}

SPRESULT CGsmGs::InitCal()
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

SPRESULT CGsmGs::TxOn( SP_BAND_INFO nBand, int nArfcn, int nPcl, BOOL bOn )
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

SPRESULT CGsmGs::UninitCal()
{
    //CHKRESULT(SP_gsmActive(m_pImp->m_hDUT, FALSE));
	m_pFuncal->LeaveFun();
    CFunUIS8910::FinalFun(&m_pFuncal);
    return SP_OK;
}

SPRESULT CGsmGs::Init()
{
    return SP_OK;
}

SPRESULT CGsmGs::Release()
{
    return SP_OK;
}
