#include"Stdafx.h"
#include "AMU_Sensor_Calibration.h"

IMPLEMENT_RUNTIME_CLASS(CAMU_Sensor_Calibration)

CAMU_Sensor_Calibration::CAMU_Sensor_Calibration(void)
{
	m_bAccelerometer = TRUE;	//加速度
	m_bMagnetic = TRUE;			//磁力
	m_bGyroscope = TRUE;		//陀螺仪
}

CAMU_Sensor_Calibration::~CAMU_Sensor_Calibration(void)
{
}

SPRESULT CAMU_Sensor_Calibration::__PollAction(void)
{
	MessageBox(nullptr, L"进行传感器校准测试，请将主板放置水平", L"提示", NULL);

	if (m_bAccelerometer) CHKRESULT(AccCalibration());
	if (m_bMagnetic) CHKRESULT(MagCalibration());
	if (m_bGyroscope) CHKRESULT(GyrCalibration());
	
	return SP_OK;
}

BOOL CAMU_Sensor_Calibration::LoadXMLConfig(void)
{
	m_bAccelerometer = 1 == GetConfigValue(L"Option:Accelerometer", 0) ? TRUE : FALSE;
	m_bGyroscope = 1 == GetConfigValue(L"Option:Gyroscope", 0) ? TRUE : FALSE;
	m_bMagnetic = 1 == GetConfigValue(L"Option:Magnetic", 0) ? TRUE : FALSE;
	return TRUE;
}

SPRESULT CAMU_Sensor_Calibration::AccCalibration()
{
	char byRecv[128] = { 0 };
	memset(&byRecv, NULL, sizeof(byRecv));
	CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SENSORCALI=1,1,1", TRUE, byRecv, sizeof(byRecv), nullptr, 10000),
		"BBAT Sensor Test: Accelerometer Sensor Calibration Fail",
		LEVEL_ITEM));
	string strReturn = byRecv;
	_UiSendMsg("Calibration", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr, "%c", byRecv[0]);
	if (-1 == strReturn.find("+SENSORCALI:ERROR"))
	{
		PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Accelerometer Sensor Calibration Pass", LEVEL_ITEM);
		return SP_OK;
	}

	PrintErrorMsg(SP_E_BBAT_VALUE_FAIL,
		"Accelerometer Sensor Calibration ERROR", LEVEL_ITEM);
	return SP_E_BBAT_CMD_FAIL;	
}

SPRESULT CAMU_Sensor_Calibration::MagCalibration()
{
	char byRecv[128] = { 0 };
	memset(&byRecv, NULL, sizeof(byRecv));
	CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SENSORCALI=1,1,2", TRUE, byRecv, sizeof(byRecv), nullptr, 10000),
		"BBAT Sensor Test: Magnetic Sensor Calibration Fail",
		LEVEL_ITEM));
	string strReturn = byRecv;
	if (-1 == strReturn.find("+SENSORCALI:ERROR"))
	{
		PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Magnetic Sensor Calibration Pass", LEVEL_ITEM);
		return SP_OK;
	}
	PrintErrorMsg(SP_E_BBAT_VALUE_FAIL,
		"Magnetic Sensor Calibration ERROR", LEVEL_ITEM);
	return SP_E_BBAT_BASE_ERROR;
}

SPRESULT CAMU_Sensor_Calibration::GyrCalibration()
{
	char byRecv[128] = { 0 };
	memset(&byRecv, NULL, sizeof(byRecv));
	CHKRESULT(PrintErrorMsg(SP_SendATCommand(m_hDUT, "AT+SENSORCALI=1,1,4", TRUE, byRecv, sizeof(byRecv), nullptr, 10000),
		"BBAT Sensor Test: Gyroscope Sensor Calibration Fail",
		LEVEL_ITEM));

	string strReturn = byRecv;
	if (-1 == strReturn.find("+SENSORCALI:ERROR"))
	{
		PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Gyroscope Sensor Calibration Pass", LEVEL_ITEM);
		return SP_OK;
	}
	PrintErrorMsg(SP_E_BBAT_VALUE_FAIL,
		"Gyroscope Sensor Calibration ERROR", LEVEL_ITEM);
	return SP_E_BBAT_BASE_ERROR;	
}
