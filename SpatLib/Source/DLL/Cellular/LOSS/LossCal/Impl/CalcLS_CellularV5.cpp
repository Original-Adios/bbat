#include "StdAfx.h"
#include "CalcLS_CellularV5.h"
#include "GsmClc.h"
#include "TdClc.h"
#include "WcdmaClc.h"
#include "C2KClc.h"
#include "LteClcV5.h"
#include "DMRClc.h"
#include "WCNCLC.h"
#include "NrAlgoClcV5.h"

IMPLEMENT_RUNTIME_CLASS(CCalcLSCellularV5)
CCalcLSCellularV5::CCalcLSCellularV5(void)
{

}

CCalcLSCellularV5::~CCalcLSCellularV5(void)
{

}

SPRESULT CCalcLSCellularV5::__InitAction(void)
{
    m_pGsmFunc = new CGsmClc(this);
    m_pGsmFunc->Init();

    m_pTdFunc = new CTdClc(this);
    m_pTdFunc->Init();

    m_pWcdmaFunc = new CWcdmaClc(this);
    m_pWcdmaFunc->Init();

    m_pC2KFunc = new CC2KClc(this);
    m_pC2KFunc->Init();

    m_pLteFunc = new CLteClcV5(this);
    m_pLteFunc->Init();

    m_pNrFunc = new CNrAlgoClcV5(this);
    m_pNrFunc->Init();
    m_pDMRCalFunc = new CDMRClc(this);
    m_pDMRCalFunc->Init();

    m_pDMRFtFunc = new CDMRClc(this);
    m_pDMRFtFunc->Init();

    m_pWCNFunc = new CWCNCLC(this);
    m_pWCNFunc->Init();

    return SP_OK;
}

SPRESULT CCalcLSCellularV5::__PollAction(void)
{
    m_eModemVer = MV_V3;
    CHKRESULT(CCalcLS::__PollAction());
    return SP_OK;
}