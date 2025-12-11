#include "StdAfx.h"
#include "SpatBase.h"
#include "dlmalloc.h"
#include "ActionApp.h"
#include "ModeSwitch.h"
#include <assert.h>
#include <atlconv.h>
#include "DebuggingTips.h"
#include <sstream>

#pragma comment(lib, "PhoneCommand.lib")
#pragma comment(lib, "SpatEngine.lib")

extern CActionApp myApp;
extern LPCSTR g_szModule;
//////////////////////////////////////////////////////////////////////////
/// CSpatBase Runtime declaration

static wchar_t g_lpszCSpatBase[] = TOWSTR(CSpatBase);
CSpatRuntimeClass CSpatBase::classCSpatBase((LPCWSTR)g_lpszCSpatBase, NULL, NULL);

static SPAT_INIT_RUNTIME_CLASS g_init_CSpatBase(&CSpatBase::classCSpatBase);
//////////////////////////////////////////////////////////////////////////
///
CSpatBase::CSpatBase(void)
    : m_lpSeqParse(NULL)
    , m_lpContainer(NULL)
    , m_hEngine(INVALID_NPI_HANDLE)
    , m_hDUT(INVALID_NPI_HANDLE)
    , m_pRFTester(NULL)
    , m_pDCSource(NULL)
    , m_nLogLevel(SPLOGLV_ERROR)
    , m_pModeSwitch(NULL)
    , m_eDutRunMode(RM_INVALID_MODE)
    , m_bUnShowFailItem(FALSE)
    , m_strRepairBand("00")
    , m_eRepairMode(RepairMode_System)
    , m_strRepairItem($REPAIR_ITEM_UNKNOWN)
    , m_bFailStop(TRUE)
    , m_bSaveFinalTestFlag(TRUE)
    , m_u32MaxFailRetryCount(3)
    , m_bAutoStart(FALSE)
    , m_u32PreserveSpot(0)
    , m_pCallback(NULL)
    , m_bUiOnlyListItem(FALSE)
    , m_bClassCalFlag(FALSE)
{

}

CSpatBase::~CSpatBase(void)
{
}

void CSpatBase::InitLog(LPVOID pLogUtil, UINT nLv)
{
    m_nLogLevel = nLv;
    trInit((ISpLog*)pLogUtil, g_szModule, m_nLogLevel);
}

void CSpatBase::Release(void)
{
    trFree();
    delete this;
}

SPRESULT CSpatBase::SetDriver(INT nDriv, LPVOID pDriv, LPVOID /*pParam*/)
{
    SPRESULT res = SP_OK;
    switch (nDriv)
    {
    case SPATDRV_RFTESTER:
        m_pRFTester = (IRFDevice*)pDriv;
        break;
    case SPATDRV_DCSOURCE:
        m_pDCSource = (IDCS*)pDriv;
        break;
    case SPATDRV_PHONECMD:
        m_hDUT = (SP_HANDLE)pDriv;
        if (NULL == m_hDUT)
        {
            res = SP_E_SPAT_INVALID_PARAMETER;
            LogRawStrA(SPLOGLV_ERROR, "Invalid PhoneCommand handle! (NULL)");
            assert(0);
        }
        break;
    case SPATDRV_ENGINE:
        m_hEngine = (SP_HANDLE)pDriv;
        break;
    case SPATDRV_CONTAINER:
        m_lpContainer = (IContainer*)pDriv;
        if (NULL == m_lpContainer)
        {
            res = SP_E_SPAT_INVALID_PARAMETER;
            LogRawStrA(SPLOGLV_ERROR, "Invalid Container Object! (NULL)");
            assert(0);
        }
        break;
    case SPATDRV_SEQPARSE:
        m_lpSeqParse = (ISeqParse*)pDriv;
        if (NULL == m_lpSeqParse)
        {
            res = SP_E_SPAT_INVALID_PARAMETER;
            LogRawStrA(SPLOGLV_ERROR, "Invalid SeqParse Object! (NULL)");
            assert(0);
            return SP_E_SPAT_INVALID_PARAMETER;
        }
        break;
    case SPATDRV_DB:
        break;
    case SPATDRV_CALLBACK:
        m_pCallback = ( ICallback* )pDriv;
        if ( NULL == m_pCallback)
        {
            res = SP_E_SPAT_INVALID_PARAMETER;
            LogRawStrA( SPLOGLV_ERROR, "Invalid Callback Object! (NULL)" );
            assert( 0 );
        }
        break;
    case SPATDRV_SYSPREPTR:
        m_pSystemPrePtr = (CSpatBase*)pDriv;
        if (NULL == m_pSystemPrePtr)
        {
            res = SP_E_SPAT_INVALID_PARAMETER;
            LogRawStrA(SPLOGLV_ERROR, "Invalid System PrePtr Object! (NULL)");
            assert(0);
        }
        break;
    default:
        res = SP_E_SPAT_NOT_SUPPORT;
        LogFmtStrA(SPLOGLV_ERROR, "Unknown Driver %d!", nDriv);
        assert(0);
        break;
    }

    return SP_OK;
}

