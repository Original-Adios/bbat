LPCSTR static g_lpAntNameA[MAX_LTE_ANT] = {"Primary", "Diversity"};
LPCSTR static g_lpCaNameA[MAX_LTE_CA] = {"PCC", "SCC"};
LPCSTR static g_lpDoorNameA[MAX_SETTINGS] = {"Outdoor", "Indoor"};
LPCWSTR static g_lpAntNameW[MAX_LTE_ANT] = {L"Primary", L"Diversity"};
LPCWSTR static g_lpCaNameW[MAX_LTE_CA] = {L"PCC", L"SCC"};
LPCWSTR static g_lpDoorNameW[MAX_SETTINGS] = {L"Outdoor", L"Indoor"};
LPCSTR static g_lpDcNameA[2] = {"I", "Q"};
LPCSTR static g_lpBWNameA[MAX_LTE_BW] = {"1.4MHz", "3MHz", "5MHz", "10MHz", "15MHz", "20MHz"};
LPCSTR static g_lpRBNameA[MAX_RB_STATUS] = {"1RB_MIN", "1RB_MID", "1RB_MAX", "PRB_MIN", "PRB_MID", "PRB_MAX", "FRB"};

LTE_ANT_FLAG_E static g_LteAntFlag[MAX_LTE_ANT] = {LTE_ANT_FLAG_MAIN, LTE_ANT_FLAG_DIV};
LTE_CA_FLAG_E static g_LteCaFlag[MAX_LTE_CA] = {LTE_CA_FLAG_PCC, LTE_CA_FLAG_SCC};

#define CURRENT_CA L"current ca"
#define SPATBASE L"Spatbase"


#define FUNC_INAL L"INAL"
#define FUNC_INV L"INv"
#define FUNC_INAL_MANAGER L"INAL Manager"
#define FUNC_FILE_CONFIG L"File Config"

#define ALGO_APC_MAIN_PCC L"algo apc main pcc"
#define ALGO_APC_DIV_SCC L"algo apc div scc"

#define ALGO_APC_SCAN_MAIN_PCC L"algo apc scan main pcc"
#define ALGO_APC_SCAN_DIV_SCC L"algo apc scan div scc"

#define ALGO_TXDC_MAIN_PCC L"algo tx dc main pcc"
#define ALGO_TXDC_DIV_SCC L"algo tx dc div scc"

#define ALGO_AGC_MAIN_PCC L"algo agc main pcc"
#define ALGO_AGC_MAIN_SCC L"algo agc main scc"
#define ALGO_AGC_DIV_PCC L"algo agc div pcc"
#define ALGO_AGC_DIV_SCC L"algo agc div scc"

#define ALGO_AGC_MAIN_CA_PCC L"algo agc main ca pcc"
#define ALGO_AGC_MAIN_CA_SCC L"algo agc main ca scc"
#define ALGO_AGC_DIV_CA_PCC L"algo agc div ca pcc"
#define ALGO_AGC_DIV_CA_SCC L"algo agc div ca scc"

#define ALGO_AGC_SCAN_MAIN_PCC L"algo agc scan main pcc"
#define ALGO_AGC_SCAN_MAIN_SCC L"algo agc scan main scc"
#define ALGO_AGC_SCAN_DIV_PCC L"algo agc scan div pcc"
#define ALGO_AGC_SCAN_DIV_SCC L"algo agc scan div scc"

#define ALGO_IRR_MAIN_PCC L"algo irr main pcc"
#define ALGO_IRR_MAIN_SCC L"algo irr main scc"
#define ALGO_IRR_DIV_PCC L"algo irr div pcc"
#define ALGO_IRR_DIV_SCC L"algo irr div scc"

#define ALGO_FDIQ_MAIN L"algo fdiq main"
#define ALGO_FDIQ_DIV L"algo fdiq div"

#define ALGO_TXDROOP L"algo txdroop"
#define ALGO_PDT L"algo pdt"

#define API_APC L"api apc"
#define API_PDT L"api pdt"
#define API_AGC L"api agc"
#define API_IRR L"api irr"
#define API_FDIQ L"api fdiq"
#define API_TXDC L"api tx dc"
#define API_RSRP L"api RSRP"
#define API_TXDROOP L"api txdroop"

#define MAX_AGC_RETRY_COUNT 5

#define CHKRESULT_WITH_RAW_STR_W(statement, RawStr) \
{										            \
    SPRESULT __sprslt = (statement);                \
    if (SP_OK != __sprslt)                          \
    {                                               \
        LogRawStrW(SPLOGLV_ERROR, RawStr);          \
        return __sprslt;                            \
    }                                               \
}

#define CHKRESULT_WITH_FMT_STR_W(statement, FmtStr, ...)    \
{										                    \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        LogFmtStrW(SPLOGLV_ERROR, FmtStr, __VA_ARGS__);     \
        return __sprslt;                                    \
    }                                                       \
}

#define CHKRESULT_WITH_RAW_STR_A(statement, RawStr) \
{										            \
    SPRESULT __sprslt = (statement);                \
    if (SP_OK != __sprslt)                          \
    {                                               \
        LogRawStrA(SPLOGLV_ERROR, RawStr);          \
        return __sprslt;                            \
    }                                               \
}

#define CHKRESULT_WITH_FMT_STR_A(statement, FmtStr, ...)    \
{										                    \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        LogFmtStrA(SPLOGLV_ERROR, FmtStr, __VA_ARGS__);     \
        return __sprslt;                                    \
    }                                                       \
}

#define CHKRESULT_WITH_FAIL_DELEGATE(statement, delegate)   \
{										                    \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        delegate                                            \
        return __sprslt;                                    \
    }                                                       \
}

#define IS_USER_STOP        \
if (_IsUserStop())          \
{                           \
    return SP_E_USER_ABORT; \
}

#define IS_USER_STOP_FUNC           \
    if (m_pSpatBase->_IsUserStop())  \
{                                   \
    return SP_E_USER_ABORT;         \
}