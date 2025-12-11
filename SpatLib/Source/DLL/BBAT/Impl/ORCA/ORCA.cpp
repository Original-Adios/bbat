#include "StdAfx.h"
#include "ORCA.h"

//
IMPLEMENT_RUNTIME_CLASS(COrca)

COrca::COrca(void)
{
}
COrca::~COrca(void)
{

}

SPRESULT COrca::__PollAction(void)
{
    if (m_bRJ45)
    {
        CHKRESULT(RunOrca_RJ45());
    }
    if (m_bFT232)
    {
        CHKRESULT(RunOrca_FT232_UART());
    }
    if (m_bEXT)
    {
        CHKRESULT(RunOrca_EXTERNAL_IO());
    }
    if (m_bTypeC)
    {
        CHKRESULT(RunOrca_TypeC());
    }
    if (m_bADC)
    {
        CHKRESULT(RunOrca_ADC());
    }
    return SP_OK;
}

BOOL COrca::LoadXMLConfig(void)
{
    m_bRJ45 = GetConfigValue(L"Option:RJ45", 0);
    m_bFT232 = GetConfigValue(L"Option:FT232_UART", 0);
    m_bEXT = GetConfigValue(L"Option:EXTERNAL_IO", 0);
    m_bTypeC = GetConfigValue(L"Option:TYPEC_USB", 0);
    m_bADC = GetConfigValue(L"Option:ADC", 0);
    return TRUE;
}

SPRESULT COrca::RunOrca_RJ45()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_ORCA_RJ45(m_hDUT),
        "ORCA_RJ45", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "ORCA_RJ45", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT COrca::RunOrca_FT232_UART()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_ORCA_FT232_UART(m_hDUT),
        "ORCA_FT232_UART", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "ORCA_FT232_UART", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT COrca::RunOrca_EXTERNAL_IO()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_ORCA_EXTERNAL_IO(m_hDUT),
        "ORCA_EXTERNAL_IO", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "ORCA_EXTERNAL_IO", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT COrca::RunOrca_TypeC()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_ORCA_TYPEC_USB(m_hDUT),
        "ORCA_TYPEC_USB", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "ORCA_TYPEC_USB", LEVEL_ITEM);
    return SP_OK;
}

SPRESULT COrca::RunOrca_ADC()
{
    CHKRESULT(PrintErrorMsg(
        SP_BBAT_ORCA_ADC(m_hDUT),
        "ORCA_ADC", LEVEL_ITEM));
    PrintSuccessMsg(SP_OK, "ORCA_ADC", LEVEL_ITEM);
    return SP_OK;
}
