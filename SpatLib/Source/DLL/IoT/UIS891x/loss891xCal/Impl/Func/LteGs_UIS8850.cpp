#include "StdAfx.h"
#include "LteGs_UIS8850.h"
#include "ImpBaseUIS8910.h"
#include "ApiTxUIS8850.h"
#include "ApiRxUIS8850.h"

CLteGs_UIS8850::CLteGs_UIS8850(CImpBaseUIS8910 *pSpat)
: ILossFunc_UIS8910(pSpat)
, m_pFuncCenter(NULL)
, m_CurrentCa(LTE_CA_PCC)
, m_pNv(NULL)
, m_pNal(NULL)
, m_pFileConfig(NULL)
, m_pTxApi(NULL)
, m_pRxApi(NULL)
, m_pTxAlgoMainPcc(NULL)
, m_pTxAlgoDivScc(NULL)
, m_pRxAlgoMainPcc(NULL)
, m_pRxAlgoDivPcc (NULL)
{
    m_pSpatBase = (CSpatBase*)m_pImp;
}

CLteGs_UIS8850::~CLteGs_UIS8850(void)
{

}

SPRESULT CLteGs_UIS8850::Run()
{
    CHKRESULT(SP_lteActive(m_pImp->m_hDUT, TRUE));

    CHKRESULT(m_pNv->Load());

    CHKRESULT(m_pSpatBase->m_pRFTester->SetNetMode(NM_LTE));
    CHKRESULT(m_pSpatBase->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TRX_FDT, NULL));

    //if (!m_pNv->m_bSameDownloadChecksum)
    {
        CHKRESULT(m_pNal->GetConfig());
        CHKRESULT(m_pNal->InitResult());
        CHKRESULT(m_pNal->InitResultAcc());
        m_pNv->ApplyChecksum();
    }
    CHKRESULT(m_pNal->ClearResult());
    CHKRESULT(m_pNal->ClearResultAcc());

    CHKRESULT(m_pRxAlgoMainPcc->Init());
    CHKRESULT(m_pRxAlgoDivPcc->Init());
    CHKRESULT(m_pTxAlgoMainPcc->Init());
    CHKRESULT(m_pTxAlgoDivScc->Init());

    for(int i=0;i<m_pFileConfig->m_nTestNum;i++)
    {
        CHKRESULT(m_pNal->InitResult());
        CHKRESULT(m_pNal->ClearResult());
        CHKRESULT(m_pRxAlgoMainPcc->Run());
        CHKRESULT(m_pRxAlgoDivPcc->Run());
        if(0x1930 == m_pImp->m_Uetype)//UIS1930
        {
        }
        else
        {
            CHKRESULT(m_pTxAlgoMainPcc->Run());
            CHKRESULT(m_pTxAlgoDivScc->Run());
        }

        m_pNal->AccResultPro(i);
    }

    CHKRESULT(SP_lteActive(m_pImp->m_hDUT, FALSE));

    U_RF_PARAM param;
    ZeroMemory(&param, sizeof(U_RF_PARAM));
    CHKRESULT(m_pSpatBase->m_pRFTester->SetParameter(PT_RESET, param));

    return SP_OK;
}

