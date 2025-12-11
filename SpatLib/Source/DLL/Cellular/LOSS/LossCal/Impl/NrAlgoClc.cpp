#include "StdAfx.h"
#include "NrAlgoClc.h"
#include "ImpBase.h"
#include "NrUtility.h"

CNrAlgoClc::CNrAlgoClc(CImpBase *pImp) : CCommonAlgoClcBase(pImp), ILossFunc(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;

    this->m_pApi = &m_Api;

    this->m_pLoss = &pImp->m_lossVal.nrLoss;
}

CNrAlgoClc::~CNrAlgoClc(void)
{
}

LPCSTR CNrAlgoClc::GetBandName(int nBand)
{
    return CNrUtility::m_BandInfo[nBand].NameA;
}

SPRESULT CNrAlgoClc::Run()
{
    CHKRESULT(ClcLoss());


    return SP_OK;
}

SPRESULT CNrAlgoClc::Init()
{
    return SP_OK;
}

SPRESULT CNrAlgoClc::Release()
{
    return SP_OK;
}

void CNrAlgoClc::Deserialization(std::vector<uint8> *parrData)
{
    uint32 uSize = parrData->size() / sizeof(GsData);
    m_arrGsData.resize(uSize);

    memcpy(&m_arrGsData[0], &(*parrData)[0], parrData->size());
}



