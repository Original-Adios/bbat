#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanMeasSongShanW6DL.h"
//#include "WlanMeasCW.h"
#include "WlanLoadXMLSongShanW6.h"
#include "WlanTestSysSongShanW6DL.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanTestSysSongShanW6DL)

CWlanTestSysSongShanW6DL::CWlanTestSysSongShanW6DL(void)
{
}

CWlanTestSysSongShanW6DL::~CWlanTestSysSongShanW6DL(void)
{
}

SPRESULT CWlanTestSysSongShanW6DL::__InitAction(void)
{
    auto _function = SimpleAop(this, __FUNCTION__);
    CHKRESULT(__super::__InitAction());

    if (m_stWlanParamImp.VecWlanParamBandImp.empty())
    {
        LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null");
        return SP_E_SPAT_INVALID_PARAMETER;
    }
    m_pEnterMode = new CWlanEnterMode(this);
    if (NULL == m_pEnterMode)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanEnterMode failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    m_pWlanAnt = new CWlanAntSwitch(this);
    if (NULL == m_pWlanAnt)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanAntSwitch failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    m_pWlanMeas = new CWlanMeaSongShanW6DL(this);
    if (NULL == m_pWlanMeas)
    {
        LogFmtStrA(SPLOGLV_ERROR, "new CWlanMeasOS80 failed!");
        return SP_E_SPAT_ALLOC_MEMORY;
    }
    return SP_OK;
}