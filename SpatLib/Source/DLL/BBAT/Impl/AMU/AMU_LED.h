#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CAMU_LED :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAMU_LED)

public:
	CAMU_LED(void);
	virtual ~CAMU_LED(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

	struct RGB_Light
	{
		int8 MISCIndex;
		int8 Color;
		int8 Operate;
	};

	enum BBAT_MISC
	{
		LCDBackLight = 0,
		Reserved = 1,
		KeypadBackLight = 2,
		Vibrator = 3,
		FlashLight = 4,
		OTG = 5,
		HeadSetDetect = 6,
		COB = 7,
		FrontFlashLight = 8,
		RGBLight = 9,
		NTC = 10,
		Torch = 11,
		FingerPrint = 12,
		NFC = 13,
		RGBShine = 33
	};

	


	enum RGB_COLOR
	{
		RED,
		GREEN,
		BLUE
	};

	enum RGB_OPERATE
	{
		CLOSE,
		OPEN
	};

private:
	SPRESULT Shine();
	SPRESULT Close();
	SPRESULT OpenGreen();
};