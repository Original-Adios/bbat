#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanMeas24G.h"
#include "WlanMeas5G.h"
#include "WlanMeasCW.h"
#include "WlanTestSys.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanTestSys)

CWlanTestSys::CWlanTestSys(void)
: m_pEnterMode(NULL)
, m_pLoadXml(NULL)
{
}

CWlanTestSys::~CWlanTestSys(void)
{
}

SPRESULT CWlanTestSys::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	//m_VecWlanParamBandImp.resize(0);	
	CHKRESULT(__super::__InitAction());

	if (m_VecWlanParamBandImp.empty())
	{
        LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null" );
        return SP_E_SPAT_INVALID_PARAMETER;
	}
    m_pEnterMode = new CWlanEnterMode(this);
	m_pvecWlanMeas.clear();

	try
	{
		for (unsigned int nBandIndex = 0; nBandIndex < m_VecWlanParamBandImp.size(); nBandIndex++)
		{
			if (IS_WLAN_24G(m_VecWlanParamBandImp[nBandIndex].eMode))
			{
				CWlanMeasBase *pclWlanMeas24G = new CWlanMeas24G(this);
				pclWlanMeas24G->WlanParamSet(&m_VecWlanParamBandImp[nBandIndex]);
				m_pvecWlanMeas.push_back(pclWlanMeas24G);
			}
			else if (IS_WLAN_50G(m_VecWlanParamBandImp[nBandIndex].eMode))
			{
				CWlanMeasBase *pclWlanMeas5G = new CWlanMeas5G(this);
				pclWlanMeas5G->WlanParamSet(&m_VecWlanParamBandImp[nBandIndex]);
				m_pvecWlanMeas.push_back(pclWlanMeas5G);
			}
			else if (IS_WLAN_CW(m_VecWlanParamBandImp[nBandIndex].eMode))
			{
				CWlanMeasBase *pclWlanMeasCW = new CWlanMeasCW(this);
				pclWlanMeasCW->WlanParamSet(&m_VecWlanParamBandImp[nBandIndex]);
				m_pvecWlanMeas.push_back(pclWlanMeasCW);
			}
			else
			{
				continue;
			}
		}
	}
	catch (const std::bad_alloc& /*e*/)
	{
		assert(0);
		return SP_E_SPAT_ALLOC_MEMORY;
		
	}
	return SP_OK;
}

BOOL CWlanTestSys::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_VecWlanParamBandImp.empty())
	{
		m_VecWlanParamBandImp.clear();
	}
    m_pLoadXml = new CWlanLoadXML(this);

    CHKRESULT(m_pLoadXml->WlanLoadXmlFile(m_VecWlanParamBandImp));

	return TRUE;
}


SPRESULT CWlanTestSys::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Wlan);
 // CHKRESULT(ChangeDUTRunMode()); // 2018/03/22 @JXP This has been done by CSpatBase::PollAction
	if (m_pvecWlanMeas.empty())
	{
		LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null" );
		return SP_E_SPAT_INVALID_PARAMETER;
	}

    //Enable wlan mode
	SPRESULT eRes = m_pEnterMode->Wlan_ModeEnable(TRUE);
	_UiSendMsg("WlanEnterEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
	CHKRESULT(eRes);

    //Wlan performance meas
	vector<CWlanMeasBase*>::iterator pitWlanBase;
	SPRESULT eWlanBandRlst = SP_OK;

	for (pitWlanBase = m_pvecWlanMeas.begin(); pitWlanBase != m_pvecWlanMeas.end(); pitWlanBase++)
	{
		eRes = (*pitWlanBase)->WlanMeasPerform();
		//Customer control RF Port
		(*pitWlanBase)->m_Customize.ResetRfSwitch();
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
	eRes = m_pEnterMode->Wlan_ModeEnable(FALSE);
	_UiSendMsg("WlanLeaveEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
	CHKRESULT(eRes);
	return eWlanBandRlst;
}

void CWlanTestSys::__LeaveAction( void )
{
}

SPRESULT CWlanTestSys::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }

    if (NULL != m_pLoadXml)
    {
        delete m_pLoadXml;
        m_pLoadXml = NULL;
    }
    
    vector<CWlanMeasBase*>::iterator itWlanBand;
    for (itWlanBand = m_pvecWlanMeas.begin(); itWlanBand != m_pvecWlanMeas.end(); ++itWlanBand)
    {
        delete *itWlanBand;
        *itWlanBand = NULL;
    }
    m_pvecWlanMeas.clear();

    return SP_OK;
}