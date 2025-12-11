#include "StdAfx.h"
#include "MyAction.h"
#include "ISeqParse.h"
#pragma comment(lib, "iSpLog.lib")

//
extern ISeqParse*    g_lpSeqParse;

//////////////////////////////////////////////////////////////////////////
CMyAction::CMyAction(const SPAT_EXPORT_FUNCS& fn)
    : m_fn(fn)
    , m_hAction(INVALID_NPI_HANDLE)
    , m_pContainer(NULL)
    , m_hDiagPhone(INVALID_NPI_HANDLE)
    , m_pLog(NULL)
{
    CreateISpLogObject(&m_pLog);
}

CMyAction::~CMyAction(void)
{
    m_pLog->Release();
    m_pLog = NULL;
}

BOOL CMyAction::Create(const SPCALLBACK_PARAM& cb, INT nID)
{
    if (NULL != m_pLog)
    {
        OpenArgs_T args;
        strcpy_s(args.szModule, "SpatLib");
        args.nLogLevel  = SPLOGLV_VERBOSE;
        swprintf_s(args.Local.szLogFile, L"TaskLog%d.Log", nID);
     
        m_pLog->Open((LPCVOID)&args);
    }

    m_hAction = m_fn.CreateAction(L"Dummy", L"CDummyAction", m_pLog, NULL);
    if (INVALID_NPI_HANDLE == m_hAction)
    {
        printf("CreateAction(%d) failed!\n", nID);
        return FALSE;
    }

    if (NULL == m_pContainer)
    {
        m_pContainer = CreateContainer();
        if (NULL == m_pContainer)
        {
            printf("CreateContainer(%d) failed!\n", nID);
            return FALSE;
        }
    }

    if (NULL == m_hDiagPhone)
    {
        m_hDiagPhone = SP_CreatePhone(m_pLog);
        if (NULL == m_hDiagPhone)
        {
            printf("SP_CreatePhone(%d) failed!\n", nID);
            return FALSE;
        }
    }
    

    // 设置回调参数
    m_fn.SetCallBack(m_hAction, &cb); 

    SPAT_INIT_PARAM param;
    param.lpActionName  = L"ITEM1"; // Action名称必须和SEQ文件中<<Node Name="ITEM1"> Name属性一致
    param.pSeqParse     = g_lpSeqParse;
    param.pContainer    = m_pContainer;
    param.hDUT          = m_hDiagPhone;
    param.nTASK_ID      = nID;
    // 本示例SPAT_INIT_PARAM结构中有很多成员没有赋值，在实际调用中需要补充赋值
    // 具体请参见Framework中相关调用的源码 CSpatTask::GetAdjParam(const CSpatAction& Action)
    param.pRF           = NULL; // RF测试仪器操作对象
    param.pDC           = NULL;
    // ... 
    param.hStopEvent    = NULL;
    SPRESULT res = m_fn.InitAction(m_hAction, &param);
    if (SP_OK != res)
    {
        printf("InitAction() failed. %d\n", res);
        return FALSE;
    }

    return TRUE;
}

void CMyAction::Free(void)
{
    if (INVALID_NPI_HANDLE != m_hAction)
    {
        m_fn.FinalAction(m_hAction);
        m_fn.FreeAction(m_hAction);
        m_hAction = INVALID_NPI_HANDLE;
    }

    if (NULL != m_pContainer)
    {
        m_pContainer->Release();
        m_pContainer = NULL;
    }

    if (NULL != m_hDiagPhone)
    {
        SP_ReleasePhone(m_hDiagPhone);
        m_hDiagPhone = INVALID_NPI_HANDLE;
    }

    m_pLog->Close();
}

void CMyAction::run(void)
{
    m_fn.EnterAction(m_hAction);
    m_fn.PollAction(m_hAction);
    m_fn.LeaveAction(m_hAction);
}

void CMyAction::Exec(void)
{
    CThread::start();
}
