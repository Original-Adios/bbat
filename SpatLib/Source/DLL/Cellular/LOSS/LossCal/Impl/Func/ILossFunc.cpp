#include "StdAfx.h"
#include "ILossFunc.h"

ILossFunc::ILossFunc(CImpBase *pSpat)
: m_pImp(pSpat)
, m_eMode(SP_INVALID)
{

}

ILossFunc::~ILossFunc(void)
{

}

void ILossFunc::Serialization(std::vector<uint8>* /*parrData*/)
{
    return;
}

void ILossFunc::Deserialization(std::vector<uint8>* /*parrData*/)
{
    return;
}

SPRESULT ILossFunc::LoadBandInfo()
{
    return SP_OK;
}

