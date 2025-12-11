#include "StdAfx.h"
#include "ImpAudioLoop.h"
IMPLEMENT_RUNTIME_CLASS(CImpAudioLoopBase)
CImpAudioLoopBase::CImpAudioLoopBase(void)
{
}
CImpAudioLoopBase::~CImpAudioLoopBase(void)
{
}

SPRESULT CImpAudioLoopBase::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return AudioLoopAction();
}

BOOL CImpAudioLoopBase::LoadXMLConfig(void)
{
    m_vecTestItems.clear();
    m_nGroupSize = GetConfigValue(L"Param:AudioLoop:GroupCount", 0);
    wstring sBase = L"Param:AudioLoop:";
    for (int i = 0; i < m_nGroupSize; i++)
    {
        TGroupAudioLoop tGroup;
        ZeroMemory(&tGroup, sizeof(tGroup));
        wchar_t    szGroup[20] = { 0 };
        swprintf_s(szGroup, L"Group%d:", i + 1);
        wstring sTemp = sBase + szGroup;
        wstring sPath = sTemp + L"Type";
        tGroup.eTestType = (alTestType)GetComboxValue(m_lpwTestType, 2, sPath.c_str());
        sPath = sTemp + L"Mic";
        tGroup.eMicId = (BASE_MIC_CHANNEL)GetComboxValue(m_lpwMicName, BASE_MAX_MIC, sPath.c_str());
        sPath = sTemp + L"Speaker";
        tGroup.eSpkId = (BASE_SPEAKER_CHANNEL)GetComboxValue(m_lpwSpkName, BASE_MAX_SPEAKER, sPath.c_str());
        sPath = sTemp + L"LoopSelect:LoopIn";
        tGroup.nLoopIn = GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"LoopSelect:LoopOut";
        tGroup.nLoopOut = GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"PA";
        tGroup.ePaId = (BBAT_PA_MIC_CHANNEL)GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"SNR";
        tGroup.dSnrSpec = GetConfigValue(sPath.c_str(), 0.0);
        sPath = sTemp + L"Peak";
        tGroup.dPeakSpec = GetConfigValue(sPath.c_str(), 0.0);
        sPath = sTemp + L"Mic_8K";
        tGroup.nMic8K = GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"Voltage:VoltagePDown";
        tGroup.dVoltageDownP = GetConfigValue(sPath.c_str(), 0.0);
        sPath = sTemp + L"Voltage:VoltageNUp";
        tGroup.dVoltageUpN = GetConfigValue(sPath.c_str(), 0.0);
        m_vecTestItems.push_back(tGroup);
    }
    BuildSteps();
    return TRUE;
}

void CImpAudioLoopBase::BuildSteps()
{
    size_t nVecSize = m_vecTestItems.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CAudioLoopBase* Step = nullptr;
        switch (m_vecTestItems[i].eTestType)
        {
        case PA_MIC:
        {
            Step = new CAudioLoopPa(m_vecTestItems[i], this);
            break;
        }
        case G3:
        {
            Step = new CAudioLoopAfcb(m_vecTestItems[i], this);
            break;
        }
        default:
        break;
        }
        if (Step != nullptr)
        {
            m_vecSteps.push_back(Step);
        }
    }
}

SPRESULT CImpAudioLoopBase::AudioLoopAction()
{
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CHKRESULT(m_vecSteps[i]->Run());
    }
    return SP_OK;
}

void CImpAudioLoopBase::__EnterAction()
{
    CImpBase::__EnterAction();
    m_pObjFunBox->JigMonitorEvent(FALSE);
    LogFmtStrA(SPLOGLV_INFO, "Jig monitor close.");
}

void CImpAudioLoopBase::__LeaveAction()
{
    m_pObjFunBox->JigMonitorEvent(TRUE);
    LogFmtStrA(SPLOGLV_INFO, "Jig monitor open.");
}

SPRESULT CImpAudioLoopBase::__FinalAction()
{
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        if (NULL != m_vecSteps[i])
        {
            delete m_vecSteps[i];
            m_vecSteps[i] = NULL;
        }
    }
    m_vecSteps.clear();
    return SP_OK;
}