SPRESULT CSpatBase::SetProperty(INT nFlags, INT /*nOption*/, LPCVOID lpValue)
{
    if (NULL == lpValue)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Invalid parameters!", __FUNCTION__);
        assert(0);
        return SP_E_SPAT_INVALID_POINTER;
    }

    switch (nFlags)
    {
    case PropertyInternal_001:
        m_bUnShowFailItem = (BOOL)(*(UINT32*)lpValue);
        break;

    case PropertyInternal_002:
        m_bClassCalFlag = (BOOL)(*(UINT32*)lpValue);
        break;

    case Property_RepairCode:
        break;

    case Property_LogLevel:
        break;

    default:
        break;
    }

    return SP_OK;
}

SPRESULT CSpatBase::GetProperty(INT nFlags, INT nOption, LPVOID lpValue)
{
    switch (nFlags)
    {
    case PropertyInternal_001:
        *((UINT32*)lpValue) = (UINT32)m_bUnShowFailItem;
        break;

    case PropertyInternal_002:
        *((UINT32*)lpValue) = (UINT32)(m_bUiOnlyListItem && m_bClassCalFlag);
        break;

    case Property_RepairCode:
    {
        const LPCSTR arrMode[Max_RepairMode] =
        {
            $REPAIR_MODE_SYSTEM,
            $REPAIR_MODE_COMMON,
            $REPAIR_MODE_GSM,
            $REPAIR_MODE_EDGE,
            $REPAIR_MODE_TD,
            $REPAIR_MODE_WCDMA,
            $REPAIR_MODE_LTE,
            $REPAIR_MODE_NR,
            $REPAIR_MODE_Bluetooth,
            $REPAIR_MODE_Wlan,
            $REPAIR_MODE_GPS,
            $REPAIR_MODE_NB
        };

        if (IsBadWritePtr(lpValue, 6) || nOption < 6)
        {
            return SP_E_SPAT_INVALID_PARAMETER;
        }

        memcpy(lpValue, arrMode[m_eRepairMode], 1);
        memcpy(((char*)lpValue) + 1, m_strRepairBand.c_str(), 3);
        memcpy(((char*)lpValue) + 4, m_strRepairItem.c_str(), 2);
    }
    break;

    case Property_LogLevel:
        break;

    default:
        break;
    }

    return SP_OK;
}

void CSpatBase::SendProgress(uint32 uPercent)
{
    m_pCallback->UiMsgTestProgress(m_strActionName.c_str(), uPercent);
}

void CSpatBase::SendWorkStatus()
{
    BOOL bFirst = TRUE;
    GetShareMemory(ShareMemory_AutoStart, &bFirst, 4);

    if (bFirst)
    {
        bFirst = FALSE;
        SetShareMemory(ShareMemory_AutoStart, &bFirst, 4);
        m_pCallback->UiMsgWorkStatus(m_strActionName.c_str());
    }
}

void CSpatBase::SendCommonCallback( LPCWSTR lpMessage, ... )
{
    WCHAR szCond[256] = { 0 };
    va_list  args;
    va_start( args, lpMessage );
    int nSize = _vsnwprintf_s( &szCond[0], _countof( szCond ), _TRUNCATE, lpMessage, args );
    va_end( args );

    m_pCallback->UiMsgCommon( m_strActionName.c_str(), ( nSize > 0 ) ? &szCond[0] : NULL );
}

