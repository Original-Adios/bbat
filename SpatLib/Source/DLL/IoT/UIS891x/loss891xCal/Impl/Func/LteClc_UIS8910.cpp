#include "StdAfx.h"
#include "LteClc_UIS8910.h"
#include "ImpBaseUIS8910.h"
#include "ApiTxUIS8910.h"
#include "ApiRxUIS8910.h"

CLteClc_UIS8910::CLteClc_UIS8910(CImpBaseUIS8910 *pSpat)
: ILossFunc_UIS8910(pSpat)
, m_pFuncCenter(NULL)
, m_CurrentCa(LTE_CA_PCC)
, m_pFileConfig(NULL)
, m_pNal(NULL)
, m_pTxApi(NULL)
, m_pRxApi(NULL)
, m_pTxAlgoMainPcc(NULL)
, m_pTxAlgoDivScc(NULL)
, m_pRxAlgoMainPcc(NULL)
, m_pRxAlgoDivPcc (NULL)
{
    m_pSpatBase = (CSpatBase*)m_pImp;
}

CLteClc_UIS8910::~CLteClc_UIS8910(void)
{
}

SPRESULT CLteClc_UIS8910::Run()
{
    CHKRESULT(SP_lteActive(m_pImp->m_hDUT, TRUE));

    CHKRESULT(m_pSpatBase->m_pRFTester->SetNetMode(NM_LTE));
    CHKRESULT(m_pSpatBase->m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TRX_FDT, NULL));
	CHKRESULT(IniFuncMod());
    CHKRESULT(m_pNal->InitResult());
    CHKRESULT(m_pNal->ClearResult());
    CHKRESULT(m_pNal->InitResultAcc());
    CHKRESULT(m_pNal->ClearResultAcc());

    CHKRESULT(m_pRxAlgoMainPcc->Init());
    CHKRESULT(m_pRxAlgoDivPcc->Init());
    CHKRESULT(m_pTxAlgoMainPcc->Init());
    CHKRESULT(m_pTxAlgoDivScc->Init());

	for(int i=0;i<m_pFileConfig->m_nTestNum;i++)
	{
		CHKRESULT(m_pNal->InitResult());
		CHKRESULT(m_pNal->ClearResult())
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

    m_pNal->SetResult();

    CHKRESULT(SP_lteActive(m_pImp->m_hDUT, FALSE));

    return SP_OK;
}

SPRESULT CLteClc_UIS8910::Init()
{
    if (m_pFuncCenter == NULL)
    {
        m_pFuncCenter = new CFuncCenter();
    }
    m_pFuncCenter->AddFunc(SPATBASE, (CSpatBase*)m_pImp);
    m_pFuncCenter->AddFunc(CURRENT_CA, &m_CurrentCa);

    if (m_pFileConfig == NULL)
    {
        m_pFileConfig = new CFileConfig_UIS8910(FUNC_FILE_CONFIG, m_pFuncCenter);
    }

    m_pFileConfig->Load();
    m_pFileConfig->m_dLossValLower = m_pImp->m_dLossValLower;
    m_pFileConfig->m_dLossValUpper = m_pImp->m_dLossValUpper;
    m_pFileConfig->m_pstrNegativeLossDetected = &m_pImp->m_strNegativeLossDetected;

    if (m_pNal == NULL)
    {
        m_pNal = new CNalClc_UIS8910(FUNC_INAL, m_pFuncCenter);
    }
    if (m_pTxAlgoMainPcc == NULL)
    {
        m_pTxAlgoMainPcc = new CAlgoClcTx(ALGO_APC_MAIN_PCC, m_pFuncCenter);
    }
    if (m_pTxAlgoDivScc == NULL)
    {
        m_pTxAlgoDivScc = new CAlgoClcTx(ALGO_APC_DIV_SCC, m_pFuncCenter);
    }
    if (m_pRxAlgoMainPcc == NULL)
    {
        m_pRxAlgoMainPcc = new CAlgoClcRx(ALGO_AGC_MAIN_PCC, m_pFuncCenter);
    }
    if (m_pRxAlgoDivPcc == NULL)
    {
        m_pRxAlgoDivPcc = new CAlgoClcRx(ALGO_AGC_DIV_PCC, m_pFuncCenter);
    }

    return SP_OK;
}

SPRESULT CLteClc_UIS8910::Release()
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

void CLteClc_UIS8910::Deserialization( std::vector<uint8>* parrData )
{
    m_pNal->Deserialization(parrData);
}

SPRESULT CLteClc_UIS8910::IniFuncMod( void )
{
#if 0
	BOOL bBand66 = FALSE;
	for (uint32 nCh=0; nCh<m_pNal->m_arrConfig[RF_ANT_1st].size();nCh++)
	{
		int nMeasCh = m_pNal->m_arrConfig[RF_ANT_1st][nCh].usChannel;
		if (nMeasCh > 65535)
		{
			bBand66 = TRUE;
		}
	}
	if(bBand66)
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
		m_pTxApi = new CApiTxUIS8910(API_APC, m_pFuncCenter);
	}
	if (m_pRxApi == NULL)
	{
		m_pRxApi = new CApiRxUIS8910(API_AGC, m_pFuncCenter);
	}

	CHKRESULT(m_pFileConfig       ->PreInit());
	CHKRESULT(m_pNal              ->PreInit());
	CHKRESULT(m_pTxApi            ->PreInit());
	CHKRESULT(m_pRxApi            ->PreInit());
	CHKRESULT(m_pTxAlgoMainPcc    ->PreInit());
	CHKRESULT(m_pTxAlgoDivScc     ->PreInit());
	CHKRESULT(m_pRxAlgoMainPcc    ->PreInit());
	CHKRESULT(m_pRxAlgoDivPcc     ->PreInit());

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


