#include "StdAfx.h"
#include "ImpMFC.h"
#include <tchar.h>

//
IMPLEMENT_RUNTIME_CLASS(CImpMFC)

//////////////////////////////////////////////////////////////////////////
CImpMFC::CImpMFC(void)
: m_C_boolValue(FALSE)
{
    for (INT i=0; i<ARRAY_SIZE(m_B_doubleValue); i++)
    {
        m_B_doubleValue[i] = INVALID_NEGATIVE_DOUBLE_VALUE;
    }

    for (INT i=0; i<ARRAY_SIZE(m_F2_intValue); i++)
    {
        m_F2_intValue[i] = (uint32)INVALID_NEGATIVE_INTEGER_VALUE;
    }

    for (INT i=0; i<ARRAY_SIZE(m_P52_doubleValue); i++)
    {
        m_P52_doubleValue[i] = INVALID_POSITIVE_DOUBLE_VALUE;
    }
}

CImpMFC::~CImpMFC(void)
{
}

BOOL CImpMFC::LoadXMLConfig(void)
{
    /*
        <Node Name="XXXXX" Description="XXXXXXXXXXXXXXXXXXXXXXXXXX" Enabled="1" Type="Action">
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
            </Param>
        </Node>

        For Example:
        --------------------
        BOOL bBandA = (BOOL)GetConfigValue(L"Option:BAND:A", 0);
        BOOL bBandB = (BOOL)GetConfigValue(L"Option:BAND:B", 0);

        long CH1 = (long)GetConfigValue(L"Param:DLChannel:Channel1", 10800);
        assert(10700 == CH1);

        std::wstring sValue = GetConfigValue(L"Param:CellPower", L"-50.0,-50.0,-50.0");
        assert(L"-70.0,-80.0,-90.0" == sValue);

        INT nCount = 0;
        double* parrDouble = GetTokenDoubleW(sValue.c_str(), DEFAULT_DELIMITER_W, nCount);
        if (NULL == parrDouble || 3 != nCount)
        {
            return FALSE;
        }
        else
        {
            assert(-70.0 == parrDouble[0] && -80.0 == parrDouble[1] && -90.0 == parrDouble[2]);

            m_dDemoDoubleConfigVars[0] = parrDouble[0];
            m_dDemoDoubleConfigVars[1] = parrDouble[1];
            m_dDemoDoubleConfigVars[2] = parrDouble[2];
        }
    */

    // TODO: Load the configuration of XXXX.XML specified by IDR_XML_XXXX defined in <ActionList.h> 

    std::wstring strValue = L"";
    INT nCount = 0;
    double* pDouble = NULL;
    INT* pInt = NULL;
    
    //
    // TODO:  Load the configuration of node <Option> ... </Option>
    //

    // <A>I am a text parameter in &lt;Option&gt;</A>
    m_A_textValue = GetConfigValue(L"Option:A", L"DefaultTextValue");

    // <B>33.0, 66.0, 99.0, 100</B>
    strValue = GetConfigValue(L"Option:B", L"");
    pDouble  = GetTokenDoubleW(strValue.c_str(), DEFAULT_DELIMITER_W, nCount);
    if (nCount != ARRAY_SIZE(m_B_doubleValue))
    {
        return FALSE;
    }
    else
    {
        for (INT i=0; i<nCount; i++)
        {
            m_B_doubleValue[i] = pDouble[i];
        }
    }

    // <C Control="CheckBox" Description="I am CheckBox 复选框 in &lt;Option&gt;">0</C>
    m_C_boolValue = (BOOL)GetConfigValue(L"Option:C", (INT)FALSE);

    // <D Control="ComboBox" Elements="Option1,Option2,Option3" Description="I am ComboBox 下拉框 in &lt;Option&gt;">Option2</D>
    LPCWSTR COMBO_ELEMENTS[3] = {L"Option1", L"Option2", L"Option3"};
    strValue = GetConfigValue(L"Option:D", L"OptionX");
    BOOL bFound = FALSE;
    for (INT i=0; i<ARRAY_SIZE(COMBO_ELEMENTS); i++)
    {
        if (0 == strValue.compare(COMBO_ELEMENTS[i]))
        {
            bFound = TRUE;
            m_D_comboTextValue = _W2CT(strValue.c_str());       // Convert to TCHAR string
            break;
        }
    }
    if (!bFound)
    {
        // Not found the option
        return FALSE;
    }

    /*
        <E Property="ReadOnly" Description="I am ReadyOnly 只读 in &lt;Option&gt;">
            <E1>Example for read only property</E1>
            <E2>88.88,99.99</E2>
        </E>
    */
    strValue = GetConfigValue(L"Option:E:E2", L"");
    m_E2_textValue = _W2CA(strValue.c_str());   // Convert to ANSI string

    /*
        <F Property="InVisible" Description="I am invisible 看不到我 in &lt;Option&gt;">
            <F1>Example for invisible property</F1>
            <F2>100,20</F2>
        </F>
    */
    strValue = GetConfigValue(L"Option:F:F2", L"");
    pInt     = GetTokenIntegerW(strValue.c_str(), DEFAULT_DELIMITER_W, nCount);
    if (nCount != ARRAY_SIZE(m_F2_intValue))
    {
        return FALSE;
    }
    else
    {
        for (INT i=0; i<nCount; i++)
        {
            m_F2_intValue[i] = pInt[i];
        }
    }

    //
    // TODO:  Load the configuration of node <Param> ... </Param>
    // 
    // It is almost same with the loading node <Option> ... </Option>
    // The ONLY different is the node key path replaced "Option:xxx" by "Param:xxx"
    // Here we just list two examples.
    //

    // <P1>I am a text parameter in &lt;Param&gt;</P1>
    m_P1_textValue = GetConfigValue(L"Param:P1", L"");
    
    /*
        <P5 Property="ReadOnly" Description="I am ReadyOnly 只读 in &lt;Param&gt;">
            <P51>Example for read only property</P51>
            <P52>11.1,22.22</P52>
        </P5>
    */
    strValue = GetConfigValue(L"Param:P5:P52", L"");
    pDouble  = GetTokenDoubleW(strValue.c_str(), DEFAULT_DELIMITER_W, nCount);
    if (nCount != ARRAY_SIZE(m_P52_doubleValue))
    {
        return FALSE;
    }
    else
    {
        for (INT i=0; i<nCount; i++)
        {
            m_P52_doubleValue[i] = pDouble[i];
        }
    }


    return TRUE;
}

