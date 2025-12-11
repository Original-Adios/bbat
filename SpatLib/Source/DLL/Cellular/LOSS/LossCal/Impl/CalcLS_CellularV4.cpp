#include "StdAfx.h"
#include "CalcLS_CellularV4.h"
#include "GsmClc.h"
#include "TdClc.h"
#include "WcdmaClc.h"
#include "C2KClc.h"
#include "LteClcV4.h"
#include "DMRClc.h"
#include "WCNCLC.h"
#include "NrAlgoClcV4.h"

IMPLEMENT_RUNTIME_CLASS(CCalcLSCellularV4)
CCalcLSCellularV4::CCalcLSCellularV4(void)
{

}

CCalcLSCellularV4::~CCalcLSCellularV4(void)
{

}

SPRESULT CCalcLSCellularV4::__InitAction(void)
{
    m_pGsmFunc = new CGsmClc(this);
    m_pGsmFunc->Init();

    m_pTdFunc = new CTdClc(this);
    m_pTdFunc->Init();

    m_pWcdmaFunc = new CWcdmaClc(this);
    m_pWcdmaFunc->Init();

    m_pC2KFunc = new CC2KClc(this);
    m_pC2KFunc->Init();

    m_pLteFunc = new CLteClcV4(this);
    m_pLteFunc->Init();

    m_pNrFunc = new CNrAlgoClcV4(this);
    m_pNrFunc->Init();
    m_pDMRCalFunc = new CDMRClc(this);
    m_pDMRCalFunc->Init();

    m_pDMRFtFunc = new CDMRClc(this);
    m_pDMRFtFunc->Init();

    m_pWCNFunc = new CWCNCLC(this);
    m_pWCNFunc->Init();

    return SP_OK;
}
