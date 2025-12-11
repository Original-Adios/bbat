#include "stdafx.h"
#include "NrAlgoClcV4.h"

CNrAlgoClcV4::CNrAlgoClcV4(CImpBase* pImp) : CNrAlgoClc(pImp)
{
    m_Api.m_hDUT = pImp->m_hDUT;
    m_Api.m_pRFTester = pImp->m_pRFTester;

    this->m_pApi = &m_Api;
}

CNrAlgoClcV4::~CNrAlgoClcV4(void)
{
}
