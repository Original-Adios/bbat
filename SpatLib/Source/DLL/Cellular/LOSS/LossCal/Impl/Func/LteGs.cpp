#include "StdAfx.h"
#include "LteGs.h"
#include "ImpBase.h"
#include "ApiTxV21.h"
#include "ApiRxV1.h"

#include "ApiTxV22.h"
#include "ApiRxV3.h"

#include "ApiTxModV3.h"
#include "ApiRxModV3.h"

CLteGs::CLteGs(CImpBase *pSpat)
: ILossFunc(pSpat)
, m_pFuncCenter(NULL)
, m_CurrentCa(LTE_CA_PCC)
, m_pNvHelper(NULL)
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

CLteGs::~CLteGs(void)
{

}

SPRESULT CLteGs::Run()
{
	if (m_pImp->m_eModemVer == MV_V2)
	{
		CHKRESULT(SP_lteActive(m_pImp->m_hDUT, TRUE));

		CHKRESULT(m_pNvHelper->Load());

		CHKRESULT(m_pSpatBase->m_pRFTester->SetNetMode(NM_LTE));
		CHKRESULT(m_pSpatBase->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TRX_FDT, NULL));

		if (!m_pNvHelper->m_bSameDownloadChecksum)
		{
			CHKRESULT(m_pNal->GetConfig());
			CHKRESULT(m_pNal->InitResult());
			m_pNvHelper->ApplyChecksum();
		}
		CHKRESULT(m_pNal->ClearResult());

		CHKRESULT(m_pTxAlgoMainPcc->Init());
		CHKRESULT(m_pTxAlgoDivScc->Init());
		CHKRESULT(m_pRxAlgoMainPcc->Init());
		CHKRESULT(m_pRxAlgoDivPcc->Init());

		CHKRESULT(m_pTxAlgoMainPcc->Run());
		CHKRESULT(m_pTxAlgoDivScc->Run());
		CHKRESULT(m_pRxAlgoMainPcc->Run());
		CHKRESULT(m_pRxAlgoDivPcc->Run());
	}
	else if (m_pImp->m_eModemVer == MV_V3)
	{
		CHKRESULT(SP_ModemV3_LTE_Active(m_pImp->m_hDUT, TRUE));
		CHKRESULT(m_pNvHelper->Load());
		CHKRESULT(m_pSpatBase->m_pRFTester->SetNetMode(NM_LTE));
		CHKRESULT(m_pSpatBase->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TRX_FDT, NULL));

		CHKRESULT(m_pNal->GetConfig());
		CHKRESULT(m_pNal->InitResult());
		CHKRESULT(m_pNal->ClearResult());

		for( int m = 0 ;m < MAX_RF_PortCompNum;m++ )
		{
			m_pTxAlgoMainPcc->SetAntRfChain(RF_ANT_1st, (RF_PortCompIndex_E)m);
			CHKRESULT(m_pTxAlgoMainPcc->Init());
			m_pRxAlgoMainPcc->SetAntRfChain(RF_ANT_1st, (RF_PortCompIndex_E)m);
			CHKRESULT(m_pRxAlgoMainPcc->Init());
			m_pRxAlgoDivPcc->SetAntRfChain(RF_ANT_2nd, (RF_PortCompIndex_E)m);
			CHKRESULT(m_pRxAlgoDivPcc->Init());

			CHKRESULT(m_pTxAlgoMainPcc->Run());
			CHKRESULT(m_pTxAlgoDivScc->Run());
			CHKRESULT(m_pRxAlgoMainPcc->Run());
			CHKRESULT(m_pRxAlgoDivPcc->Run());
		}

	}  

    return SP_OK;
}

SPRESULT CLteGs::Init()
{
    if (m_pFuncCenter == NULL)
    {
        m_pFuncCenter = new CFuncCenter();
    }
    m_pFuncCenter->AddFunc(SPATBASE, m_pSpatBase);
    m_pFuncCenter->AddFunc(CURRENT_CA, &m_CurrentCa);

	if (m_pFileConfig == NULL)
	{
		m_pFileConfig = new CFileConfig(FUNC_FILE_CONFIG, m_pFuncCenter);
	}

	m_pFileConfig->Load();
	if (m_pNal == NULL)
	{
		m_pNal = new CNalGs(FUNC_INAL, m_pFuncCenter);
	}

	if (m_pImp->m_eModemVer == MV_V2)
	{
		if (m_pNvHelper == NULL)
		{
			m_pNvHelper = new CNvHelper(FUNC_INVHelper, m_pFuncCenter,m_pImp->m_eModemVer == MV_V2);
		}

		if (m_pTxApi == NULL)
		{
			m_pTxApi = new CApiTxV22(API_APC, m_pFuncCenter);
		}
		if (m_pRxApi == NULL)
		{
			m_pRxApi = new CApiRxV3(API_AGC, m_pFuncCenter);
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
	}
	else if(m_pImp->m_eModemVer == MV_V3)
	{
		if (m_pNvHelper == NULL)
		{
			m_pNvHelper = new CNvHelper(FUNC_INVHelper, m_pFuncCenter,m_pImp->m_eModemVer == MV_V2);
		}

		if (m_pTxApi == NULL)
		{
			m_pTxApi = new CApiTxModV3(API_APC, m_pFuncCenter);
		}

		if (m_pRxApi == NULL)
		{
			m_pRxApi = new CApiRxModV3(API_AGC, m_pFuncCenter);
		}

		if (m_pTxAlgoMainPcc == NULL)
		{
			m_pTxAlgoMainPcc = new CAlgoGsTxModV3(ALGO_APC_MAIN_PCC, m_pFuncCenter);
		}
		if (m_pTxAlgoDivScc == NULL)
		{
			m_pTxAlgoDivScc = new CAlgoGsTxModV3(ALGO_APC_DIV_SCC, m_pFuncCenter);
		}
		if (m_pRxAlgoMainPcc == NULL)
		{
			m_pRxAlgoMainPcc = new CAlgoGsRxModV3(ALGO_AGC_MAIN_PCC, m_pFuncCenter);
		}
		if (m_pRxAlgoDivPcc == NULL)
		{
			m_pRxAlgoDivPcc = new CAlgoGsRxModV3(ALGO_AGC_DIV_PCC, m_pFuncCenter);
		}
	}

	CHKRESULT(m_pFileConfig       ->PreInit());
	CHKRESULT(m_pNvHelper         ->PreInit());
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

    return SP_OK;
}

SPRESULT CLteGs::Release()
{
    if (m_pFuncCenter != NULL)
    {
        delete m_pFuncCenter;
        m_pFuncCenter = NULL;
    }
    if (m_pNvHelper != NULL)
    {
        delete m_pNvHelper;
        m_pNvHelper = NULL;
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

void CLteGs::Serialization( std::vector<uint8>* parrData )
{
	m_pNal->Serialization(parrData);
}
