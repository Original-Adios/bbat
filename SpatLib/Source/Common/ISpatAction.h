#ifndef _ISPATACTION_F3DABC7D_3508_41EB_A3ED_B3C3EE9A2E3A_H_
#define _ISPATACTION_F3DABC7D_3508_41EB_A3ED_B3C3EE9A2E3A_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpatLibExport.h"


//////////////////////////////////////////////////////////////////////////
///
class ISpatAction
{
public:
    virtual ~ISpatAction(void) { };
    virtual void     Release(void) = 0;
 
    /// Driver Setup
    virtual SPRESULT SetDriver(INT nDriv, LPVOID pDriv, LPVOID pParam) = 0;

    /// Action Step
    virtual SPRESULT InitAction (LPCSPAT_INIT_PARAM pParam) = 0;
    virtual void     EnterAction(void) = 0;
    virtual SPRESULT PollAction (void) = 0;
    virtual void     LeaveAction(void) = 0;
    virtual SPRESULT FinalAction(void) = 0;

    /// Property
    virtual SPRESULT SetProperty(INT nFlags, INT nOption, LPCVOID lpValue) = 0;
    virtual SPRESULT GetProperty(INT nFlags, INT nOption, LPVOID  lpValue) = 0;
};

///
typedef ISpatAction* pISpatAction;

#endif /* _ISPATACTION_F3DABC7D_3508_41EB_A3ED_B3C3EE9A2E3A_H_ */