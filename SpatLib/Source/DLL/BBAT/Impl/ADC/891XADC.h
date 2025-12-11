#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////

#define MAX_ADC_GROUP 4
#define FIRST_ADC_CHANNEL 1
#define LAST_ADC_CHANNEL 4

class C891xADC : public CImpBase
{
    DECLARE_RUNTIME_CLASS(C891xADC)
public:
    C891xADC(void);
    virtual ~C891xADC(void);

    struct ADC_Group
    {
        uint8 iChannel = 1;
        int iLowLimit = 1000;
        int iUpLimit = 3000;
    };

	struct ADCValue
	{
		int8 iChannel;
		int8 iScale;
	};
	struct ADCData
	{
		uint8 iCurrent[2];
	};



protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL LoadXMLConfig(void);



private:
    
    SPRESULT GetADC(uint8 iChannel, uint8 iScale, int& dResult);
    int m_iGroupCount = 4;
    ADC_Group m_AdcGroup[MAX_ADC_GROUP];
};


