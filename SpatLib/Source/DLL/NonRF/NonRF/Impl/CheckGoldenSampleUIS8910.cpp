#include "StdAfx.h"
#include "CheckGoldenSampleUIS8910.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CCheckGoldenSampleUIS8910)

CCheckGoldenSampleUIS8910::CCheckGoldenSampleUIS8910()
{
    m_Uetype = 0;
    m_bGoldenPass = FALSE;
    m_UeBootUpWaitTime = 0;
    memset(m_szGSBuf,0,sizeof(m_szGSBuf));
}

CCheckGoldenSampleUIS8910::~CCheckGoldenSampleUIS8910()
{

}

BOOL CCheckGoldenSampleUIS8910::LoadXMLConfig(void)
{
    __super::LoadXMLConfig();
    m_Uetype = GetConfigValue(L"Option:DUTType:UEType", 0x8910);
    m_bGoldenPass = GetConfigValue(L"Option:GoldenPass", FALSE);
    m_UeBootUpWaitTime = GetConfigValue(L"Option:UeBootUpWaitTime", 5000);
    return TRUE;
}

SPRESULT CCheckGoldenSampleUIS8910::__PollAction()
{
    LogFmtStrA(SPLOGLV_INFO, "Reading golden sample information...");

    unsigned short usGoldSampleLen = 0;
    if (m_Uetype == 0x8850)
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8850;
    }
    else
    {
        usGoldSampleLen = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
    }

    Sleep(m_UeBootUpWaitTime);//wait for UE boot complete

    ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
    g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_READ;
    g_nvTransBuf.header.Position = 0;
    g_nvTransBuf.header.DataSize = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
    CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));
    memcpy(&m_szGSBuf[0], &g_nvTransBuf.nData[0], MAX_GOLDEN_SAMPLE_SIZE_UIS8910);

    if (usGoldSampleLen > MAX_GOLDEN_SAMPLE_SIZE_UIS8910)
    {
        ZeroMemory(&g_nvTransBuf, sizeof(PC_LTE_NV_UIS8910_DATA_T));
        g_nvTransBuf.header.eNvType = LTE_NV_UIS8910_TYPE_GOLD_BOARD_DATA_READ;
        g_nvTransBuf.header.Position = MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        g_nvTransBuf.header.DataSize = usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910;
        CHKRESULT(SP_lteLoadNV_UIS8910(m_hDUT, &g_nvTransBuf));
        memcpy(&m_szGSBuf[MAX_GOLDEN_SAMPLE_SIZE_UIS8910], &g_nvTransBuf.nData[0], usGoldSampleLen - MAX_GOLDEN_SAMPLE_SIZE_UIS8910);
    }

    int nStartAddr = 0;
    int nLen = sizeof(m_gs.common);
    CopyMemory(&m_gs.common, &m_szGSBuf[nStartAddr], nLen);
    
    if (m_bGoldenPass)
    {
        if (GS19 != m_gs.common.nMagicNum)
        {
            LogFmtStrA(SPLOGLV_ERROR, "This is not a valid golden sample.MagicNum:%d", m_gs.common.nMagicNum);
            NOTIFY("GoldenSample", LEVEL_ITEM, 1, 0, 1);
            return SP_E_SPAT_LOSS_LOAD_FROM_PHONE;
        }
        else
        {
            return SP_OK;
        }
    }
    else
    {
        if (GS19 == m_gs.common.nMagicNum)
        {
            LogFmtStrA(SPLOGLV_ERROR, "This is a golden sample.");
            MessageBox(
                NULL,
                _T("This is a golden sample, re-calibration will erase the golden parameters.\n当前的待测样机是金板，重新校准会导致金板参数擦除!"),
                _T("Golden Sample"),
                MB_OK | MB_TOPMOST | MB_ICONERROR
            );
            NOTIFY("GoldenSample", LEVEL_ITEM, 0, 1, 0);
            return SP_E_PHONE_GOLDEN_SAMPLE;
        }
        else
        {
            return SP_OK;
        }
    }
}