#include "StdAfx.h"
#include "SwitchDC.h"

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS(CSwitchDC)

///
CSwitchDC::CSwitchDC(void)
{
}

CSwitchDC::~CSwitchDC(void)
{
}

SPRESULT CSwitchDC::__PollAction(void)
{ 
    if (!IsFakeDC())
    {
        SP_EndPhoneTest(m_hDUT);

        CHKRESULT(m_pDCSource->SetVoltage(-1.0f));
        Sleep(200);
        CHKRESULT(m_pDCSource->SetVoltage(GetAdjParam().fVoltage));

		int32 nVbusChan = 0;
		DCS_CHANNEL_SPECIAL stChan;
		stChan.nChannelType = DC_CHANNEL_VBUS;
		if (SP_OK == m_pDCSource->GetProperty(DCP_CHANNEL_SPECIAL, NULL, &stChan))
		{
			nVbusChan = stChan.nChannel;
		}

        if (nVbusChan > 0)
        {
            m_pDCSource->SetVoltage(static_cast<float>(-1.0), DC_CHANNEL_VBUS);
            Sleep(200);
            m_pDCSource->SetVoltage(5.0, DC_CHANNEL_VBUS);
        }
    }

    return SP_OK;
}