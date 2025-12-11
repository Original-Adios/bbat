#pragma once
#include "../ImpBase.h"
#include "AudioLoopBase.h"

class CAudioLoopPa :public CAudioLoopBase
{

public:
    CAudioLoopPa(TGroupAudioLoop tGroup, CImpBase* pImp);
    virtual ~CAudioLoopPa(void);
    virtual SPRESULT Run();

private:   

    SPRESULT AFCB_PA_Enable(bool bStatus);
    SPRESULT MicAction();
    SPRESULT MicReadAction();
};