SPRESULT CLteGs_UIS8850::Init()
{
    if (m_pFuncCenter == NULL)
    {
        m_pFuncCenter = new CFuncCenter();
    }
    m_pFuncCenter->AddFunc(SPATBASE, m_pSpatBase);
    m_pFuncCenter->AddFunc(CURRENT_CA, &m_CurrentCa);

    if (m_pNv == NULL)
    {
        m_pNv = new CNv_UIS8910(FUNC_INV, m_pFuncCenter);
    }
    if (m_pNal == NULL)
    {
        m_pNal = new CNalGs_UIS8910(FUNC_INAL, m_pFuncCenter);
    }
    if (m_pFileConfig == NULL)
    {
        m_pFileConfig = new CFileConfig_UIS8910(FUNC_FILE_CONFIG, m_pFuncCenter);
    }

    m_pFileConfig->Load();

#if 0
    if(m_pFileConfig->m_arrBandEnable[LTE_BAND66])
    {
        if (m_pTxApi == NULL)
        {
            m_pTxApi = new CApiTxV22(API_APC, m_pFuncCenter);
        }
        if (m_pRxApi == NULL)
        {
            m_pRxApi = new CApiRxV3(API_AGC, m_pFuncCenter);
        }
    }
    else
    {
        if (m_pTxApi == NULL)
        {
            m_pTxApi = new CApiTxV21(API_APC, m_pFuncCenter);
        }
        if (m_pRxApi == NULL)
        {
            m_pRxApi = new CApiRxV1(API_AGC, m_pFuncCenter);
        }
    }
#endif

	if (m_pTxApi == NULL)
	{
		m_pTxApi = new CApiTxUIS8850(API_APC, m_pFuncCenter);
	}
	if (m_pRxApi == NULL)
	{
		m_pRxApi = new CApiRxUIS8850(API_AGC, m_pFuncCenter);
	}

    if (m_pTxAlgoMainPcc == NULL)
    {
        m_pTxAlgoMainPcc = new CAlgoGsTx(ALGO_APC_MAIN_PCC, m_pFuncCenter);
    }
    if (m_pTxAlgoDivScc == NULL)
    {
        m_pTxAlgoDivScc = new CAlgoGsTx(ALGO_APC_DIV_SCC, m_pFuncCenter);
    }
    if (m_pRxAlgoMainPcc == NULL)
    {
        m_pRxAlgoMainPcc = new CAlgoGsRx(ALGO_AGC_MAIN_PCC, m_pFuncCenter);
    }
    if (m_pRxAlgoDivPcc == NULL)
    {
        m_pRxAlgoDivPcc = new CAlgoGsRx(ALGO_AGC_DIV_PCC, m_pFuncCenter);
    }

    CHKRESULT( m_pFileConfig       ->PreInit());
    CHKRESULT(m_pNv               ->PreInit());
    CHKRESULT(m_pNal              ->PreInit());
    CHKRESULT(m_pTxApi            ->PreInit());
    CHKRESULT(m_pRxApi            ->PreInit());
    CHKRESULT(m_pTxAlgoMainPcc    ->PreInit());
    CHKRESULT(m_pTxAlgoDivScc     ->PreInit());
    CHKRESULT(m_pRxAlgoMainPcc    ->PreInit());
    CHKRESULT(m_pRxAlgoDivPcc     ->PreInit());

    m_pTxAlgoMainPcc  ->SetAntCa(RF_ANT_1st, LTE_CA_PCC);
    m_pTxAlgoDivScc   ->SetAntCa(RF_ANT_2nd, LTE_CA_SCC);
    m_pRxAlgoMainPcc  ->SetAntCa(RF_ANT_1st, LTE_CA_PCC);
    m_pRxAlgoDivPcc   ->SetAntCa(RF_ANT_2nd, LTE_CA_PCC);

    //m_pSpatBase->SetupDUTRunMode(RM_LTE_CALIBRATION_MODE);

    return SP_OK;
}

SPRESULT CLteGs_UIS8850::Release()
{
    if (m_pFuncCenter != NULL)
    {
        delete m_pFuncCenter;
        m_pFuncCenter = NULL;
    }
    if (m_pNv != NULL)
    {
        delete m_pNv;
        m_pNv = NULL;
    }
    if (m_pNal != NULL)
    {
        delete m_pNal;
        m_pNal = NULL;
    }
    if (m_pFileConfig != NULL)
    {
        delete m_pFileConfig;
        m_pFileConfig = NULL;
    }
    if (m_pTxApi != NULL)
    {
        delete m_pTxApi;
        m_pTxApi = NULL;
    }
    if (m_pRxApi != NULL)
    {
        delete m_pRxApi;
        m_pRxApi = NULL;
    }
    if (m_pTxAlgoMainPcc != NULL)
    {
        delete m_pTxAlgoMainPcc;
        m_pTxAlgoMainPcc = NULL;
    }
    if (m_pTxAlgoDivScc != NULL)
    {
        delete m_pTxAlgoDivScc;
        m_pTxAlgoDivScc = NULL;
    }
    if (m_pRxAlgoMainPcc != NULL)
    {
        delete m_pRxAlgoMainPcc;
        m_pRxAlgoMainPcc = NULL;
    }
    if (m_pRxAlgoDivPcc != NULL)
    {
        delete m_pRxAlgoDivPcc;
        m_pRxAlgoDivPcc = NULL;
    }

    return SP_OK;
}

void CLteGs_UIS8850::Serialization( std::vector<uint8>* parrData )
{
    m_pNal->Serialization(parrData);
}
