#include "StdAfx.h"
#include <cassert>
#include "BTTestSysSongShan.h"
#include "BTLoadXML_BLE53.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CBTTestSysSongShan)

CBTTestSysSongShan::CBTTestSysSongShan(void)
: m_pEnterMode(NULL)
, m_pBtApi(NULL)
, m_pRfPathSwitch(NULL)
{
	m_pBdrMeas = NULL;
	m_pEdrMeas = NULL;
	m_pBle53Meas = NULL;
}

CBTTestSysSongShan::~CBTTestSysSongShan(void)
{
}

SPRESULT CBTTestSysSongShan::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());

	if (m_VecBTParamBandImp.stBTParamBand.empty())
	{
        LogFmtStrA(SPLOGLV_ERROR, "The BT config parameter is null" );
        return SP_E_SPAT_INVALID_PARAMETER;
	}
    m_pBtApi = new CBTApiAT(this->m_hDUT, BDR);
	if(NULL == m_pBtApi)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTApiAT fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
    m_pEnterMode = new CBTEnterMode(m_pBtApi, BDR);
	if(NULL == m_pEnterMode)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTEnterMode fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pRfPathSwitch = new CBTRFPathSwitch(m_pBtApi);
	if(NULL == m_pRfPathSwitch)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTRFPathSwitch fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}

	m_pBdrMeas = new CBTMeasBDR(this, m_pBtApi);
	if(NULL == m_pBdrMeas)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTMeasBDR fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pEdrMeas = new CBTMeasEDR(this, m_pBtApi);
	if(NULL == m_pEdrMeas)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTMeasEDR fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	m_pBle53Meas = new CBTMeasBLE_BLE53(this, m_pBtApi);
	if(NULL == m_pBle53Meas)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTMeasBLEEx fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	
	return SP_OK;
}

BOOL CBTTestSysSongShan::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_VecBTParamBandImp.stBTParamBand.empty())
	{
		m_VecBTParamBandImp.stBTParamBand.clear();
	}
    CBTLoadXML_BLE53 *pLoadXml = new CBTLoadXML_BLE53(this);
	
	if(NULL == pLoadXml)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CBTLoadXML fail!");
		return SP_E_SPAT_ALLOC_MEMORY;
	}

    SPRESULT res = (pLoadXml->BTLoadXmlFile(m_VecBTParamBandImp.stBTParamBand));

	delete pLoadXml;

	return (SP_OK == res);
}