SPRESULT CSpatBase::InitAction(LPCSPAT_INIT_PARAM pParam)
{
    USES_CONVERSION;

    string strException = "";

    SPRESULT res = SP_OK;
    try
    {
        if (NULL == pParam || NULL == pParam->lpActionName)
        {
            LogRawStrA(SPLOGLV_ERROR, "Invalid parameter or Action name!");
            assert(0);
            return SP_E_SPAT_INVALID_PARAMETER;
        }
        else
        {
            m_AdjParam = *pParam;
            m_strActionName = pParam->lpActionName;
            m_AdjParam.lpActionName = m_strActionName.c_str();
        }

        /// Driver
        CHKRESULT(SetDriver(SPATDRV_RFTESTER, (LPVOID)pParam->pRF, NULL));
        CHKRESULT(SetDriver(SPATDRV_DCSOURCE, (LPVOID)pParam->pDC, NULL));
        CHKRESULT(SetDriver(SPATDRV_PHONECMD, (LPVOID)pParam->hDUT, NULL));
        CHKRESULT(SetDriver(SPATDRV_ENGINE, (LPVOID)pParam->hEngine, NULL));
        CHKRESULT(SetDriver(SPATDRV_CONTAINER, (LPVOID)pParam->pContainer, NULL));
        CHKRESULT(SetDriver(SPATDRV_SEQPARSE, (LPVOID)pParam->pSeqParse, NULL));
        CHKRESULT(SetDriver(SPATDRV_DB, (LPVOID)pParam->pDB, NULL));
        CHKRESULT(SetDriver(SPATDRV_CALLBACK, (LPVOID)pParam->pCallback, NULL));
        CHKRESULT(SetDriver(SPATDRV_SYSPREPTR, (LPVOID)pParam->pSystemPrePtr, NULL));

        //
        if (NULL == m_pModeSwitch)
        {
            // InitAction will be re-invoked by Framework, so add protection code to avoid memory leak
            m_pModeSwitch = new CModeSwitch(this);
        }

        if (!LoadXMLConfig())
        {
            SendCommonCallback(m_strXMLPath.c_str());

            LogFmtStrW(SPLOGLV_ERROR, L"Invalid XML configuration! Node: %s <%s>", pParam->lpActionName, m_strXMLPath.c_str());
            return SP_E_SPAT_LOAD_XML;
        }

        res = __InitAction();

        if (IS_VALID_MODE(m_eDutRunMode))
        {
            m_pModeSwitch->SetupRunMode(m_eDutRunMode);
        }
    }
    catch (CXExceptionA* e)
    {
        res = SP_E_SPAT_USER_EXCEPTION;
        strException = e->what();
        LogFmtStrA(SPLOGLV_ERROR, "%s", e->what());
        delete e;
    }
    catch (const std::bad_alloc& e)
    {
        res = SP_E_SPAT_ALLOC_MEMORY;
        strException = e.what();
        LogFmtStrA(SPLOGLV_ERROR, "%s", e.what());
    }
    catch (const std::exception& e)
    {
        res = SP_E_SPAT_SYSTEM_EXCEPTION;
        strException = e.what();
        LogFmtStrA(SPLOGLV_ERROR, "%s", e.what());
    }
    catch (...)
    {
        SendCommonCallback(L"Unknown Exception!");
        LogRawStrA(SPLOGLV_ERROR, "Unknown Exception!");
        return SP_E_SPAT_SYSTEM_EXCEPTION;
    }

    if (strException != "")
    {
        SendCommonCallback(L"%s", _A2CW(strException.c_str()));
    }

    return res;
}

void CSpatBase::EnterAction(void)
{
    try
    {
        __EnterAction();
    }
    catch (CXExceptionA* e)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s", e->what());
        delete e;
    }
    catch (...)
    {
        LogRawStrA(SPLOGLV_ERROR, "Unknown Exception!");
    }
}

SPRESULT CSpatBase::PollAction(void)
{
    if (_IsUserStop())
    {
        return SP_E_USER_ABORT;
    }

    SPRESULT res = SP_OK;
    try
    {
        LoadGlobalSettings();

        // Change the DUT mode at first
        SetRepair(RepairMode_System, NULL, $REPAIR_ITEM_UNKNOWN);
        //res = ChangeDUTRunMode();
        if (SP_OK != res)
        {
            return res;
        }

        res = __PollAction();

        if (SP_OK != res && (PSPOT_LEVLE_ACTION & m_u32PreserveSpot))
        {
            CDebuggingTips cDebugTips;
            cDebugTips.Messag(CDebuggingTips::Chinese, "[ ACTION ] %s", _W2CA(m_AdjParam.lpActionName));
        }
    }
    catch (CXExceptionA* e)
    {
        res = SP_E_SPAT_USER_EXCEPTION;
        LogFmtStrA(SPLOGLV_ERROR, "%s", e->what());
        delete e;
    }
    catch (const std::bad_alloc& e)
    {
        res = SP_E_SPAT_ALLOC_MEMORY;
        LogFmtStrA(SPLOGLV_ERROR, "%s", e.what());
    }
    catch (const std::exception& e)
    {
        res = SP_E_SPAT_SYSTEM_EXCEPTION;
        LogFmtStrA(SPLOGLV_ERROR, "%s", e.what());
    }
    catch (...)
    {
        res = SP_E_SPAT_SYSTEM_EXCEPTION;
        LogRawStrA(SPLOGLV_ERROR, "Unknown Exception!");
    }

    return res;
}

void CSpatBase::LeaveAction(void)
{
    try
    {
        __LeaveAction();
    }
    catch (CXExceptionA* e)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s", e->what());
        delete e;
    }
    catch (...)
    {
        LogRawStrA(SPLOGLV_ERROR, "Unknown Exception!");
    }
}

SPRESULT CSpatBase::FinalAction(void)
{
    SPRESULT res = SP_OK;
    try
    {
        res = __FinalAction();

        if (NULL != m_pModeSwitch)
        {
            m_pModeSwitch->Release();
            m_pModeSwitch = NULL;
        }
    }
    catch (CXExceptionA* e)
    {
        res = SP_E_SPAT_USER_EXCEPTION;
        LogFmtStrA(SPLOGLV_ERROR, "%s", e->what());
        delete e;
    }
    catch (const std::exception& e)
    {
        res = SP_E_SPAT_SYSTEM_EXCEPTION;
        LogFmtStrA(SPLOGLV_ERROR, "%s", e.what());
    }
    catch (...)
    {
        res = SP_E_SPAT_SYSTEM_EXCEPTION;
        LogRawStrA(SPLOGLV_ERROR, "Unknown Exception!");
    }

    return res;
}

