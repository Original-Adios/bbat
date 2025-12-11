#pragma once
#include "ImpBase.h"
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
class CImpWin32 : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImpWin32)
public:
    CImpWin32(void);
    virtual ~CImpWin32(void);

protected:
    // Initialize and finalize the internal resource if needed.
    virtual SPRESULT __InitAction (void);
    virtual SPRESULT __FinalAction(void);

    // 
    // Each time user press START button, Simba will invoke the APIs as following sequence:
    //     __EnterAction();
    //     SPRESULT res = __PollAction();
    //     __LeaveAction();
    //     return res;
    //
    virtual void     __EnterAction(void);
    virtual SPRESULT __PollAction (void);
    virtual void     __LeaveAction(void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);

private:
    // <Option> ... </Option>
    std::wstring m_A_textValue;     // <A>I am a text parameter in &lt;Option&gt;</A>
    double m_B_doubleValue[4];      // <B>33.0, 66.0, 99.0, 100</B>
    BOOL m_C_boolValue;             // <C>0</C>
    string_t m_D_comboTextValue;    // <D Control="ComboBox" Elements="Option1,Option2,Option3">Option2</D>
    std::string m_E2_textValue;     // <E2>88.88,99.99</E2>
    uint32 m_F2_intValue[2];        // <F2>100,20</F2>

    // <Param> ... </Param>
    std::wstring m_P1_textValue;    // <P1>I am a text parameter in &lt;Param&gt;</P1>
    double m_P52_doubleValue[4];    // <P52>11.1,22.22</P52>

    // 
    std::vector<uint8> m_vData;
};
