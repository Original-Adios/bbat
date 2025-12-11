#include "StdAfx.h"
#include "CheckGoldenSample.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckGoldenSample)

///
CCheckGoldenSample::CCheckGoldenSample(void)
{
   
}

CCheckGoldenSample::~CCheckGoldenSample(void)
{

}

SPRESULT CCheckGoldenSample::__PollAction(void)
{
    CHAR szSN[64] = {0};
    SPRESULT res = SP_LoadSN(m_hDUT, SN1, szSN, sizeof(szSN));
    if (SP_OK != res)
    {
        if (SP_E_PHONE_GOLDEN_SAMPLE == res)
        {
            MessageBox( 
                NULL, 
                _T("This is a golden sample, re-calibration will erase the golden parameters.\n当前的待测样机是金板，重新校准会导致金板参数擦除!"),
                _T("Golden Sample"),
                MB_OK|MB_TOPMOST|MB_ICONERROR
                );

            NOTIFY("GoldenSample", LEVEL_ITEM, 1, 0, 1);
            return res;
        }
    }

    return res;
}

