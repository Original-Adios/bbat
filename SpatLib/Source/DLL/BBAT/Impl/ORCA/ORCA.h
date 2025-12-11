#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class COrca : public CImpBase
{
    DECLARE_RUNTIME_CLASS(COrca)
public:
    COrca(void);
    virtual ~COrca(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

private:
    SPRESULT RunOrca_RJ45();
    SPRESULT RunOrca_FT232_UART();
    SPRESULT RunOrca_EXTERNAL_IO();
    SPRESULT RunOrca_TypeC();
    SPRESULT RunOrca_ADC();

    BOOL m_bRJ45 = FALSE;
    BOOL m_bFT232 = FALSE;
    BOOL m_bEXT = FALSE;
    BOOL m_bTypeC = FALSE;
    BOOL m_bADC = FALSE;

};