#pragma once
#include "../ImpBase.h"
#include "AudioLoopBase.h"

//////////////////////////////////////////////////////////////////////////


class CAudioLoopAfcb:public CAudioLoopBase
{
public:
    CAudioLoopAfcb(TGroupAudioLoop tGroup, CImpBase* pImp);
    virtual ~CAudioLoopAfcb(void);
    virtual SPRESULT Run();

private:

    SPRESULT AFCB_VoltageGetN();
    SPRESULT AFCB_VoltageGetP();

    SPRESULT MicAction();
    SPRESULT MicReadAction();
};