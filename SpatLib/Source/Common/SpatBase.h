#pragma once
#include "ISpatAction.h"
#include "SpatEngineExport.h"
#include "SpatRuntimeClass.h"
#include "SpatEngineExport.h"
#include "PhoneCommand.h"
#include "IRFDevice.h"
#include "IDCS.h"
#include "Tr.h"
#include "Utility.h"
#include "ShareMemoryDefine.h"
#include "ISeqParse.h"
#include "IContainer.h"
#include "XException.h"
#include "InstrumentLock.h"
#include "callback_def.h"
#include "res_def.h"
#include "IItemDataObserver.h"
#include "ICallback.h"
#include "SpatSystemPrePtr.h"
#include "SpatContainer.hpp"
extern SpatContainer::CParamsContainer g_SpatParamsContainer;

class CModeSwitch;
///
#define CHKRESULT_WITH_REPAIR(statement, Mode, Band, Item)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        SetRepair(Mode, Band, Item);                        \
        return __sprslt;                                    \
    }                                                       \
}

#define RETURN_WITH_REPAIR(ErrorCode, Mode, Band, Item) \
{                                                        \
    SetRepair(Mode, Band, Item);                        \
    return ErrorCode;                                   \
}

#define RETURN_WITH_REPAIR_BAND(ErrorCode, Band)        \
{                                                        \
    SetRepairBand(Band);                                \
    return ErrorCode;                                   \
}

#define CHKRESULT_WITH_NOTIFY(statement, Itemname)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        NOTIFY(Itemname, LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
        return __sprslt;                                    \
    }                                                       \
}

#define CHKRESULT_WITH_NOTIFY1(pImpbase, statement, Itemname)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
{                                                       \
    pImpbase->NOTIFY(Itemname, LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
    return __sprslt;                                    \
}                                                       \
}

#define CHKRESULT_WITH_NOTIFY2(statement, Itemname, value,strparam1,strparam2)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        NOTIFY(Itemname,LEVEL_ITEM|LEVEL_DEBUG,1,value,1,strparam1,strparam2);                \
        return __sprslt;                                    \
    }                                                       \
}

#define CHKRESULT_WITH_NOTIFY_FUNNAME(statement)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
{                                                       \
    std::string itemName = #statement;                    \
    itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");            \
    NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
    return __sprslt;                                    \
}                                                       \
}

#define CHKRESULT_WITH_NOTIFY_FUNNAME1(pImpbase, statement)            \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
{                                                       \
    std::string itemName = #statement;                    \
    itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");            \
    pImpbase->NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);                \
    return __sprslt;                                    \
}                                                       \
}

#define CHKRESULT_WITH_UISHOW(pImpbase, statement, bUiShow)            \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
{                                                       \
        if(bUiShow)                                     \
{                                               \
            std::string itemName = #statement;                    \
            itemName.replace(itemName.find('('), itemName.size() - itemName.find('(') , "()");            \
            pImpbase->NOTIFY(itemName.c_str(), LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);        \
}    \
        return __sprslt;                                    \
}                                                       \
}


#define  CHKRESULT_WITH_GOTO(statement, result, dest)       \
{                                                           \
    result = (statement);                                   \
    if (SP_OK != result)                                    \
    {                                                       \
        goto dest;                                          \
    }                                                       \
}

#define CHKRESULT_WITH_NOTIFY_HANDLE_SIGNAL_OFF(pImpbase, statement, Itemname)  \
{                                                            \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
        pImpbase->m_pRFTester->EndTest(0);                   \
        pImpbase->NOTIFY(Itemname, LEVEL_ITEM|LEVEL_DEBUG, 1, 0, 1);  \
        return __sprslt;                                    \
    }                                                       \
}

#define CHKRESULT_WITH_HANDLE_SIGNAL_OFF(statement, pImpbase) \
{                                                              \
   SPRESULT __sprslt = (statement);                         \
   if (SP_OK != __sprslt)                                   \
   {                                                        \
     pImpbase->m_pRFTester->EndTest(0);                     \
     return __sprslt;                                       \
   }                                                        \
}


#define NOTIFY          _UiSendMsg
#define ShareMemory_AutoStart   L"08DC3789_79BB_4855_86DA_4C8183CB23F4_AUTO_START "