SPRESULT CImpMFC::__InitAction(void)
{
    // TODO: Initialize the internal variables and resource 
    //  
    // 1. LoadXMLConfig API will be invoked before __InitAction, refer to CSpatBase::InitAction  
    // 2. __InitAction will be invoked ONLY once while seq. is loaded 
    // 

    return SP_OK;
}

void CImpMFC::__EnterAction(void)
{
    // TODO: Internal resource allocation and initialization
    const INT SIZE = 20;
    m_vData.resize(SIZE);
    for (INT i=0; i<SIZE; i++)
    {
        m_vData[i] = (uint8)('3' + i);
    }
}

SPRESULT CImpMFC::__PollAction(void)
{ 
    // TODO: Implement the detail testing process here:
    //
    // 1. Invoke CSpatBase::'NOTIFY' macro to show items in UI:  name, condition, lower, data, upper, unit ...
    //    Refer to CDummyAction::__PollAction of Dummy\FuncImp\DummyAction.cpp to check how to show an item in UI
    //
    // 2. How to invoke PhoneCommand APIs ?
    //    uint8 szVer[1000] = {0};
    //    CHKRESULT(SP_GetSWVer(m_hDUT, CP, szVer, sizeof(szVer)));   
    //
    // 3. How to control instrument by m_pRFTester, m_pDCSource handle defined in CSpatBase
    //    m_pRFTester->SetNetMode(NM_GSM);
    //    m_pDCSource->SetVoltage(4.0);
    //
    // 4. Some static utility classes such as CUtility, CgsmUtility, CwcdmaUtility and CLteUtility are also provided 
    //    by the path 'Source\Common\UtilFn\'.
    //      
    // 5. Share memory is provided in order to transfer data between two SpatLib libraries:
    //    For example:
    //    uint32 u32flag = 0;
    //    CHKRESULT(GetShareMemory(L"MySharedMemoryName", (void*       )&u32Flag, sizeof(u32Flag)));   // Get data from share memory
    //    CHKRESULT(SetShareMemory(L"MySharedMemoryName", (const void* )&u32Flag, sizeof(u32Flag)));   // update data into share memory

    // Trace print, trace will be saved in \Bin\Log\YYYY_MM_DD\DUTX\XXXXX\Trace.log
    LogFmtStrW(SPLOGLV_INFO, L"A = %s", m_A_textValue.c_str());

    LogRawStrA(SPLOGLV_VERBOSE, "Print data:");
    LogBufData(SPLOGLV_INFO, (const void* )&m_vData[0], m_vData.size());

    // Set share memory
    SetShareMemory(L"MyNamedMemory", (const void* )&m_F2_intValue[0], sizeof(m_F2_intValue));

    // UI 
    for (INT i=0; i<ARRAY_SIZE(m_B_doubleValue); i++)
    {
        CHAR szName[32] = {0};
        sprintf_s(szName, "B[%d]", i);
        NOTIFY(szName, LEVEL_UI|LEVEL_REPORT, 0.0, m_B_doubleValue[i], 100.0, NULL, -1, "dBm", NULL);
    }

    // Get share memory and check the last value 
    uint32 u32Array[2] = {0};
    GetShareMemory(L"MyNamedMemory", (void* )&u32Array, sizeof(m_F2_intValue));
    for (INT i=0; i<ARRAY_SIZE(m_F2_intValue); i++)
    {
        NOTIFY("CheckShareMemory", LEVEL_UI, NOLOWLMT, u32Array[i], NOUPPLMT, NULL, -1, NULL, "Original F2[%d] = %d", i, m_F2_intValue[i]);
        if (m_F2_intValue[i] != u32Array[i])
        {
            return SP_E_SPAT_OUT_OF_RANGE;
        }
    }


    return SP_OK;
}

void CImpMFC::__LeaveAction(void)
{
    // TODO: Cleanup internal resources
    m_vData.clear();
}

SPRESULT CImpMFC::__FinalAction(void)
{ 
    // TODO: Finalize the action and cleanup the internal resource

    return SP_OK;
}
