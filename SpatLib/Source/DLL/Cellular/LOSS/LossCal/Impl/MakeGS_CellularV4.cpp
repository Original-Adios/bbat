#include "StdAfx.h"
#include "MakeGS_CellularV4.h"
#include "GsmGs.h"
#include "TdGs.h"
#include "WcdmaGs.h"
#include "C2KGs.h"
#include "LteGsV4.h"
#include "NrAlgoGsV4.h"
#include "DMRGs.h"
#include "gsmUtility.h"
#include "CdmaUtility.h"
#include "LteUtility.h"
#include "Utility.h"
#include "wcnUtility.h"
#include "WCNGS.h"
#include "NrUtility.h"

IMPLEMENT_RUNTIME_CLASS(CMakeGSCellularV4)
CMakeGSCellularV4::CMakeGSCellularV4(void)
{
}

CMakeGSCellularV4::~CMakeGSCellularV4(void)
{
}


SPRESULT CMakeGSCellularV4::__InitAction( void )
{
    m_pGsmFunc = new CGsmGs(this);
    m_pGsmFunc->Init();
    m_pWcdmaFunc = new CWcdmaGs(this);
    m_pWcdmaFunc->Init();
    m_pTdFunc = new CTdGs(this);
    m_pTdFunc->Init();
    m_pC2KFunc = new CC2KGs(this);
    m_pC2KFunc->Init();
    m_pLteFunc = new CLteGsV4(this);
    m_pLteFunc->Init();
    m_pNrFunc = new CNrAlgoGsV4(this);
    m_pNrFunc->Init();
    m_pDMRCalFunc = new CDMRGs(this);
    m_pDMRCalFunc->Init();
    m_pDMRFtFunc = new CDMRGs(this);
    m_pDMRFtFunc->Init();
    m_pWCNFunc = new CWCNGS(this);
    m_pWCNFunc->Init();
    return SP_OK;
}