//////////////////////////////////////////////////////////////////////////
class CSpatBase : public ISpatAction
    , public CTr
    , public CUtility
{
public:
    CSpatBase(void);
    virtual ~CSpatBase(void);
    virtual void Release(void);

    void InitLog(LPVOID pLogUtil, UINT nLv);

    /// Property
    virtual SPRESULT SetProperty(INT nFlags, INT nOption, LPCVOID lpValue);
    virtual SPRESULT GetProperty(INT nFlags, INT nOption, LPVOID  lpValue);
public:
    const SPAT_INIT_PARAM& GetAdjParam(void)const { return m_AdjParam; };

    /// Check whether user abort or not
    BOOL _IsUserStop(void);

    /// UI & Log CallBack
    BOOL _UiSendMsg(
        const char* lpszName,
        uint32 nLv,
        double dLow, double dVal, double dUpp,
        const char* lpszBand, int32 nChannel,
        const char* lpszUnit,
        const char* lpsczCond, ...);
    BOOL _UiSendMsg(
        const char* lpszName,
        uint32 nLv,
        double dLow = 0.0, double dVal = 0.0, double dUpp = 0.0,
        const char* lpszBand = NULL, int32 nChannel = -1,
        const char* lpszUnit = NULL);
    BOOL _UiSendMsg(const char* lpsznvPath, int32 data);
    BOOL _UiSendMsg(const char* lpName, const char* lpInfo);

    BOOL _UiSendMsgPass(const char* lpszName,
        uint32 nLv,
        const char* lpszBand,
        int32 nChannel,
        const char* lpszUnit,
        const char* lpsczCond, ...);
    BOOL _UiSendMsgFail(const char* lpszName,
        uint32 nLv,
        const char* lpszBand,
        int32 nChannel,
        const char* lpszUnit,
        const char* lpsczCond, ...);

    BOOL InvokeCustomizeDataCallback(LPCSTR lpName, LPCVOID lpData);

    /// Container: Share Memory between SpatLib libraries
    SPRESULT    SetShareMemory(LPCWSTR lpszName, const void* lpValue, UINT32 u32Bytes, UINT32 nOption = IContainer::Normal);
    SPRESULT    GetShareMemory(LPCWSTR lpszName, void* lpValue, UINT32 u32Bytes, UINT32* pOption = NULL, BOOL bLog = TRUE);/*Bug 1763183 MES_Upload节点后100%闪退*/

    IContainer* GetContainer( void ) { return m_lpContainer; };
    ISeqParse* GetSeqParse( void ) { return m_lpSeqParse; };
    ICallback* GetCallback( void ) { return m_pCallback; };
    CSpatBase* GetSystemPrePtr( void ) { return m_pSystemPrePtr; };

    /// Engine: Enter/Leave/Final
    SPRESULT EnterEngine(BOOL bLock, DWORD dwTimeOut = INFINITE);
    SPRESULT LeaveEngine(BOOL bLock);
    SPRESULT FinalEngine(BOOL bLock, DWORD dwTimeOut = INFINITE);

    /// Get configuration values of SpatLib
    int      GetConfigValue(LPCWSTR lpPath, int  defValue, BOOL bGlobalSettings = FALSE);
    double   GetConfigValue(LPCWSTR lpPath, double  defValue, BOOL bGlobalSettings = FALSE);
    LPCWSTR  GetConfigValue(LPCWSTR lpPath, LPCWSTR defString, BOOL bGlobalSettings = FALSE);

    /// Change mode
    void     SetupDUTRunMode(RM_MODE_ENUM eMode, BS_CONFIG_T* pBS = NULL);
    SPRESULT ChangeDUTRunMode(void);

    // Repair Code
    void     SetRepair(RepairMode eRepairMode, LPCSTR pBand, LPCSTR pItem);
    void     SetRepairMode(RepairMode eRepairMode);
    void     SetRepairBand(LPCSTR pBand);
    void     SetRepairItem(LPCSTR pItem);
    RepairMode GetRepaireMode();
    LPCSTR   GetRepairBand(void);
    LPCSTR   GetRepairItem(void);

    BOOL     IsFakeDC(void);
    BOOL     IsFakeRFTester(void);
    void     SaveMaxMinPwr(LPCTSTR pLogName);
    void     SendWorkStatus();
    //uPercent: range 0~100
    void     SendProgress(uint32 uPercent);
    void     SendCommonCallback(LPCWSTR lpMessage, ...);

    SPRESULT SwitchOnDC(BOOL bOn, float fVBAT, float fVBUS);
    //程控电源电压调整，此模式为EnterMode要求的模式
    //fVBUS_S 调压起始值(V)   fVotageStep 调压步进值(V)  nSetpTime步进间隔(ms)
    SPRESULT SwitchOnDC(BOOL bOn, float fVBAT, float fVBUS, float fVBUS_S, float fVotageStep, int nSetpTime);

    virtual SPRESULT GetMoudlePtr(E_UNI_MODULE_PTR /*eType*/, LPVOID /*lpValue*/ ) { return SP_OK; };
    virtual SPRESULT SetMoudlePtr(E_UNI_MODULE_PTR /*eType*/, LPCVOID /*lpValue*/ ) { return SP_OK; };
protected:
    /// Load configuration from *.seq file including "Option" and "Param" node:
    virtual BOOL LoadXMLConfig(void);
    void LoadGlobalSettings(void);
    //
    virtual SPRESULT __InitAction(void) { return SP_OK; };
    virtual void     __EnterAction(void) {               };
    virtual SPRESULT __PollAction(void) = 0;
    virtual void     __LeaveAction(void) {               };
    virtual SPRESULT __FinalAction(void) { return SP_OK; };

    SPRESULT PrintErrorMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level);
    SPRESULT PrintSuccessMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level);