SPRESULT CSpatBase::PrintErrorMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level)
{
    if (Result != SP_OK)
    {
        _UiSendMsg(Msg,
            Level,
            1,
            0,
            1,
            nullptr,
            -1,
            "",
            "");
    }

    return Result;
}

SPRESULT CSpatBase::PrintSuccessMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level)
{
    if (Result == SP_OK)
    {
        _UiSendMsg(Msg,
            Level,
            1,
            1,
            1,
            nullptr,
            -1,
            "",
            "");
    }

    return Result;
}

void CSpatBase::__LogItem(const CALLBACKDATA_TEST_ITEM& item)
{
    if (IS_BIT_SET(item.nLevel, LEVEL_UI) || IS_BIT_SET(item.nLevel, LEVEL_REPORT))
    {
        CHAR szValue[64] = { 0 };
        m_strLog = item.lpName;
        if (NULL != item.lpBand)
        {
            m_strLog = m_strLog + " " + item.lpBand + "; ";
        }
        else
        {
            m_strLog += " "; // 隔开Condition
        }

        if (item.nChannel >= 0)
        {
            sprintf_s(szValue, "Channel = %d; ", item.nChannel);
            m_strLog += szValue;
        }

        if (NULL != item.lpCond)
        {
            m_strLog += item.lpCond;
        }

        CHAR szLow[24] = { "-" };
        CHAR szUpp[24] = { "-" };
        if (!IS_EQUAL(item.Value.low, NOLOWLMT))
        {
            sprintf_s(szLow, "%.2f", item.Value.low);
        }
        if (!IS_EQUAL(item.Value.upp, NOUPPLMT))
        {
            sprintf_s(szUpp, "%.2f", item.Value.upp);
        }
        sprintf_s(szValue, " [%s, %.2f, %s] ", szLow, item.Value.val, szUpp);

        m_strLog += szValue;

        if ((NULL != item.lpUnit) && (0 != _stricmp(item.lpUnit, "-")))
        {
            m_strLog += item.lpUnit;
        }

        m_strLog += ((PASS == item.result) ? " pass" : " fail");
        LogRawStrA((PASS == item.result) ? SPLOGLV_INFO : SPLOGLV_ERROR, m_strLog.c_str());
    }
}

BOOL CSpatBase::_UiSendMsg(
    const char* lpszName,
    uint32 nLv,
    double dLow, double dVal, double dUpp,
    const char* lpszBand, int32 nChannel,
    const char* lpszUnit,
    const char* lpsczCond, ...
)
{
    if (NULL == lpszName)
    {
        assert(0);
        return FALSE;
    }

    CALLBACKDATA_TEST_ITEM item;
    item.lpName = lpszName;
    item.nLevel = nLv;
    if (ISLEVELNONE(nLv))
    {
        item.nLevel |= LEVEL_DEBUG;
    }

    item.Value.low = dLow;
    item.Value.val = dVal;
    item.Value.upp = dUpp;
    item.lpBand = lpszBand;
    item.nChannel = nChannel;
    item.lpUnit = (NULL != lpszUnit) ? lpszUnit : "-";
    item.result = (UINT8)(item.Value.operator bool() ? PASS : FAIL);
    item.lpActionName = m_AdjParam.lpActionName;

    CHAR szCond[256] = { 0 };
    std::string strGlobalConditionTemp = "";
    if (NULL != lpsczCond)
    {
        va_list  args;
        va_start(args, lpsczCond);
        int nSize = _vsnprintf_s(&szCond[0], _countof(szCond), _TRUNCATE, lpsczCond, args);
        va_end(args);

        if (m_strGlobalCondition != "")
        {
            strGlobalConditionTemp = szCond + m_strGlobalCondition;
            item.lpCond = strGlobalConditionTemp.c_str();
        }
        else
        {
            item.lpCond = (nSize > 0) ? &szCond[0] : NULL;
        }
    }
    else
    {
        item.lpCond = NULL;
    }

    __LogItem(item);

    if (m_bUnShowFailItem && FAIL == item.result)
    {
        // If fail happens during repeat, ignore to show in UI
        return TRUE;
    }

    IItemDataObserver* pDataObserver = NULL;
    if ((SP_OK == GetShareMemory(ShareMemoryItemDataObserver, (void*)&pDataObserver, sizeof(pDataObserver), NULL, FALSE))
        && (NULL != pDataObserver)
        )
    {
        IItemDataObserver::ITEMDATA_T obsdata;
        strncpy_s(obsdata.szActionName, _W2CA(item.lpActionName), CopySize(obsdata.szActionName));
        strncpy_s(obsdata.szItemName, item.lpName, CopySize(obsdata.szItemName));
        strncpy_s(obsdata.szUnit, item.lpUnit, CopySize(obsdata.szUnit));
        if (NULL != item.lpBand)
        {
            strncpy_s(obsdata.szBand, item.lpBand, CopySize(obsdata.szBand));
        }
        if (NULL != item.lpCond)
        {
            strncpy_s(obsdata.szCond, item.lpCond, CopySize(obsdata.szCond));
        }
        obsdata.nChannel = item.nChannel;
        obsdata.nLv = item.nLevel;
        obsdata.dLower = item.Value.low;
        obsdata.dValue = item.Value.val;
        obsdata.dUpper = item.Value.upp;
        pDataObserver->PushData(obsdata);
    }

    BOOL bOK = TRUE;
    if (m_bUiOnlyListItem && LEVEL_CAL == (item.nLevel & LEVEL_CAL))
    {

    }
    else
    {
        bOK = m_pCallback->UiMsgTestItem(&item);
    }

    if (FAIL == item.result && (PSPOT_LEVLE_ITEM & m_u32PreserveSpot))
    {
        CDebuggingTips cDebugTips;
        cDebugTips.Messag(CDebuggingTips::Chinese, " [ ACTION ] %s -> [ ITEM ] %s ", _W2CA(item.lpActionName), lpszName);
    }

    return bOK;
}

