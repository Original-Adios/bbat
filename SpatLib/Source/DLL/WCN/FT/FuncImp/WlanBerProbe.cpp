#include "StdAfx.h"
#include <cassert>
#include "WlanEnterMode.h"
#include "WlanAntSwitch.h"
#include "WlanRxSearchOS80.h"
#include "WlanBerProbe.h"
#include "SimpleAop.h"
//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_RUNTIME_CLASS(CWlanBerProbe)

CWlanBerProbe::CWlanBerProbe(void)
: m_pEnterMode(NULL)
, m_pWlanMeas(NULL)
, m_pWlanAnt(NULL)
{
}

CWlanBerProbe::~CWlanBerProbe(void)
{
}

SPRESULT CWlanBerProbe::__InitAction(void)
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
	m_pWlanMeas = new CWlanRxSearchOS80(this);
	if(NULL == m_pWlanMeas)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanRxSearchOS80 failed!" );
		return SP_E_SPAT_ALLOC_MEMORY;
	}
	return SP_OK;
}

BOOL CWlanBerProbe::LoadXMLConfig(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    /*CHKRESULT*/(__super::LoadXMLConfig());
	if (!m_stWlanParamImp.VecWlanParamBandImp.empty())
	{
		m_stWlanParamImp.VecWlanParamBandImp.clear();
	}
	CWlanProbeLoadXML* pLoadXml = NULL;
	pLoadXml = new CWlanProbeLoadXML(this);
	if(NULL == pLoadXml)
	{
		LogFmtStrA(SPLOGLV_ERROR, "new CWlanProbeLoadXML failed!" );
		return FALSE;
	}
	SPRESULT rlt = pLoadXml->WlanLoadXmlFile(m_stWlanParamImp);
	delete pLoadXml;
	return (SP_OK == rlt);
}

SPRESULT CWlanBerProbe::__PollAction( void )
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

	//write WlanBerProbe.csv
	ISpLog* iSplog = GetISpLogObject();
	WCHAR szlogPath[_MAX_PATH] = { 0 };
	iSplog->GetProperty(LogProp_LogFilePath, _MAX_PATH, szlogPath);
	LPWSTR lpChar = wcsrchr(szlogPath, L'\\');
	if (NULL != lpChar)
	{
		*lpChar = L'\0';
	}
	std::wstring strAbsPath = (std::wstring)szlogPath + L"\\WlanBerProbe.csv";
	m_pWlanMeas->m_hcsvHandle = CreateFileW(strAbsPath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_pWlanMeas->m_hcsvHandle)
	{
		_UiSendMsg("Create WlanBerProbe.csv", LEVEL_ITEM, SP_OK, SP_E_HANDLE, SP_OK, "", -1, "-");
		return SP_E_HANDLE;
	}

	char szFileHead[_MAX_PATH] = { 0 };
	sprintf_s(&szFileHead[0], _MAX_PATH, "%s", "Chain,Band,Mode,Rate,Cen_Channel,Per,Sensitivity");
	DWORD dwByteWritten = 0;
	BOOL bRet = WriteFile(m_pWlanMeas->m_hcsvHandle, &szFileHead[0], sizeof(szFileHead), &dwByteWritten, NULL);
	if (!bRet || sizeof(szFileHead) != dwByteWritten)
	{
		_UiSendMsg("Write WlanBerProbe.csv", LEVEL_ITEM, SP_OK, SP_E_FILE_IO, SP_OK, "", -1, "-");
		return SP_E_FILE_IO;
	}
	//

//	SPRESULT eRes = SP_OK;
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
		eRes = m_pWlanMeas->WlanMeasPerform();
		//Customer control RF Port
		m_pWlanMeas->m_Customize.ResetRfSwitch();

		if (m_bFailStop)
		{
			CHKRESULT(eRes);
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
	CHKRESULT(eRes);
	return eWlanBandRlst;
}

void CWlanBerProbe::__LeaveAction( void )
{
	if (m_pWlanMeas->m_hcsvHandle)
	{
		FlushFileBuffers(m_pWlanMeas->m_hcsvHandle);
		CloseHandle(m_pWlanMeas->m_hcsvHandle);
	}
}

SPRESULT CWlanBerProbe::__FinalAction(void)
{
	auto _function = SimpleAop(this, __FUNCTION__);
    if (NULL != m_pEnterMode)
    {
        delete m_pEnterMode;
        m_pEnterMode = NULL;
    }

	if (NULL != m_pWlanAnt)
	{
		delete m_pWlanAnt;
		m_pWlanAnt = NULL;
	}
	//
	if (NULL != m_pWlanMeas)
	{
		delete m_pWlanMeas;
		m_pWlanMeas = NULL;
	}

    return SP_OK;
}