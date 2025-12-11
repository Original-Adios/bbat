#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMisc : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CMisc)
public:
	CMisc(void);
	virtual ~CMisc(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

private:
	int m_nBackFlashCold = 0;
	int m_nBackFlashWarm = 0;
	int m_nFrontFlash = 0;
	int m_nTorch = 0;
	int m_nKeypadBackLight = 0;
	int m_nRgbRed = 0;
	int m_nRgbBlue = 0;
	int m_nRgbGreen = 0;
	int m_nVibratorOpen = 0;
	int m_nVibratorClose = 0;
	int m_nLcdWhite = 0;
	int m_nLcdRed = 0;
	int m_nLcdGreen = 0;
	int m_nLcdBlue = 0;
	int m_nLcdBlack = 0;
	int m_nUartLoop = 0;
	int m_nFlashDelayTime = 500;
	int m_nRgbDelayTime = 500;
	int m_nVibratorDelayTime = 1000;
	int m_nLcdDelayTime = 500;

	SPRESULT BackFlashCold();
	SPRESULT BackFlashWarm();
	SPRESULT FrontFlash();
	SPRESULT Torch();
	SPRESULT KeypadBackLight();
	SPRESULT RgbRed();
	SPRESULT RgbBlue();
	SPRESULT RgbGreen();
	SPRESULT VibratorOpen();
	SPRESULT VibratorClose();
	SPRESULT LcdWhite();
	SPRESULT LcdRed();
	SPRESULT LcdGreen();
	SPRESULT LcdBlue();
	SPRESULT LcdBlack();
	SPRESULT UartLoop();

};