BOOL CSpatBase::_UiSendMsg(
    const char* lpszName,
    uint32 nLv,
    double dLow /*= 0.0*/, double dVal /*= 0.0*/, double dUpp /*= 0.0*/,
    const char* lpszBand /*= NULL*/, int32 nChannel /*= -1*/,
    const char* lpszUnit /*= NULL*/
)
{
    return _UiSendMsg(lpszName, nLv, dLow, dVal, dUpp, lpszBand, nChannel, lpszUnit, NULL);
}

BOOL CSpatBase::_UiSendMsg(const char* /*lpsznvPath*/, int32 /*data*/)
{
    /* Bug 929677
    // Xiaoping: 减少Simba Log存储空间，不再将NV保存到Backup.nv
    // 这个功能通过SpatLib\System.dll  CfixnvDump替代
    // Bug 815985 - [需求]校准完成后把nv分区中的完整bin load出来，并通过SN来标识备�?
    return _UiSendMsg(lpsznvPath, LEVEL_NV, 0.0, (double)data, 0.0);
    */
    return TRUE;
}

BOOL CSpatBase::_UiSendMsg( const char* lpName, const char* lpInfo )
{
    /// Temporarily modification
    std::string strValue = lpInfo;
    for ( size_t i = 0; i < strValue.length(); i++ )
    {
        strValue[i] = ( lpInfo[i] < 0 || 0 == isprint( lpInfo[i] ) ) ? ' ' : lpInfo[i];
    }
    LogFmtStrA( SPLOGLV_INFO, "%s: %s", lpName, strValue.c_str() );
    return m_pCallback->UiMsgDutInfo( lpName, strValue.c_str() );
}

BOOL CSpatBase::InvokeCustomizeDataCallback( LPCSTR lpName, LPCVOID lpData )
{
    return m_pCallback->UiMsgCustomizedData( lpName, lpData );
}

BOOL CSpatBase::_IsUserStop(void)
{
    HANDLE hEvent = m_AdjParam.hStopEvent;
    if (NULL != hEvent)
    {
        if (WAIT_OBJECT_0 == ::WaitForSingleObject(hEvent, 0)) /// Here cannot be 1, it will block efficiency while loop testing
        {
            return TRUE;
        }
    }

    return FALSE;
}

int CSpatBase::GetConfigValue(LPCWSTR lpPath, int defValue, BOOL bGlobalSettings /* = FALSE */)
{
    if (NULL == m_lpSeqParse)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid SeqParse Object (NULL)!");
        assert(0);
        return defValue;
    }
    else
    {
        m_strXMLPath = lpPath;
        return m_lpSeqParse->GetValue(bGlobalSettings ? SEQ_GLOBAL_NODE_NAME : m_AdjParam.lpActionName, lpPath, defValue);
    }
}

double CSpatBase::GetConfigValue(LPCWSTR lpPath, double defValue, BOOL bGlobalSettings /* = FALSE */)
{
    if (NULL == m_lpSeqParse)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid SeqParse Object (NULL)!");
        assert(0);
        return defValue;
    }
    else
    {
        m_strXMLPath = lpPath;
        return m_lpSeqParse->GetValue(bGlobalSettings ? SEQ_GLOBAL_NODE_NAME : m_AdjParam.lpActionName, lpPath, defValue);
    }
}

LPCWSTR CSpatBase::GetConfigValue(LPCWSTR lpPath, LPCWSTR defString, BOOL bGlobalSettings /* = FALSE */)
{
    if (NULL == m_lpSeqParse)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid SeqParse Object (NULL)!");
        assert(0);
        return defString;
    }
    else
    {
        m_strXMLPath = lpPath;
        return m_lpSeqParse->GetValue(bGlobalSettings ? SEQ_GLOBAL_NODE_NAME : m_AdjParam.lpActionName, lpPath, defString);
    }
}

