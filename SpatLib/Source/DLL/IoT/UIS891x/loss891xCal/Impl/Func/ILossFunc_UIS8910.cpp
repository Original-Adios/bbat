#include "StdAfx.h"
#include "ILossFunc_UIS8910.h"

ILossFunc_UIS8910::ILossFunc_UIS8910(CImpBaseUIS8910 *pSpat)
: m_pImp(pSpat)
{
    m_eMode = SP_INVALID;
}

ILossFunc_UIS8910::~ILossFunc_UIS8910(void)
{

}

void ILossFunc_UIS8910::Serialization(std::vector<uint8>* /*parrData*/)
{
    return;
}

void ILossFunc_UIS8910::Deserialization(std::vector<uint8>* /*parrData*/)
{
    return;
}

