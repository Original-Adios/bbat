#pragma once
#include "SpatBase.h"
#include "BBAT_GlobelDef.h"
#include "IFunctionBox.h"
#define BYSEND_BUFF_SIZE  (0x400)


class CImpBase : public CSpatBase
{
public:
    LPCSTR BBAT_MIC_CHANNEL_NAME[MAX_MIC_CHANNEL] =
    {
      "MainMic",
      "HeadSetMic",
      "SubMic",
      "MidMic"
    };

    LPCSTR BBAT_SPK_CHANNEL_NAME[MAX_SPK_CHANNEL] =
    {
      "MainSpeaker",
      "MainReceiver",
      "HeadSet",
      "SubSpeaker",
    };

    LPCSTR BBAT_FP_MIC_CHANNEL_NAME[MAX_MIC_CHANNEL] =
    {
      "FP_MainMic",
      "FP_HeadSetMic",
      "FP_SubMic",
      "FP_MidMic"
    };

    LPCSTR BBAT_FP_SPK_CHANNEL_NAME[MAX_SPK_CHANNEL] =
    {
      "FP_Speaker",
      "FP_Receiver",
      "FP_HeadSet",
    };

    enum AFCB_ADC_Channel_TYPE : BYTE
    {
        Com_ADC,
        Main_Mic_P,
        Main_Mic_N,
        Sub_Mic_P,
        Sub_Mic_N,
        Head_Mic
    };

    enum AFCB_HeadSet_Mic_Mode_TYPE : BYTE
    {
        HeadSet_Mic_Off,
        HeadSet_Mic_Europe,
        HeadSet_Mic_American,
        HeadSet_Mic_NC
    };

    enum AFCB_HeadSet_LR_TYPE : BYTE
    {
        HeadSet_Left_Channel,
        HeadSet_Right_Channel
    };

    CImpBase(void);
    virtual ~CImpBase(void);

    int GetComboxValue(LPCWSTR* lpwSource, int nSourceSize, LPCWSTR lpPath);
    SPRESULT PrintSuccessMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level);
    SPRESULT PrintErrorMsg(SPRESULT Result, LPCSTR Msg, SPAT_LEVEL Level);
    SPRESULT GetPortInfo(UINT  nPortNum, SPDBT_INFO& epi, std::vector<SPDBT_INFO>& vecPortInfo);
    SPRESULT GetLocationPathRoot(SPDBT_INFO& epi, wstring& strRoot);;
    SPRESULT PortExist(int& nPort);
    SPRESULT AnalysisG3Audio(unsigned short* pWave, BYTE byCh, double& dSNR, double& dPeak);
    void GenerateSpeakerData(int* pData);
    void CalcMicData(BYTE* pData, BOOL bMic8k, AudioResult& tResult);
    SPRESULT GetG3HandleFromShareMemory();
    SPRESULT GetFunctionBoxHandleFromShareMemory();
    SPRESULT Getm_bAutoStartByJigFromShareMemory();
    SPRESULT Getm_bFunBoxInitFromShareMemory();
    SPRESULT ChargeOn(BOOL bStatus, ChargeVoltage eVoltage);
    SPRESULT PowerOn(BOOL bStatus);
    SPRESULT GetAppPath(void);

    AFC_CONTEXT m_hContext = nullptr;
    int m_nBoardPort = 0;
    IFunctionBox* m_pObjFunBox = NULL;
    int m_nG3Port = -1;
    BOOL m_bAutoStartByJig = FALSE;
    BOOL m_bFunBoxInit = FALSE;
    TCHAR m_szAppPath[_MAX_PATH] = { 0 };

protected:
    virtual void __EnterAction(void);

private:    
    SPRESULT CheckPortInfo(SPDBT_INFO& epi);
    SPRESULT InsertPortInfo(SPDBT_INFO& epi, std::vector<SPDBT_INFO>& vecPortInfo);


};

#define RETRY_PRINT(nRetry, lpPrint) if(nRetry!=0)PrintSuccessMsg(SP_OK,lpPrint,LEVEL_ITEM);

#define  FUNBOX_INIT_CHECK()                                \
{                                                           \
    if (FALSE == m_bFunBoxInit)                             \
    {                                                       \
        LogFmtStrA(SPLOGLV_ERROR, "FunctionBox Init Fail"); \
        return SP_E_BBAT_FUNBOX_INIT_FAIL;                  \
    }                                                       \
}

// 定义发送AT命令并检查响应的宏
#define CHECK_AT_COMMAND_RESPONSE(AT_COMMAND, RECV_BUFFER, SUCCESS_MARKER) \
    do { \
        std::string strReturnTemp = RECV_BUFFER; \
        if (strReturnTemp.find(SUCCESS_MARKER) == std::string::npos) { \
            NOTIFY(AT_COMMAND, LEVEL_ITEM, 1, 0, 1, NULL, -1, "-"); \
            return SP_E_PHONE_AT_EXECUTE_FAIL; \
        } \
    } while(0)