SPRESULT CSpatBase::SetShareMemory(LPCWSTR lpszName, const void* lpValue, UINT32 u32Bytes, UINT32 nOption/* = IContainer::Normal */)
{
    LogFmtStrW(SPLOGLV_VERBOSE, L"Set Share Memory %s: %d", lpszName, u32Bytes);

    if (NULL == m_lpContainer)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid Container Object (NULL)!");
        assert(0);
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return m_lpContainer->SetValue(lpszName, lpValue, u32Bytes, nOption);
    }
}

SPRESULT CSpatBase::GetShareMemory(LPCWSTR lpszName, void* lpValue, UINT32 u32Bytes, UINT32* pOption/* = NULL */, BOOL bLog/* = TRUE*/)
{
    if (bLog)
    {
        LogFmtStrW(SPLOGLV_VERBOSE, L"Get Share Memory %s: %d bLog = %d", lpszName, u32Bytes, bLog);
    }
    if (NULL == m_lpContainer)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid Container Object (NULL)!");
        assert(0);
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        SPRESULT res = m_lpContainer->GetValue(lpszName, lpValue, u32Bytes, pOption);
        if (res == SP_OK)
        {
            try
            {
                stringstream ss;
                for (size_t i = (u32Bytes - 1); ; i--)
                {
                    ss << std::hex << (int)*((char*)lpValue + i) << " ";

                    if (i == 0)
                    {
                        if (bLog)
                        {
                            LogFmtStrA(SPLOGLV_INFO, "%s", ss.str().c_str());
                        }
                        break;
                    }
                }
            }
            catch (const std::ios_base::failure& e)
            {
                UNREFERENCED_PARAMETER(e);
                return SP_E_SPAT_SYSTEM_EXCEPTION;
            }
        }
        return res;
    }
}

SPRESULT CSpatBase::EnterEngine(BOOL bLock, DWORD dwTimeOut/*= INFINITE*/)
{
    if (INVALID_NPI_HANDLE == m_hEngine)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid Engine Handle (NULL)!");
        assert(0);
        return SP_E_SPAT_INVALID_HANDLE;
    }

    if (bLock)
    {
        return ENG_Enter(m_hEngine, m_AdjParam.lpActionName, dwTimeOut);
    }
    else
    {
        return ENG_EnterNoLock(m_hEngine, m_AdjParam.lpActionName, dwTimeOut);
    }
}

SPRESULT CSpatBase::LeaveEngine(BOOL bLock)
{
    if (INVALID_NPI_HANDLE == m_hEngine)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid Engine Handle (NULL)!");
        assert(0);
        return SP_E_SPAT_INVALID_HANDLE;
    }

    if (bLock)
    {
        return ENG_Leave(m_hEngine, m_AdjParam.lpActionName);
    }
    else
    {
        return ENG_LeaveNoLock(m_hEngine, m_AdjParam.lpActionName);
    }
}

SPRESULT CSpatBase::FinalEngine(BOOL bLock, DWORD dwTimeOut/*= INFINITE*/)
{
    if (INVALID_NPI_HANDLE == m_hEngine)
    {
        LogRawStrA(SPLOGLV_ERROR, "Invalid Engine Handle (NULL)!");
        assert(0);
        return SP_E_SPAT_INVALID_HANDLE;
    }

    if (bLock)
    {
        return ENG_Final(m_hEngine, m_AdjParam.lpActionName, dwTimeOut);
    }
    else
    {
        return ENG_FinalNoLock(m_hEngine, m_AdjParam.lpActionName, dwTimeOut);
    }
}

BOOL CSpatBase::LoadXMLConfig(void)
{
    /// TODO:
    /*
       <Node Type="Action" Name="GSM AGC" Description="GSM COMMON AGC" Checked="1">
           <Option>
               <BAND>
                    <A Control="CheckBox">1</A>
                    <F Control="CheckBox">1</F>
               </BAND>
           </Option>
            <Param>
                <DLChannel>
                    <Channel1>10700</Channel1>
                    <Channel2>10562</Channel2>
                </DLChannel>
                <CellPower>-70.0,-80.0,-90.0</CellPower>
                <GainTable Property="ReadOnly">0xFA00,0xFA00,0xFA00,0xFA00</GainTable>
                <GainValue Property="InVisible">0xE500,0xE500,0xE500,0xE500</GainValue>
            </Param>
        </Node>

        For Example:
        --------------------
        BOOL bBandA = GetConfigValue(L"Option:BAND:A", 0);
        BOOL bBandB = GetConfigValue(L"Option:BAND:B", 0);

        long CH1 = GetConfigValue(L"Param:DLChannel:Channel1", 10800);
        assert(10700 == CH1);

        std::string sValue = GetConfigValue(L"Param:CellPower", L"-50.0,-50.0,-50.0");
        assert(L"-70.0,-80.0,-90.0" == sValue);
    */

    return TRUE;
}

void CSpatBase::SetupDUTRunMode(RM_MODE_ENUM eMode, BS_CONFIG_T* pBS /* = NULL */)
{
    m_eDutRunMode = eMode;
    if (pBS != NULL)
    {
        memcpy(&m_CurrBS, pBS, sizeof(BS_CONFIG_T));
    }
    //m_pCurrBS = pBS;
    if (NULL != pBS && NULL != m_pModeSwitch)
    {
        m_pModeSwitch->SetupBSConfig(*pBS);
    }
}