SPRESULT CBTTestSysSongShan::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Bluetooth);
	SPRESULT eBandStateRlst = SP_OK;
	
	for (int nBIndx=0; nBIndx<(int)m_VecBTParamBandImp.stBTParamBand.size(); nBIndx++)
	{
		if (_IsUserStop())
		{ 
			return SP_E_USER_ABORT;
		}
		if(0 == m_VecBTParamBandImp.stBTParamBand[nBIndx].vecBTFileParamChan.size())
		{
			continue;
		}

		SPRESULT eRes = SP_OK;
		BT_TYPE eMode = m_VecBTParamBandImp.stBTParamBand[nBIndx].eMode;
		CHKRESULT(m_pEnterMode->BT_SetBtType(eMode));
		SPRESULT eModeRslt = m_pEnterMode->BT_ModeEnable(BT_NST, TRUE);
		_UiSendMsg("BT Enter Mode" ,LEVEL_ITEM, 0, eModeRslt == SP_OK ? 0:1, 0, CwcnUtility::BT_BAND_NAME[eMode]);
		CHKRESULT(eModeRslt);
		BT_RFPATH_ENUM eRfPath = m_VecBTParamBandImp.stBTParamBand[nBIndx].vecBTFileParamChan[0].ePath;
		eRes = m_pRfPathSwitch->BT_RFPathSet(eRfPath);
		_UiSendMsg("Set Ant" ,LEVEL_ITEM, 0, eRes == SP_OK ? 0:1, 0, CwcnUtility::BT_ANT_NAME[eRfPath]);
		CHKRESULT(eRes);


		int nRfSwitchAnt1st = m_VecBTParamBandImp.stBTParamBand[nBIndx].vecBTFileParamChan[0].nRfSwitchAnt1st;
		int nRfSwitchAnt2nd = m_VecBTParamBandImp.stBTParamBand[nBIndx].vecBTFileParamChan[0].nRfSwitchAnt2nd;

		switch(eMode)
		{
		case BDR:
		{
			//Customer control RF Port
			if (ANT_SINGLE == eRfPath && nRfSwitchAnt1st > 0)
			{
				m_pBdrMeas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt1st, nRfSwitchAnt1st);
			}

			if (ANT_SHARED == eRfPath && nRfSwitchAnt2nd > 0)
			{
				m_pBdrMeas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt2nd, nRfSwitchAnt2nd);
			}

			m_pBdrMeas->BTParamSet(&m_VecBTParamBandImp.stBTParamBand[nBIndx]);
			eRes = m_pBdrMeas->BTMeasPerform();
			//Customer control RF Port
			m_pBdrMeas->m_Customize.ResetRfSwitch();
		}
			break;
		case EDR:
		{
			//Customer control RF Port
			if (ANT_SINGLE == eRfPath && nRfSwitchAnt1st > 0)
			{
				m_pEdrMeas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt1st, nRfSwitchAnt1st);
			}

			if (ANT_SHARED == eRfPath && nRfSwitchAnt2nd > 0)
			{
				m_pEdrMeas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt2nd, nRfSwitchAnt2nd);
			}
			m_pEdrMeas->BTParamSet(&m_VecBTParamBandImp.stBTParamBand[nBIndx]);
			eRes = m_pEdrMeas->BTMeasPerform();
			//Customer control RF Port
			m_pEdrMeas->m_Customize.ResetRfSwitch();
		}
			break;
		case BLE_53:
		{
			//Customer control RF Port
			if (ANT_SINGLE == eRfPath && nRfSwitchAnt1st > 0)
			{
				m_pBle53Meas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt1st, nRfSwitchAnt1st);
			}

			if (ANT_SHARED == eRfPath && nRfSwitchAnt2nd > 0)
			{
				m_pBle53Meas->m_Customize.SetRfSwitch(eMode, nRfSwitchAnt2nd, nRfSwitchAnt2nd);
			}
			m_pBle53Meas->BTParamSet(&m_VecBTParamBandImp.stBTParamBand[nBIndx]);
			eRes = m_pBle53Meas->BTMeasPerform();
			//Customer control RF Port
			m_pBle53Meas->m_Customize.ResetRfSwitch();
		}
			break;
		default:
			break;
		}

		eModeRslt = m_pEnterMode->BT_ModeEnable(BT_LEAVE, FALSE);
		_UiSendMsg("BT Leave Mode" ,LEVEL_ITEM, 0, eModeRslt == SP_OK ? 0:1, 0, CwcnUtility::BT_BAND_NAME[eMode]);
		CHKRESULT(eModeRslt);

		if (m_bFailStop)
		{
			CHKRESULT(eRes);
		}
		else
		{
			if(SP_OK == eBandStateRlst)
			{
				eBandStateRlst = eRes;
			}
		}		
	}

	return eBandStateRlst;
}

void CBTTestSysSongShan::__LeaveAction( void )
{
}

SPRESULT CBTTestSysSongShan::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }
	//m_pRfPathSwitch
	if (NULL != m_pRfPathSwitch)
	{
		delete m_pRfPathSwitch;
		m_pRfPathSwitch = NULL;
	}

    if (NULL != m_pBtApi)
    {
        delete m_pBtApi;
        m_pBtApi = NULL;
    }

	if (NULL != m_pBle53Meas)
	{
		delete m_pBle53Meas;
		m_pBle53Meas = NULL;
	}
    return SP_OK;
}