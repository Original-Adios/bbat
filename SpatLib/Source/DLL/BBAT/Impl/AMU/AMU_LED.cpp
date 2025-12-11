
#include "stdafx.h"
#include "AMU_LED.h"

IMPLEMENT_RUNTIME_CLASS(CAMU_LED)

CAMU_LED::CAMU_LED(void)
{

}

CAMU_LED::~CAMU_LED(void)
{

}

SPRESULT CAMU_LED::__PollAction(void)
{
	CHKRESULT(Shine());
	Sleep(2000);

	const int res = MessageBox(nullptr, L"三色灯是否依次亮灭？", L"提示", MB_YESNO);
	SPRESULT ans;
	if (res == IDYES)
	{
		_UiSendMsg("三色灯Pass", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr);
		ans = SP_OK;
	}
	else
	{
		_UiSendMsg("三色灯Fail", LEVEL_ITEM, 0, 1, 0, nullptr, -1, nullptr);
		ans = SP_E_BBAT_VALUE_FAIL;
	}
	CHKRESULT(Close());
	Sleep(2000);
	CHKRESULT(OpenGreen());
//	Sleep(1000);
	return ans;
}

BOOL CAMU_LED::LoadXMLConfig(void)
{
	return TRUE;
}

SPRESULT CAMU_LED::Shine()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_MISC;
	RGB_Light DataSend;
	DataSend.MISCIndex = BBAT_MISC::RGBShine;

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend.MISCIndex), nullptr, 0, Timeout_PHONECOMMAND_BBAT),
		"BBAT RGB Shine Test: Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT RGB Shine Test: Pass",LEVEL_ITEM);
	
	return SP_OK;
}

SPRESULT CAMU_LED::Close()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_MISC;
	RGB_Light DataSend;
	DataSend.MISCIndex = BBAT_MISC::RGBLight;
	DataSend.Color = RGB_COLOR::RED;
	DataSend.Operate = RGB_OPERATE::CLOSE;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, Timeout_PHONECOMMAND_BBAT),
		"BBAT RGB Close: Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT RGB Close: Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CAMU_LED::OpenGreen()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_MISC;
	RGB_Light DataSend;
	DataSend.MISCIndex = BBAT_MISC::RGBLight;
	DataSend.Color = RGB_COLOR::GREEN;
	DataSend.Operate = RGB_OPERATE::OPEN;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend), nullptr, 0, Timeout_PHONECOMMAND_BBAT),
		"BBAT RGB Green Open: Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT RGB Green Open: Pass", LEVEL_ITEM);

	return SP_OK;
}


