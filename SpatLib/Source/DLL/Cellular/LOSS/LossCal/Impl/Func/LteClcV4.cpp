#include "StdAfx.h"
#include "LteClcV4.h"
#include "ImpBase.h"
#include "ApiTxModV4.h"
#include "ApiRxModV4.h"

CLteClcV4::CLteClcV4(CImpBase *pSpat)
: ILossFunc(pSpat)
, m_pFuncCenter(NULL)
, m_CurrentCa(LTE_CA_PCC)
, m_pFileConfig(NULL)
, m_pNal(NULL)
, m_pTxApi(NULL)
, m_pRxApi(NULL)
, m_pTxAlgo(NULL)
, m_pRxAlgo (NULL)
, m_pNvHelper(NULL)
{
    m_pSpatBase = (CSpatBase*)m_pImp;
}

CLteClcV4::~CLteClcV4(void)
{
	SP_ModemV3_LTE_Active(m_pImp->m_hDUT, FALSE);
	SP_LTE_Base_Release(m_pImp->m_hDUT);
}

SPRESULT CLteClcV4::Run()
{
	CHKRESULT(SP_ModemV3_LTE_Active(m_pImp->m_hDUT, TRUE));
	CHKRESULT(SP_LTE_Base_Init(m_pImp->m_hDUT, MODEM_V4));
	CHKRESULT(m_pSpatBase->m_pRFTester->SetNetMode(NM_LTE));
	CHKRESULT(m_pSpatBase->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TRX_FDT, NULL));
	CHKRESULT(m_pNvHelper->LoadClcPorComp());
	CHKRESULT(m_pNal->InitResult());
	CHKRESULT(m_pNal->ClearResult());

	for (int m = 0; m < MAX_RF_PortCompNum; m++)
	{
		for (int i = LTE_RF_ANT_MAIN; i < LTE_RF_ANT_DIVERSITY; i++)
		{
			m_pTxAlgo->SetAntRfChain(RF_ANT_1st, (RF_PortCompIndex_E)m);
			CHKRESULT(m_pTxAlgo->Init());
			CHKRESULT(m_pTxAlgo->Run());
		}
	}

	for (int m = 0; m < MAX_RF_PortCompNum; m++)
	{
		for (int i = LTE_RF_ANT_MAIN; i < MAX_LTE_RF_ANTENNA; i++)
		{
			m_pRxAlgo->SetAntRfChain((RF_ANT_E)i, (RF_PortCompIndex_E)m);
			CHKRESULT(m_pRxAlgo->Init());
			CHKRESULT(m_pRxAlgo->Run());
		}
	}

	CHKRESULT( m_pNal->SetResult() );
	SP_LTE_Base_Release(m_pImp->m_hDUT);

    return SP_OK;
}

SPRESULT CLteClcV4::Init()
{
    if (m_pFuncCenter == NULL)
    {
        m_pFuncCenter = new CFuncCenter();
    }
    m_pFuncCenter->AddFunc(SPATBASE, (CSpatBase*)m_pImp);
    m_pFuncCenter->AddFunc(CURRENT_CA, &m_CurrentCa);

	if (m_pFileConfig == NULL)
	{
		m_pFileConfig = new CFileConfig(FUNC_FILE_CONFIG, m_pFuncCenter);
	}

	m_pFileConfig->Load();
	m_pFileConfig->m_dPreLoss = m_pImp->m_dPreLoss;
	m_pFileConfig->m_dLossValLower = m_pImp->m_dLossValLower;
	m_pFileConfig->m_dLossValUpper = m_pImp->m_dLossValUpper;
	m_pFileConfig->m_pstrNegativeLossDetected = &m_pImp->m_strNegativeLossDetected;
	
    if (m_pNal == NULL)
    {
        m_pNal = new CNalClcV4(FUNC_INAL, m_pFuncCenter);
    }

	if (m_pNvHelper == NULL)
	{
		m_pNvHelper = new CNvHelperV4(FUNC_INVHelper, m_pFuncCenter);
	}
	if (m_pTxAlgo == NULL)
	{
		m_pTxAlgo = new CAlgoClcTxModV4(ALGO_MODEM_V4_APC, m_pFuncCenter);
	}
	if (m_pRxAlgo == NULL)
	{
		m_pRxAlgo = new CAlgoClcRxModV4(ALGO_MODEM_V4_AGC, m_pFuncCenter);
	}
	if (m_pTxApi == NULL)
	{
		m_pTxApi = new CApiTxModV4(API_APC, m_pFuncCenter);
	}
	if (m_pRxApi == NULL)
	{
		m_pRxApi = new CApiRxModV4(API_AGC, m_pFuncCenter);
	}

	CHKRESULT(m_pNvHelper         ->PreInit());
	CHKRESULT(m_pFileConfig       ->PreInit());
	CHKRESULT(m_pNal              ->PreInit());
	CHKRESULT(m_pTxApi            ->PreInit());
	CHKRESULT(m_pRxApi            ->PreInit());

	CHKRESULT(m_pTxAlgo           ->PreInit());
	CHKRESULT(m_pRxAlgo           ->PreInit());

    return SP_OK;
}

SPRESULT CLteClcV4::Release()
{
    if (m_pFuncCenter != NULL)
    {
        delete m_pFuncCenter;
        m_pFuncCenter = NULL;
    } 

    if (m_pNal != NULL)
    {
        delete m_pNal;
        m_pNal = NULL;
    }

	if (m_pNvHelper != NULL)
	{
		delete m_pNvHelper;
		m_pNvHelper = NULL;
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
    if (m_pTxAlgo != NULL)
    {
        delete m_pTxAlgo;
        m_pTxAlgo = NULL;
    }
    if (m_pRxAlgo != NULL)
    {
        delete m_pRxAlgo;
        m_pRxAlgo = NULL;
    }

    return SP_OK;
}

void CLteClcV4::Deserialization( std::vector<uint8>* parrData )
{
    m_pNal->Deserialization(parrData);
}

SPRESULT CLteClcV4::LoadBandInfo()
{
	m_pImp->LogFmtStrA(SPLOGLV_INFO, "%s", __FUNCTION__);
	return m_pNvHelper->m_pNvV4->LoadSupportBandInfo();
}

