#include "StdAfx.h"
#include <cassert>
#include "BTApiAT.h"
#include "BTGeneralMeasBDR.h"
#include "BTGeneralMeasEDR.h"
#include "BTGeneralMeasBLE.h"
#include "BTMeasSysV1.h"
#include "SimpleAop.h"

IMPLEMENT_RUNTIME_CLASS(CBTMeasSysV1)

CBTMeasSysV1::CBTMeasSysV1(void)
: m_pLoadXml(NULL)
, m_pEnterMode(NULL)
, m_pBtApi(NULL)
{
}

CBTMeasSysV1::~CBTMeasSysV1(void)
{
}

SPRESULT CBTMeasSysV1::__InitAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
	CHKRESULT(__super::__InitAction());

	if (m_VecBTParamBandImp.empty())
	{
        LogFmtStrA(SPLOGLV_ERROR, "The BT config parameter is null" );
        return SP_E_SPAT_INVALID_PARAMETER;
	}
    try
    {
        m_pBtApi = new CBTApiAT(this->m_hDUT, BDR);
        m_pEnterMode = new CBTEnterMode(m_pBtApi, BDR);
	    m_pvecBTMeas.clear();


		for (unsigned int nBandIndex = 0; nBandIndex < m_VecBTParamBandImp.size(); nBandIndex++)
		{
			if (BDR == m_VecBTParamBandImp[nBandIndex].eMode)
			{
				CBTMeasBase *pclBTMeasBdr = new CBTGeneralMeasBDR(this, m_pBtApi);
				pclBTMeasBdr->BTParamSet(&m_VecBTParamBandImp[nBandIndex]);
				m_pvecBTMeas.push_back(pclBTMeasBdr);
			}
			else if (EDR == m_VecBTParamBandImp[nBandIndex].eMode)
			{
				CBTMeasBase *pclBTMeasEdr = new CBTGeneralMeasEDR(this, m_pBtApi);
				pclBTMeasEdr->BTParamSet(&m_VecBTParamBandImp[nBandIndex]);
				m_pvecBTMeas.push_back(pclBTMeasEdr);
			}
            else if (BLE == m_VecBTParamBandImp[nBandIndex].eMode)
            {
                CBTMeasBase *pclBTMeasBle = new CBTGeneralMeasBLE(this, m_pBtApi);
                pclBTMeasBle->BTParamSet(&m_VecBTParamBandImp[nBandIndex]);
                m_pvecBTMeas.push_back(pclBTMeasBle);
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

BOOL CBTMeasSysV1::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_VecBTParamBandImp.empty())
	{
		m_VecBTParamBandImp.clear();
	}
    try
    {
         m_pLoadXml = new CBTGeneralLoadXML(this);
    }
    catch (const std::bad_alloc& /*e*/)
    {
        assert(0);
        return SP_E_SPAT_ALLOC_MEMORY;

    }

    CHKRESULT(m_pLoadXml->BTLoadXmlFile(m_VecBTParamBandImp));

	return TRUE;
}


SPRESULT CBTMeasSysV1::__PollAction( void )
{
	CInstrumentLock rfLock(m_pRFTester);
	auto _function = SimpleAop(this, __FUNCTION__);
    SetRepairMode(RepairMode_Bluetooth);
 // CHKRESULT(ChangeDUTRunMode());   // 2018/03/22 @JXP This has been done by CSpatBase::PollAction
	if (m_pvecBTMeas.empty())
	{
		LogFmtStrA(SPLOGLV_ERROR, "The BT config parameter is null" );
		return SP_E_SPAT_INVALID_PARAMETER;
	}
	
    // Bug1044543	
    CHKRESULT(m_pRFTester->SetProperty(DP_UECHIP, 0, (LPCVOID)m_ueChip));

	SPRESULT eBandStateRlst = SP_OK;
	SPRESULT eModeRslt = SP_OK;
    //BT performance meas
	vector<CBTMeasBase*>::iterator pitBTBase;
    int nBIndx = 0;
	for (pitBTBase = m_pvecBTMeas.begin(); pitBTBase != m_pvecBTMeas.end(); pitBTBase++)
	{
		BT_TYPE eMode = m_VecBTParamBandImp[nBIndx++].eMode;
		CHKRESULT(m_pEnterMode->BT_SetBtType(eMode));
		eModeRslt = m_pEnterMode->BT_ModeEnable(BT_NST, TRUE);

		_UiSendMsg("BT Enter Mode" ,LEVEL_ITEM, 0, eModeRslt == SP_OK ? 0:1, 0, CwcnUtility::BT_BAND_NAME[eMode]);
        if (SP_OK != eModeRslt)
        {
            m_pEnterMode->BT_ModeEnable(BT_LEAVE, FALSE);
            return eModeRslt;
        }

		SPRESULT res = (*pitBTBase)->BTMeasPerform();

		//Customer control RF Port
		(*pitBTBase)->m_Customize.ResetRfSwitch();

		eModeRslt= m_pEnterMode->BT_ModeEnable(BT_LEAVE, FALSE);
		_UiSendMsg("BT Leave Mode" ,LEVEL_ITEM, 0, eModeRslt == SP_OK ? 0:1, 0, CwcnUtility::BT_BAND_NAME[eMode]);
		CHKRESULT(eModeRslt);
		if (m_bFailStop)
		{
			eBandStateRlst = res;
			if(SP_OK != eBandStateRlst)
			{
				break;
			}
		}
		else
		{

			if(SP_OK == eBandStateRlst)
			{
				eBandStateRlst = res;
			}
		}
	}
	return eBandStateRlst;
}

void CBTMeasSysV1::__LeaveAction( void )
{
}

SPRESULT CBTMeasSysV1::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }

    vector<CBTMeasBase*>::iterator itBandBt;
    for (itBandBt = m_pvecBTMeas.begin(); itBandBt != m_pvecBTMeas.end(); ++itBandBt)
    {
        delete *itBandBt;
        *itBandBt = NULL;
    }
    m_pvecBTMeas.clear();

    if (NULL != m_pLoadXml)
    {
        delete m_pLoadXml;
        m_pLoadXml = NULL;
    }

    if (NULL != m_pBtApi)
    {
        delete m_pBtApi;
        m_pBtApi = NULL;
    }

    return SP_OK;
}