SPRESULT CSpatBase::ChangeDUTRunMode(void)
{
    if (NULL == m_pModeSwitch)
    {
        LogFmtStrA(SPLOGLV_ERROR, "%s: Invalid pointer!", __FUNCTION__);
        assert(0);
        return SP_E_SPAT_INVALID_POINTER;
    }

    if (RM_INVALID_MODE != m_eDutRunMode)
    {
        return m_pModeSwitch->Change(m_eDutRunMode, &m_CurrBS);
    }
    else
    {
        return SP_OK;
    }

}

void CSpatBase::SetRepair(RepairMode eRepairMode, LPCSTR pBand, LPCSTR pItem)
{
    SetRepairMode(eRepairMode);
    SetRepairBand(pBand);
    SetRepairItem(pItem);
}

void CSpatBase::SetRepairMode(RepairMode eRepairMode)
{
#ifdef _DEBUG
    assert(eRepairMode >= 0 && eRepairMode < Max_RepairMode);
#endif

    m_eRepairMode = eRepairMode;
}

void CSpatBase::SetRepairBand(LPCSTR pBand)
{
#ifdef _DEBUG
    assert(pBand == NULL || strlen(pBand) == 2 || strlen(pBand) == 3);
#endif

    if (pBand == NULL)
    {
        m_strRepairBand = "00X";
    }
    else if(strlen(pBand) == 2)
    {
        m_strRepairBand = pBand;
        m_strRepairBand.push_back('X');
    }
    else
    {
        m_strRepairBand = pBand;
    }
}

void CSpatBase::SetRepairItem(LPCSTR pItem)
{
#ifdef _DEBUG
    assert(pItem != NULL && strlen(pItem) == 2);
#endif
    if (pItem != NULL)
    {
        m_strRepairItem = pItem;
    }
}

RepairMode CSpatBase::GetRepaireMode()
{
    return m_eRepairMode;
}

LPCSTR CSpatBase::GetRepairBand(void)
{
    return m_strRepairBand.c_str();
}

LPCSTR CSpatBase::GetRepairItem()
{
    return m_strRepairItem.c_str();
}

BOOL CSpatBase::IsFakeDC(void)
{
    if (m_pDCSource != NULL)
    {
        GPIB_PARAM io;
        m_pDCSource->GetGpib()->GetProperty(GPIB_IO_PARAM, 0, (LPVOID)&io);
        return (io.eGpibType == GPIB_DUMMY) ? TRUE : FALSE;
    }

    return TRUE;
}

BOOL CSpatBase::IsFakeRFTester(void)
{
    if (m_pRFTester != NULL)
    {
        GPIB_PARAM io;
        m_pRFTester->GetGpib()->GetProperty(GPIB_IO_PARAM, 0, (LPVOID)&io);
        return (io.eGpibType == GPIB_DUMMY) ? TRUE : FALSE;
    }

    return TRUE;
}

void CSpatBase::LoadGlobalSettings(void)
{
    UINT32 u32Value = 0;
    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_FailStop, (void*)&u32Value, 4))
    {
        m_bFailStop = (BOOL)u32Value;
    }

    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_SaveFinalTestFlag, (void*)&u32Value, 4))
    {
        m_bSaveFinalTestFlag = (BOOL)u32Value;
    }

    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_MaxRetryCount, (void*)&u32Value, 4))
    {
        m_u32MaxFailRetryCount = u32Value;
    }

    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_AutoStart, (void*)&u32Value, 4))
    {
        m_bAutoStart = u32Value;
    }

    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_PreserveSpot, (void*)&u32Value, 4))
    {
        m_u32PreserveSpot = u32Value;
    }

    if (SP_OK == m_lpContainer->GetValue(InternalReservedShareMemory_UiOnlyListItem, (void*)&u32Value, 4))
    {
        m_bUiOnlyListItem = u32Value;
    }
}

BOOL CSpatBase::_UiSendMsgPass(const char* lpszName, uint32 nLv, const char* lpszBand, int32 nChannel, const char* lpszUnit, const char* lpsczCond, ...)
{
    CHAR szCond[256] = { 0 };
    LPCSTR lpCond = NULL;
    if (NULL != lpsczCond)
    {
        va_list  args;
        va_start(args, lpsczCond);
        int nSize = _vsnprintf_s(&szCond[0], _countof(szCond), _TRUNCATE, lpsczCond, args);
        va_end(args);
        lpCond = (nSize > 0) ? &szCond[0] : NULL;
    }

    return this->_UiSendMsg(lpszName, nLv, 1, 1, 1, lpszBand, nChannel, lpszUnit, lpCond);
}

