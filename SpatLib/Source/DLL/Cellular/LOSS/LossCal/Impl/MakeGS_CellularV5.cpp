#include "StdAfx.h"
#include "MakeGS_CellularV5.h"
#include "GsmGs.h"
#include "TdGs.h"
#include "WcdmaGs.h"
#include "C2KGs.h"
#include "LteGsV5.h"
#include "NrAlgoGsV5.h"
#include "DMRGs.h"
#include "gsmUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "Utility.h"
#include "wcnUtility.h"
#include "WCNGS.h"
#include "NrUtility.h"

IMPLEMENT_RUNTIME_CLASS(CMakeGSCellularV5)
CMakeGSCellularV5::CMakeGSCellularV5(void)
{
}

CMakeGSCellularV5::~CMakeGSCellularV5(void)
{
}


SPRESULT CMakeGSCellularV5::__InitAction( void )
{
    m_pGsmFunc = new CGsmGs(this);
    m_pGsmFunc->Init();
    m_pWcdmaFunc = new CWcdmaGs(this);
    m_pWcdmaFunc->Init();
    m_pTdFunc = new CTdGs(this);
    m_pTdFunc->Init();
    m_pC2KFunc = new CC2KGs(this);
    m_pC2KFunc->Init();
    m_pLteFunc = new CLteGsV5(this);
    m_pLteFunc->Init();
    m_pNrFunc = new CNrAlgoGsV5(this);
    m_pNrFunc->Init();
    m_pDMRCalFunc = new CDMRGs(this);
    m_pDMRCalFunc->Init();
    m_pDMRFtFunc = new CDMRGs(this);
    m_pDMRFtFunc->Init();
    m_pWCNFunc = new CWCNGS(this);
    m_pWCNFunc->Init();
    return SP_OK;
}

SPRESULT CMakeGSCellularV5::__PollAction(void)
{
    m_eModemVer = MV_V3;
    CHKRESULT(CMakeGS::__PollAction());
    return SP_OK;
}

