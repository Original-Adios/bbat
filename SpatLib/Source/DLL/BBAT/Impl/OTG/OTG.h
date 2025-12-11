#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////

#define GPIO_B_DIR_REG			0x13	// 1:OUTPUT , 0:INPUT
#define GPIO_B					0x1
#define GPIO_Data4				0x4	
class COtg : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COtg)
public:
    COtg(void);
    virtual ~COtg(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);
    SPRESULT OtgAction();
    SPRESULT Phone_OtgDisable();
    SPRESULT Phone_OtgRead();
    SPRESULT CheckReadValue(int nSpec);

    SPRESULT AFCB_GpioInit(BOOL bStatus);
    SPRESULT AFCB_GpioSet(BOOL bStatus);
    SPRESULT ReadAction(BOOL bStatus);
    int m_nReadValue = 0;
};