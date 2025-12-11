#pragma once
#include "../ImpBase.h"

#define ADC_CHANNEL_MAX      32
//////////////////////////////////////////////////////////////////////////
class CGet_ADC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CGet_ADC)
public:
    CGet_ADC(void);
    virtual ~CGet_ADC(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);

    LPCWSTR m_lpwAdcChannel[ADC_CHANNEL_MAX] =
    {
        L"ADC0",
        L"ADC1",
        L"ADC2",
        L"ADC3",
        L"ADC4",
        L"ADC5",
        L"ADC6",
        L"ADC7",
        L"ADC8",
        L"ADC9",
        L"ADC10",
        L"ADC11",
        L"ADC12",
        L"ADC13",
        L"ADC14",
        L"ADC15",
        L"ADC16",
        L"ADC17",
        L"ADC18",
        L"ADC19",
        L"ADC20",
        L"ADC21",
        L"ADC22",
        L"ADC23",
        L"ADC24",
        L"ADC25",
        L"ADC26",
        L"ADC27",
        L"ADC28",
        L"ADC29",
        L"ADC30",
        L"ADC31"
    };

private:
    uint8 m_nChannel = 0;
	int m_nUpSpec = 0;
	int m_nDownSpec = 0;
};


