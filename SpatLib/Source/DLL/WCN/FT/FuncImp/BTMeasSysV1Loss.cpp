#include "StdAfx.h"
#include <cassert>
#include "BTApiAT.h"
#include "BTGeneralMeasBDR.h"
#include "BTGeneralMeasEDR.h"
#include "BTGeneralMeasBLE.h"
#include "BTMeasSysV1Loss.h"
#include "SimpleAop.h"
IMPLEMENT_RUNTIME_CLASS(CBTMeasSysV1Loss)

CBTMeasSysV1Loss::CBTMeasSysV1Loss(void)
: m_pLoadLossXml(NULL)
, m_pEnterMode(NULL)
, m_pBtApi(NULL)
{
}

CBTMeasSysV1Loss::~CBTMeasSysV1Loss(void)
{
}

SPRESULT CBTMeasSysV1Loss::__InitAction(void)
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

BOOL CBTMeasSysV1Loss::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());

	if (!m_VecBTParamBandImp.empty())
	{
		m_VecBTParamBandImp.clear();
	}
    try
    {
         m_pLoadLossXml = new CBTGeneralLoadLossXML(this);
    }
    catch (const std::bad_alloc& /*e*/)
    {
        assert(0);
        return SP_E_SPAT_ALLOC_MEMORY;

    }

    CHKRESULT(m_pLoadLossXml->BTLoadLossXmlFile(m_VecBTParamBandImp, m_BtLossParm));

	return TRUE;
}


SPRESULT CBTMeasSysV1Loss::__PollAction( void )
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
		CHKRESULT(eModeRslt);
		SPRESULT res = (*pitBTBase)->BTMeasLossPerform(m_BtLossParm);
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

void CBTMeasSysV1Loss::__LeaveAction( void )
{
}

SPRESULT CBTMeasSysV1Loss::__FinalAction(void)
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

    if (NULL != m_pLoadLossXml)
    {
        delete m_pLoadLossXml;
		m_pLoadLossXml = NULL;
    }

    if (NULL != m_pBtApi)
    {
        delete m_pBtApi;
        m_pBtApi = NULL;
    }

    return SP_OK;
}