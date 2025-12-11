#pragma once
#include "ImpBase.h"

typedef enum
{
	DUT_CON_AUTO,
	DUT_CON_UART,
	DUT_CON_SOCKET,
}DUT_CON_T;

//////////////////////////////////////////////////////////////////////////
class COpenDut : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COpenDut)
public:
    COpenDut(void);
    virtual ~COpenDut(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    RM_MODE_ENUM    m_eMode;
    DUT_CON_T       m_eType;
};