BOOL CSpatBase::_UiSendMsgFail(const char* lpszName, uint32 nLv, const char* lpszBand, int32 nChannel, const char* lpszUnit, const char* lpsczCond, ...)
{
    CHAR szCond[256] = { 0 };
    LPCSTR lpCond = NULL;
    if (NULL != lpsczCond)
    {
        va_list  args;
        va_start(args, lpsczCond);
        int nSize = _vsnprintf_s(&szCond[0], _countof(szCond), _TRUNCATE, lpsczCond, args);
        va_end(args);
        lpCond = (nSize > 0) ? &szCond[0] : NULL;
    }

    return this->_UiSendMsg(lpszName, nLv, 1, 0, 1, lpszBand, nChannel, lpszUnit, lpCond);
}

SPRESULT CSpatBase::SwitchOnDC(BOOL bOn, float fVBAT, float fVBUS)
{
    if (IsFakeDC())
    {
        return SP_OK;
    }

    // Separated PowerSupply for VBUS
    IDCS* pVBUS = NULL;
    if (SP_OK != GetShareMemory(ShareMemory_VBusPowerSupplyObject, (LPVOID)&pVBUS, sizeof(IDCS*)))
    {
        pVBUS = NULL;
    }

    // PowerSupply supports dual channel, channel1 is VBAT, channel2 is VBUS
    int32 nVbusChan = 0;
    DCS_CHANNEL_SPECIAL stChan;
    stChan.nChannelType = DC_CHANNEL_VBUS;
    if (SP_OK == m_pDCSource->GetProperty(DCP_CHANNEL_SPECIAL, NULL, &stChan))
    {
        nVbusChan = stChan.nChannel;
    }


    if (bOn)
    {
        // VBAT
        CHKRESULT(m_pDCSource->SetVoltage(fVBAT));

        if (NULL != pVBUS)
        {
            CHKRESULT(pVBUS->SetVoltage(fVBUS));
        }
        else if (nVbusChan > 0)
        {
            CHKRESULT(m_pDCSource->SetVoltage(fVBUS, DC_CHANNEL_VBUS));
        }
    }
    else
    {
        CHKRESULT(m_pDCSource->SetVoltage(static_cast<float>(-1.0)));

        if (NULL != pVBUS)
        {
            CHKRESULT(pVBUS->SetVoltage(static_cast<float>(-1.0)));
        }
        else if (nVbusChan > 0)
        {
            CHKRESULT(m_pDCSource->SetVoltage(static_cast<float>(-1.0), DC_CHANNEL_VBUS));
        }
    }

    return SP_OK;
}

//程控电源电压调整，此模式为EnterMode要求的模�?
//fVBUS_S 调压起始�?V)   fVotageStep 调压步进�?V)  nSetpTime步进间隔(ms)
SPRESULT CSpatBase::SwitchOnDC(BOOL bOn, float fVBAT, float fVBUS, float fVBUS_S, float fVotageStep, int nSetpTime)
{
    if (IsFakeDC())
    {
        return SP_OK;
    }

    // Separated PowerSupply for VBUS
    IDCS* pVBUS = NULL;
    if (SP_OK != GetShareMemory(ShareMemory_VBusPowerSupplyObject, (LPVOID)&pVBUS, sizeof(IDCS*)))
    {
        pVBUS = NULL;
    }

    // PowerSupply supports dual channel, channel1 is VBAT, channel2 is VBUS
    int32 nVbusChan = 0;
    DCS_CHANNEL_SPECIAL stChan;
    stChan.nChannelType = DC_CHANNEL_VBUS;
    if (SP_OK == m_pDCSource->GetProperty(DCP_CHANNEL_SPECIAL, NULL, &stChan))
    {
        nVbusChan = stChan.nChannel;
    }


    if (bOn)
    {
        // VBAT
        CHKRESULT(m_pDCSource->SetVoltage(fVBAT));

        // VBUS
        float fltExpVBUSVoltage = fVBUS_S - fVotageStep;    //进入循环后会先增加一个步进，故此这里先减一个步�?
        while (fltExpVBUSVoltage < fVBUS - 0.001)    //防止浮点数出�?.999999的情�?
        {
            fltExpVBUSVoltage += fVotageStep;
            fltExpVBUSVoltage = fltExpVBUSVoltage > fVBUS ? fVBUS : fltExpVBUSVoltage;

            if (nSetpTime > 120)        //电压调整最小间隔是120ms左右8ms（这个值是由设备决定的，实测得来（受测试设备数量限制，不能代表所有设备情况）�?
            {
                int nT = nSetpTime - 120;
                Sleep(nT);
            }

            if (NULL != pVBUS)
            {
                CHKRESULT(pVBUS->SetVoltage(fltExpVBUSVoltage));
            }
            else if (nVbusChan > 0)
            {
                CHKRESULT(m_pDCSource->SetVoltage(fltExpVBUSVoltage, DC_CHANNEL_VBUS));
            }
        }
    }
    else
    {
        CHKRESULT(m_pDCSource->SetVoltage(-1.0f));

        if (NULL != pVBUS)
        {
            CHKRESULT(pVBUS->SetVoltage(-1.0f));
        }
        else if (nVbusChan > 0)
        {
            CHKRESULT(m_pDCSource->SetVoltage(-1.0f, DC_CHANNEL_VBUS));
        }
    }

    return SP_OK;
}
