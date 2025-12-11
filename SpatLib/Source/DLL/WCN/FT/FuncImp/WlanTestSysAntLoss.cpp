#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanMeas24G.h"
#include "WlanMeas5G.h"
#include "WlanMeasCW.h"
#include "WlanTestSysAntLoss.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanTestSysAntLoss)

CWlanTestSysAntLoss::CWlanTestSysAntLoss(void):m_pLoadLossXml(NULL)
{

}

CWlanTestSysAntLoss::~CWlanTestSysAntLoss(void)
{
}

BOOL CWlanTestSysAntLoss::LoadXMLConfig(void)
{
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_VecWlanParamBandImp.empty())
	{
		m_VecWlanParamBandImp.clear();
	}
    m_pLoadLossXml = new CWlanLoadLossXML(this);

    CHKRESULT(m_pLoadLossXml->WlanLoadLossXmlFile(m_VecWlanParamBandImp, m_wlanLossParam));
	//m_pWlanMeasBase->WlanLossParamSet(&m_wlanLossParam);
	//m_VecWlanParamBandImp
	return TRUE;
}


SPRESULT CWlanTestSysAntLoss::__PollAction( void )
{
	auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Wlan);

	if (m_pvecWlanMeas.empty())
	{
		LogFmtStrA(SPLOGLV_ERROR, "The wlan config parameter is null" );
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	CInstrumentLock rfLock(m_pRFTester);

    //Enable wlan mode
	SPRESULT eRes = m_pEnterMode->Wlan_ModeEnable(TRUE);
	_UiSendMsg("WlanEnterEutMode", LEVEL_ITEM, SP_OK, eRes, SP_OK, "", -1, "-");
	CHKRESULT(eRes);

    //Wlan performance meas
	vector<CWlanMeasBase*>::iterator pitWlanBase;
	SPRESULT eWlanBandRlst = SP_OK;

	for (pitWlanBase = m_pvecWlanMeas.begin(); pitWlanBase != m_pvecWlanMeas.end(); pitWlanBase++)
	{
		
		eRes = (*pitWlanBase)->WlanMeasLossPerform(m_wlanLossParam);
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



SPRESULT CWlanTestSysAntLoss::__FinalAction(void)
{
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }

    if (NULL != m_pLoadLossXml)
    {
        delete m_pLoadLossXml;
        m_pLoadLossXml = NULL;
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