#pragma once
#include "../ImpBase.h"
#include "GpioSmpbase.h"
#include <vector>

#define MAXGIPOPAIRCOUNTS 50

class CImpGpioSmp :public CImpBase
{
    DECLARE_RUNTIME_CLASS(CImpGpioSmp)

public:
    CImpGpioSmp(void);
    virtual ~CImpGpioSmp(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __FinalAction(void);

    void BuildSteps();
    SPRESULT GpioAction();
    BOOL ReadGpioSetFile(LPCTSTR lpszFileName);

    //SPRESULT GpioInit();
    //SPRESULT GpioReset();

    vector<CGpioSmpBase*> m_vecSteps;
    vector<TGroupGpioSmpParam> m_vecTestItems;
    int m_nGroupSize = 0;
    wstring m_strGpioCsvPath;
    vector<pair<uint8_t, uint8_t>> m_vec_GpioPairs;
    BOOL m_bSetByFile = FALSE;
    BOOL m_bFailStop = FALSE;
    TCHAR m_szGpioCsvPath[_MAX_PATH] = { 0 };
};
