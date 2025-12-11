#include "stdafx.h"
#include "NrAlgoClcV5.h"

CNrAlgoClcV5::CNrAlgoClcV5(CImpBase* pImp) : CNrAlgoClc(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;

    this->m_pApi = &m_Api;
}

CNrAlgoClcV5::~CNrAlgoClcV5(void)
{
}
