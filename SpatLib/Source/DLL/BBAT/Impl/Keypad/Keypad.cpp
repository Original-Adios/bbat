#include "StdAfx.h"
#include "Keypad.h"

//
IMPLEMENT_RUNTIME_CLASS(CKeypad)

//////////////////////////////////////////////////////////////////////////
CKeypad::CKeypad(void)
{
}

CKeypad::~CKeypad(void)
{
}

SPRESULT CKeypad::__PollAction(void)
{
	FUNBOX_INIT_CHECK();
    return KeypadAction();
}
BOOL CKeypad::LoadXMLConfig(void)
{
	m_bPowerKey = GetConfigValue(L"Option:PowerKey:Enable", 0);
	m_bPowerKey2 = GetConfigValue(L"Option:PowerKey2:Enable", 0);
	m_bFpPowerKey = GetConfigValue(L"Option:PowerKey:FeaturePhone", 0);
	m_bSideKey = GetConfigValue(L"Option:SideKey:Enable", 0);

	m_nPowerKeySpec = GetConfigValue(L"Option:PowerKey:KeyValueSpec", 0);
	m_nPowerKey2Spec = GetConfigValue(L"Option:PowerKey2:KeyValueSpec", 0);
	m_nSideKeySpec = GetConfigValue(L"Option:SideKey:KeyValueSpec", 0);

	return TRUE;
}

SPRESULT CKeypad::KeypadAction()
{
	CHKRESULT(KeypadOpen());

	if (m_bPowerKey)
	{
		CHKRESULT(RunPowerKey());
		CHKRESULT(KeypadRead(m_nPowerKeySpec, m_bFpPowerKey));
	}
	if (m_bPowerKey2)
	{
		CHKRESULT(RunPowerKey2());
		CHKRESULT(KeypadRead(m_nPowerKey2Spec, FALSE));
	}
	if (m_bSideKey)
	{
		CHKRESULT(RunSideKey());
		CHKRESULT(KeypadRead(m_nSideKeySpec, FALSE));
	}
	CHKRESULT(KeypadClose());
	return SP_OK;
}

SPRESULT CKeypad::KeypadOpen()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_KeypadOpen(m_hDUT),
		"Keypad Open", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "Phone: Keypad Open", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CKeypad::KeypadClose()
{
	CHKRESULT(PrintErrorMsg(SP_BBAT_KeypadClose(m_hDUT),
		"Keypad Close", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "Phone: Keypad Close", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CKeypad::RunPowerKey()
{
	RETURNSPRESULT(m_pObjFunBox->FB_PowerKey_Enable_X(),"G3: Power Key On");
	Sleep(100);
	RETURNSPRESULT(m_pObjFunBox->FB_PowerKey_Disable_X(), "G3: Power Key Off");

	return SP_OK;
}

SPRESULT CKeypad::RunPowerKey2()
{
	RETURNSPRESULT(m_pObjFunBox->FB_PowerKey2_Enable_X(), "G3: Power Key On");
	Sleep(100);
	RETURNSPRESULT(m_pObjFunBox->FB_PowerKey2_Disable_X(), "G3: Power Key Off");

	return SP_OK;
}

SPRESULT CKeypad::RunSideKey()
{
	RETURNSPRESULT(m_pObjFunBox->FB_Side_Key_Enable_X(), "G3: Side Key On");
	Sleep(100);	
	RETURNSPRESULT(m_pObjFunBox->FB_Side_Key_Disable_X(), "G3: Side Key Off");
	return SP_OK;
}

SPRESULT CKeypad::KeypadRead(int nSpec, BOOL bFp)
{
	if (bFp)
	{
		CHKRESULT(PrintErrorMsg(
			SP_BBAT_FP_KeypadRead(m_hDUT),
			"Phone: FP Keypad Read", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Phone: FP Keypad Read", LEVEL_ITEM);
	}
	else
	{
		int nKeyValue = 0;
		CHKRESULT(PrintErrorMsg(SP_BBAT_KeypadRead(m_hDUT, &nKeyValue),
			"Phone: Keypad Read", LEVEL_ITEM));
		PrintSuccessMsg(SP_OK, "Phone: Keypad Read", LEVEL_ITEM);

		_UiSendMsg("Keypad Read", LEVEL_ITEM,
			nSpec, nKeyValue, nSpec,
			nullptr, -1, nullptr);
		if (nSpec != nKeyValue)
		{
			return SP_E_BBAT_BT_VALUE_FAIL;
		}
	}
	return SP_OK;
}
