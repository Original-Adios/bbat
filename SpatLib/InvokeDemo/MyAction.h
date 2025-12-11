#pragma once
#include "SpatLibExport.h"
#include "Thread.h"
#include "IContainer.h"
#include "PhoneCommand.h"
#include "ISpLogExport.h"

class CMyAction : public CThread
{
public:
    CMyAction(const SPAT_EXPORT_FUNCS& fn);
    virtual ~CMyAction(void);

    BOOL Create(const SPCALLBACK_PARAM& cb, INT nID);
    void Exec(void);
    void Free(void);

protected:
    virtual void run(void);

private:
    SPAT_EXPORT_FUNCS m_fn;
    SP_HANDLE m_hAction;
    IContainer* m_pContainer;
    SP_HANDLE m_hDiagPhone;
    ISpLog* m_pLog;
};

