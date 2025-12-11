#pragma once
#include "../ImpBase.h"
#include "GpioAfcbGet.h"
#include "GpioAfcbSet.h"
#include <vector>
enum GpioTestType
{
    PhoneToG3, 
    G3ToPhone
};

class CImpGpioAfcb :public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImpGpioAfcb)

public:
    CImpGpioAfcb(void);
    virtual ~CImpGpioAfcb(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);

    virtual void BuildSteps();
    virtual SPRESULT GpioAction();

    virtual SPRESULT GpioInit();
    virtual SPRESULT GpioReset();

    vector<CGpioBase*> m_vecSteps;
    vector<TGroupGpioParam> m_vecTestItems;
    GpioTestType m_eType = PhoneToG3;
    int m_nGroupSize = 0;
    BOOL m_bFailStop = FALSE;
    LPCWSTR lpTestName[2] =
    {
        L"PhoneToG3",
        L"G3ToPhone"
    };   

};
