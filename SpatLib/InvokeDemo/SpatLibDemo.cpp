// SpatLibDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include "LockQueue.h"
#include "ISeqParse.h"
#include "MyAction.h"


using namespace std;
#pragma comment(lib, "SeqParse.lib")
#pragma comment(lib, "Container.lib")
#pragma comment(lib, "PhoneCommand.lib")

//////////////////////////////////////////////////////////////////////////
// 1.   重定义CALLBACKDATA_TEST_ITEM结构内的指针对象，避免回调后指针失效。
//      本例程中只是选取了CALLBACKDATA_TEST_ITEM中的 部分 成员重定义作为示例。
// 
// 2.   CALLBACKDATA_DUT_INFO处理和CALLBACKDATA_TEST_ITEM类似，本例中不再举例。
typedef struct _MySpatLibCallBackItem 
{
    string   strName;  
    string   strUnit;
    SPAT_VALUE Value;

    _MySpatLibCallBackItem(void) { }
    _MySpatLibCallBackItem(const _MySpatLibCallBackItem& rhs) 
    {
        strName  = rhs.strName;
        strUnit  = rhs.strUnit;
        Value    = rhs.Value;
    }
    const _MySpatLibCallBackItem& operator=(const _MySpatLibCallBackItem& rhs)
    {
        if (this != &rhs)
        {
            strName  = rhs.strName;
            strUnit  = rhs.strUnit;
            Value    = rhs.Value; 
        }
        
        return *this;
    }

} MySpatLibCallBackItem_T;

// 全局变量申明
CLockQueue<MySpatLibCallBackItem_T> g_cbList;       // 锁队列，存储回调的数据
volatile BOOL g_bStopThread = TRUE;                 // 线程终止标识
ISeqParse*    g_lpSeqParse  = NULL;                 // SEQ解析器对象

// 将SpatLib回调函数打印到Console Window 
void Show(MySpatLibCallBackItem_T& MyData);
// 清理全局对象，并退出程序
int  Quit(void);
// 将SpatLib的回调数据插入锁队列
void CollectData(void);
// SpatLib回调处理函数
BOOL CALLBACK MyCallback(LPCSPCALLBACK_DATA lpcbData, LPCVOID);

CONST INT MAX_TASK_COUNT = 4;

//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
    WCHAR szAppPath[MAX_PATH] = {0};
    GetModuleFileNameW(NULL,  szAppPath, MAX_PATH);
    LPWSTR lpChar = wcsrchr(szAppPath, L'\\');
    if (NULL != lpChar)
    {
        *lpChar = L'\0';
    }

#pragma region CreateSeqParseObjectAndLoadSeqFile
    // 创建SEQ文件解析对象SeqParse并加载SEQ文件
    WCHAR szSeqPath[MAX_PATH] = {0};
    swprintf_s(szSeqPath, MAX_PATH, L"%s\\..\\Project\\demo.seq", szAppPath);
    g_lpSeqParse  = CreateSeqParse(NULL);
    if (NULL == g_lpSeqParse)
    {
        return Quit();
    }
    else
    {
        if (SP_OK != g_lpSeqParse->LoadSeq(szSeqPath))
        {
            printf("Seq file load failed!\n");
            return Quit();
        }
    }
#pragma endregion
    


#pragma region LoadSpatLibLibrary
    // 在指定的SEQ文件中可以查阅需要加载的SpatLib库名称，比如demo.seq文件中ITEM1这个测试项目：
    //
    //      <Node Name="ITEM1" Description="Dummy Action" Enabled="1" Type="Action">
    //      <SpatLib Name="Dummy.dll" FnTag="Dummy" Class="CDummyAction" />
    //
    // ITEM1这个测试项目加载Dummy.dll
    //
    WCHAR szLibPath[MAX_PATH] = {0};
    swprintf_s(szLibPath, L"%s\\SpatLib\\Dummy.dll", szAppPath);
    HMODULE hDLL = LoadLibraryW(szLibPath);
    if (NULL == hDLL)
    {
        printf("Library load failed. 0x%X\n", GetLastError());
        return Quit();
    }

    SPAT_EXPORT_FUNCS fns;
    fns.Startup         = (LPSpat_Startup)      GetProcAddress(hDLL, "Spat_Startup"     );
    fns.Cleanup         = (LPSpat_Cleanup)      GetProcAddress(hDLL, "Spat_Cleanup"     );
    fns.CreateAction    = (LPSpat_CreateAction) GetProcAddress(hDLL, "Spat_CreateAction");
    fns.FreeAction      = (LPSpat_FreeAction)   GetProcAddress(hDLL, "Spat_FreeAction"  );
    fns.SetCallBack     = (LPSpat_SetCallBack)  GetProcAddress(hDLL, "Spat_SetCallBack" );
    fns.SetDriver       = (LPSpat_SetDriver)    GetProcAddress(hDLL, "Spat_SetDriver"   );
    fns.InitAction      = (LPSpat_InitAction)   GetProcAddress(hDLL, "Spat_InitAction"  );
    fns.EnterAction     = (LPSpat_EnterAction)  GetProcAddress(hDLL, "Spat_EnterAction" );
    fns.PollAction      = (LPSpat_PollAction)   GetProcAddress(hDLL, "Spat_PollAction"  );
    fns.FinalAction     = (LPSpat_FinalAction)  GetProcAddress(hDLL, "Spat_FinalAction" );
    fns.LeaveAction     = (LPSpat_LeaveAction)  GetProcAddress(hDLL, "Spat_LeaveAction" );
    fns.SetProperty     = (LPSpat_SetProperty)	GetProcAddress(hDLL, "Spat_SetProperty" );
    fns.GetProperty     = (LPSpat_GetProperty)  GetProcAddress(hDLL, "Spat_GetProperty" );

    // 检查库导出接口是否正常
    if (!fns.operator bool())
    {
        printf("GetProcAddress() failed. 0x%X\n", GetLastError()); 
        FreeLibrary(hDLL);
        return Quit();
    }

    // SpatLib库初始化
    LPSPAT_DATA lpData = NULL;
    SPRESULT res = fns.Startup(&lpData);
    if (SP_OK != res)
    {
        printf("Startup() failed. %d\n", res);
        FreeLibrary(hDLL);
        return Quit();
    }
