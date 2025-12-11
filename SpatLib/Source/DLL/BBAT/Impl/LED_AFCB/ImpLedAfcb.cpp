#include "StdAfx.h"
#include "ImpLedAfcb.h"

//
IMPLEMENT_RUNTIME_CLASS(CImpLedAfcb)

CImpLedAfcb::~CImpLedAfcb(void)
{
}
CImpLedAfcb::CImpLedAfcb(void)
{
}

SPRESULT CImpLedAfcb::__PollAction(void)
{
    FUNBOX_INIT_CHECK();
    return LedAction();

}

BOOL CImpLedAfcb::LoadXMLConfig(void)
{
    m_vecSteps.clear();
    m_vecTestItems.clear();
    m_nGroupSize = GetConfigValue(L"Param:LED:GroupCount", 0);
    std::wstring sBase = L"Param:LED:";
    for (int i = 0; i < m_nGroupSize; i++)
    {
        TGroupLed tGroup;
        wchar_t    szGroup[20] = { 0 };
        swprintf_s(szGroup, L"Group%d:", i + 1);
        std::wstring sTemp = sBase + szGroup;
        std::wstring sPath = sTemp + L"PhoneLedType";
        tGroup.eFlashType = (FlashType)GetComboxValue(m_lpwFlashName, MAX_FLASH, sPath.c_str());
        if (tGroup.eFlashType == MAX_FLASH)
        {
            return FALSE;
        }
        sPath = sTemp + L"G3LedId";
        tGroup.eLedType = (LedType)GetConfigValue(sPath.c_str(), 0);
        sPath = sTemp + L"VoltageUpSpec";
        tGroup.dVoltageUp = GetConfigValue(sPath.c_str(), 0.0);
        sPath = sTemp + L"VoltageDownSpec";
        tGroup.dVoltageDown = GetConfigValue(sPath.c_str(), 0.0);
        sPath = sTemp + L"GPIONumber";
        tGroup.nGpioNo = GetConfigValue(sPath.c_str(), 0);
        m_vecTestItems.push_back(tGroup);
    }
    BuildSteps();

    return TRUE;
}

void CImpLedAfcb::BuildSteps()
{
    size_t nVecSize = m_vecTestItems.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CLedAfcb* Step = new CLedAfcb(m_vecTestItems[i], this);
        m_vecSteps.push_back(Step);
    }
}

SPRESULT CImpLedAfcb::LedAction()
{
    SPRESULT Res = SP_OK;
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        SPRESULT RunRes = m_vecSteps[i]->Run();
        if (RunRes != SP_OK)
        {
            Res = RunRes;
        }
    }
    return Res;
}

SPRESULT CImpLedAfcb::__FinalAction()
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