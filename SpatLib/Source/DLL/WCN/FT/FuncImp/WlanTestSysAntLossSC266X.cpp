#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanMeasSongShanW6.h"
//#include "WlanMeasCW.h"
#include "WlanLoadXMLSongShanW6.h"
#include "WlanTestSysAntLossSC266X.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanTestSysAntLossSC266X)

CWlanTestSysAntLossSC266X::CWlanTestSysAntLossSC266X(void)
: m_pEnterMode(NULL)
, m_pWlanMeas(NULL)
, m_pWlanAnt(NULL)
{
}

CWlanTestSysAntLossSC266X::~CWlanTestSysAntLossSC266X(void)
{
}

SPRESULT CWlanTestSysAntLossSC266X::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());

	if (m_stWlanParamImp.VecWlanParamBandImp.empty())
	{
        LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null" );
        return SP_E_SPAT_INVALID_PARAMETER;
	}
    m_pEnterMode = new CWlanEnterMode(this);
	if(NULL == m_pEnterMode)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanEnterMode failed!" );
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pWlanAnt = new CWlanAntSwitch(this);
	if(NULL == m_pWlanAnt)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanAntSwitch failed!" );
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pWlanMeas = new CWlanMeaSongShanW6(this);
	if(NULL == m_pWlanMeas)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanMeasOS80 failed!" );
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	return SP_OK;
}

BOOL CWlanTestSysAntLossSC266X::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_stWlanParamImp.VecWlanParamBandImp.empty())
	{
		m_stWlanParamImp.VecWlanParamBandImp.clear();
	}
	CWlanLoadXMLSongShanW6* pLoadXml = NULL;
	pLoadXml = new CWlanLoadXMLSongShanW6(this);
	if(NULL == pLoadXml)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanLoadXMLOS80 failed!" );
		return FALSE;
	}

	SPRESULT rlt = pLoadXml->WlanLoadXmlFile(m_stWlanParamImp);
	if (SP_OK != rlt)
	{
		LogFmtStrA(SPLOGLV_ERROR, "WlanLoadXmlFile failed!");
		delete pLoadXml;
		pLoadXml = NULL;
		return FALSE;
	}

	m_wlanLossParam.Init();
	rlt = pLoadXml->WlanLoadLossXmlFile(m_wlanLossParam);

	delete pLoadXml;
	return (SP_OK == rlt);
}

SPRESULT CWlanTestSysAntLossSC266X::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Wlan);
    CHKRESULT(ChangeDUTRunMode());
	if (m_stWlanParamImp.VecWlanParamBandImp.empty())
	{
		LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null" );
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	
    //Enable wlan mode
	SPRESULT eRes = m_pEnterMode->Wlan_ModeEnable(TRUE);
    _UiSendMsg("WlanEnterEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
	CHKRESULT(eRes);
    //Wlan performance meas
	SPRESULT eWlanBandRlst = SP_OK;
	ANTENNA_ENUM eAnt = ANT_WLAN_INVALID;
	int eBand = -1;
	for (int i=0; i<(int)m_stWlanParamImp.VecWlanParamBandImp.size(); i++)
	{
		if(0 == m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup.size())
		{
			continue;
		}

		//SetBand
		if (eBand != m_stWlanParamImp.VecWlanParamBandImp[i].eBand)
		{
			eBand = m_stWlanParamImp.VecWlanParamBandImp[i].eBand;
			eRes = m_pEnterMode->Wlan_SetBand(m_stWlanParamImp.VecWlanParamBandImp[i].eBand);
			if (SP_OK != eRes)
			{
				_UiSendMsg("Wlan_SetBand", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
				return eRes;
			}
		}

		if(eAnt != m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt)
		{
			eAnt = m_stWlanParamImp.VecWlanParamBandImp[i].vecConfParamGroup[0].stAlgoParamGroupSub.eAnt;
			eRes = m_pWlanAnt->Wlan_AntSwitch(eAnt);
			if(SP_OK != eRes)
			{
				_UiSendMsg("WlanSwitchAnt", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
				return eRes;
			}	
		}
		m_pWlanMeas->WlanParamSet(&m_stWlanParamImp.VecWlanParamBandImp[i]);
		//eRes = m_pWlanMeas->WlanMeasPerform();
		eRes = m_pWlanMeas->WlanMeasLossPerform(m_wlanLossParam);
		//Customer control RF Port
		m_pWlanMeas->m_Customize.ResetRfSwitch();

		if (m_bFailStop)
		{
			eWlanBandRlst = eRes;
			if(SP_OK != eWlanBandRlst)
			{
				break;
			}
		}
		else
		{
			if(SP_OK == eWlanBandRlst)
			{
				eWlanBandRlst = eRes;
			}
		}
	}
	//Disable Wlan mode
	eRes = m_pEnterMode->Wlan_ModeEnable(FALSE);
	_UiSendMsg("WlanLeaveEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
	if(SP_OK == eWlanBandRlst)
	{
		return eRes;
	}
	return eWlanBandRlst;
}

void CWlanTestSysAntLossSC266X::__LeaveAction( void )
{
    
}

SPRESULT CWlanTestSysAntLossSC266X::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }
	if (NULL != m_pWlanMeas)
	{
		delete m_pWlanMeas;
		m_pWlanMeas = NULL;
	}
	if (NULL != m_pWlanAnt)
	{
		delete m_pWlanAnt;
		m_pWlanAnt = NULL;
	}

    return SP_OK;
}