#pragma endregion

    // 单线程在Console Window显示，多线程打印到LOG当中
    CThread DisplayThread(&CollectData);
    if (1 == MAX_TASK_COUNT)
    {
        DisplayThread.start();
    }

    CMyAction* pTaskList[MAX_TASK_COUNT] = {NULL};
    for (INT i=0; i<MAX_TASK_COUNT; i++)
    {
        pTaskList[i] = new CMyAction(fns);
        SPCALLBACK_PARAM cb;
        cb.pFunc = MyCallback;
        pTaskList[i]->Create(cb, i+1);
    }
  
    for (INT i=0; i<MAX_TASK_COUNT; i++)
    {
        // 在线程中执行TASK
        pTaskList[i]->Exec();
    }

    for (INT i=0; i<MAX_TASK_COUNT; i++)
    {
        // 等待每个线程结束
        pTaskList[i]->join();
    }

    for (INT i=0; i<MAX_TASK_COUNT; i++)
    {
        pTaskList[i]->Free();
    }

    if (1 == MAX_TASK_COUNT)
    {
        g_bStopThread = TRUE;
        DisplayThread.join();
    }

    // 释放SpatLib库句柄
    fns.Cleanup();
    FreeLibrary(hDLL);

	return Quit();
}

// 将SpatLib回调数据打印到Console Window
void Show(MySpatLibCallBackItem_T& MyData)
{
    printf("%s\t%.2f  %.2f  %.2f  %s  %s\n", \
        MyData.strName.c_str(), \
        MyData.Value.low, MyData.Value.val, MyData.Value.upp, \
        MyData.strUnit.c_str(), \
        MyData.Value.operator bool() ? "pass" : "fail");
}

//////////////////////////////////////////////////////////////////////////
// 清除全局对象退出程序
int Quit(void)
{
    if (NULL != g_lpSeqParse)
    {
        g_lpSeqParse->Release();
        g_lpSeqParse = NULL;
    }

    printf("-- End\n");
    printf("Press any key to Quit.\n");
    return getchar();
}

//////////////////////////////////////////////////////////////////////////
void CollectData(void)
{
    g_bStopThread = FALSE;
    while (!g_bStopThread)
    {
        if (g_cbList.size() > 0)
        {
            MySpatLibCallBackItem_T* pMyData = g_cbList.pop_front();
            Show( *pMyData);
            delete pMyData;

        }
        else
        {
            Sleep(1);
        }
    }

    /// Free all messages which are not handled
    int nCount = g_cbList.size();
    for (int i=0; i<nCount; i++)
    {
        MySpatLibCallBackItem_T* pMyData = g_cbList.pop_front();
        Show( *pMyData);
        delete pMyData;
    }
}

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK MyCallback(LPCSPCALLBACK_DATA lpcbData, LPCVOID)
{
    if (MAX_TASK_COUNT > 1)
    {
        return TRUE;
    }

    if (NULL == lpcbData)
    {
        return FALSE;
    }

    if (CALLBACK_TEST_ITEM == lpcbData->eType)
    {
        // 本例中仅就CALLBACK_TEST_ITEM一种回调类型作示例参考
        // SpatLib库还支持CALLBACK_DUT_INFO回调，处理方式类同，不再赘述
        LPCALLBACKDATA_TEST_ITEM  lpItem = LPCALLBACKDATA_TEST_ITEM(lpcbData->lpData);

        MySpatLibCallBackItem_T *pMyItem = new MySpatLibCallBackItem_T;
        pMyItem->strName = lpItem->lpName;
        pMyItem->Value   = lpItem->Value;
        pMyItem->strUnit = lpItem->lpUnit;

        g_cbList.push_back(pMyItem);
    }

    return TRUE;
}
