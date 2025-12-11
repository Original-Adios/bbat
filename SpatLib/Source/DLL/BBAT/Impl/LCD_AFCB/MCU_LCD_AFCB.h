#pragma once
#include "../ImpBase.h"

//////////////////////////////////////////////////////////////////////////
enum ColorType
{
    LCD_RED = 0,
    LCD_GREEN = 1,
    LCD_BLUE = 2,
    MAX_COLOR
};

struct ColorSpec
{
    int nUpLimit;
    int nDownLimit;
    double dPercent;
};

struct ColorValue
{
    WORD wValue[MAX_COLOR];
    WORD wColorC;
    double dPercent;
};

class CMcuLcdAfcb : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CMcuLcdAfcb)
public:
	CMcuLcdAfcb(void);
	virtual ~CMcuLcdAfcb(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);


private:
    SPRESULT LcdAction();
    SPRESULT RgbGet(ColorType Color);
    SPRESULT CheckResult(ColorType Color);
    SPRESULT RgbSet();
    SPRESULT LcdRgbRed();
    SPRESULT LcdRgbBlue();
    SPRESULT LcdRgbBlack();
    SPRESULT LcdRgbSwitchCtrl();

    ColorValue m_TValue[MAX_COLOR];
    ColorSpec m_TSpec[MAX_COLOR];

    vector<string> m_arrColorName = { "ColorR: ","ColorG: ","ColorB: " };
    int nTime = 0;
    int nAmp = 0;
    int m_nSleepTime = 2000;
};