public:
    std::string m_strGlobalCondition = "";

private:
    void __LogItem(const CALLBACKDATA_TEST_ITEM& item);

    //////////////////////////////////////////////////////////////////////////
    /// Change ISpatAction interfaces to private to prohibit child class to re-implement this APIs directly
    /// Override by using __InitAciton/__EnterAction/__LeaveAction/__PollAction/__FinalAction APIs
    /// Driver Setup
    virtual SPRESULT SetDriver(INT nDriv, LPVOID pDriv, LPVOID pParam);

    /// Action Step
    /// -----------------------------------------------------------------------
    ///                !!! DO NOT virtual below APIs !!!
    /// -----------------------------------------------------------------------
private:
    virtual SPRESULT InitAction(LPCSPAT_INIT_PARAM pParam);
    virtual void     EnterAction(void);
    virtual SPRESULT PollAction(void);
    virtual void     LeaveAction(void);
    virtual SPRESULT FinalAction(void);

public:
    /// RunTime class name, this name is fixed (classXXXX), don't change
    static CSpatRuntimeClass classCSpatBase;

    SP_HANDLE         m_hDUT;
    IRFDevice* m_pRFTester;
    IDCS* m_pDCSource;
    CModeSwitch* m_pModeSwitch;
    BOOL              m_bFailStop;
    BOOL              m_bSaveFinalTestFlag;
    UINT32            m_u32MaxFailRetryCount;
    BOOL              m_bAutoStart;
    BS_CONFIG_T       m_CurrBS;
    UINT32            m_u32PreserveSpot;
    BOOL              m_bUiOnlyListItem;
private:
    UINT32            m_nLogLevel;
    IContainer* m_lpContainer;
    SP_HANDLE         m_hEngine;
    ISeqParse* m_lpSeqParse;
    ICallback* m_pCallback;
    CSpatBase* m_pSystemPrePtr = nullptr;

    std::wstring      m_strActionName;
    SPAT_INIT_PARAM   m_AdjParam;
    std::wstring      m_strXMLPath;
    std::string       m_strLog;

    RM_MODE_ENUM      m_eDutRunMode;

    // While during repeating to run, this flag will be set to TRUE
    // Fail item will not be shown to UI
    BOOL              m_bUnShowFailItem;
    BOOL              m_bClassCalFlag;

    RepairMode        m_eRepairMode;
    std::string       m_strRepairItem;
    std::string       m_strRepairBand;
};

//////////////////////////////////////////////////////////////////////////




