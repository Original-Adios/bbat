
#include "stdafx.h"
#include "AMU_Sensor_Test.h"


IMPLEMENT_RUNTIME_CLASS(CAMU_Sensor_Test)

CAMU_Sensor_Test::CAMU_Sensor_Test(void)
{
	m_bAccelerometer = TRUE;	//加速度
	m_bMagnetic = TRUE;			//磁力
	m_bGyroscope = TRUE;		//陀螺仪
	m_bLight = TRUE;			//距离光

}

CAMU_Sensor_Test::~CAMU_Sensor_Test(void)
{
}

SPRESULT CAMU_Sensor_Test::__PollAction(void)
{
	if (m_bAccelerometer) CHKRESULT(RunAcc());
	if (m_bMagnetic) CHKRESULT(RunMag());
	if (m_bGyroscope) CHKRESULT(RunGyr());
	if (m_bLight) CHKRESULT(RunLight());

	return SP_OK;
}

BOOL CAMU_Sensor_Test::LoadXMLConfig(void)
{
	m_bAccelerometer = 1 == GetConfigValue(L"Option:Acc_Sensor:Accelerometer", 0) ? TRUE : FALSE;
	m_bLight = 1 == GetConfigValue(L"Option:Light_Sensor:Light_Int", 0) ? TRUE : FALSE;
	m_bGyroscope = 1 == GetConfigValue(L"Option:Gyr_Sensor:Gyroscope", 0) ? TRUE : FALSE;
	m_bMagnetic = 1 == GetConfigValue(L"Option:Mag_Sensor:Magnetic", 0) ? TRUE : FALSE;

	std::string x = _W2CA(GetConfigValue(L"Option:Acc_Sensor:X_Axis", L""));
	std::string y = _W2CA(GetConfigValue(L"Option:Acc_Sensor:Y_Axis", L""));
	std::string z = _W2CA(GetConfigValue(L"Option:Acc_Sensor:Z_Axis", L""));
	std::string offset = _W2CA(GetConfigValue(L"Option:Acc_Sensor:Offset", L""));

	m_AccX = atof(x.c_str());
	m_AccY = atof(y.c_str());
	m_AccZ = atof(z.c_str());
	m_Offset = atof(offset.c_str());

	std::string n = _W2CA(GetConfigValue(L"Option:Dis_Sensor:Noise_Value", L""));
	std::string i = _W2CA(GetConfigValue(L"Option:Int_Sensor:Int_Value", L""));

	m_NoiseVal = atoi(n.c_str());
	m_IntVal = atoi(i.c_str());

	return TRUE;
}

SPRESULT CAMU_Sensor_Test::RunAcc()
{
	//打开加速Sensor

	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_SENSOR;

	SensorValue DataSend;
	DataSend.SensorTest = SPECIAL;
	DataSend.iSensorID = BBAT_SENSOR_SPECIAL::Accelerometer_Sensor;
	DataSend.Operation = BBAT_SENSOR_OPERATE::OPEN;

	Data_Acc DataRecv;
	memset(&DataRecv, NULL, sizeof(DataRecv));

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		&DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT), "BBAT Sensor Test: Accelerometer Sensor Open Fail",
		LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Accelerometer Sensor Open Pass", LEVEL_ITEM);
	//Need Check
	CHKRESULT(PrintErrorMsg(CheckAcc(DataRecv), "Angle Fail", LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "Degree Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::RunMag()
{
	//打开磁力Sensor
	MessageBox(nullptr, L"点击确定按钮开始测试，并同时晃动产品", L"提示", NULL);
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_SENSOR;
	SensorValue DataSend;
	DataSend.SensorTest = SPECIAL;
	DataSend.iSensorID = BBAT_SENSOR_SPECIAL::Magnetic_Sensor;
	DataSend.Operation = BBAT_SENSOR_OPERATE::OPEN;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, 20000), "BBAT Sensor Test: Magnetic Sensor Open Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Magnetic Sensor Open Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::RunGyr()
{
	//打开陀螺仪Sensor
	MessageBox(nullptr, L"点击确定按钮开始测试，并同时晃动产品", L"提示", NULL);
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_SENSOR;
	SensorValue DataSend;
	DataSend.SensorTest = SPECIAL;
	DataSend.iSensorID = BBAT_SENSOR_SPECIAL::Gyroscope_Sensor;
	DataSend.Operation = BBAT_SENSOR_OPERATE::OPEN;
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, 20000), "BBAT Sensor Test: Gyroscope Sensor Open Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Gyroscope Sensor Open Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::RunLight()
{
	MessageBox(nullptr, L"请移除传感器的遮挡,并点击确定按钮开始测试", L"提示", NULL);
	PrintSuccessMsg(SP_OK, "BBAT Faraway Test", LEVEL_ITEM);
	CHKRESULT(RunLightDis(FARAWAY));
	CHKRESULT(RunLightInt(FARAWAY));
	PrintSuccessMsg(SP_OK, "BBAT Faraway Test Pass", LEVEL_ITEM);

	MessageBox(nullptr, L"请遮挡传感器，并点击确定按钮开始测试", L"提示", NULL);
	PrintSuccessMsg(SP_OK, "BBAT Nearby Test", LEVEL_ITEM);
	CHKRESULT(RunLightDis(NEARBY));
	CHKRESULT(RunLightInt(NEARBY));
	PrintSuccessMsg(SP_OK, "BBAT Nearby Test Pass", LEVEL_ITEM);
	return SPRESULT();
}

SPRESULT CAMU_Sensor_Test::RunLightDis(int DisLevel)
{
	//打开距离光测试
//	MessageBox(nullptr, L"请移除传感器的遮挡,并点击确定按钮开始测试", L"提示", NULL);

	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_SENSOR;
	SensorValue DataSend;
	DataSend.SensorTest = SPECIAL;
	DataSend.iSensorID = BBAT_SENSOR_SPECIAL::Light_Dis_Sensor;
	DataSend.Operation = BBAT_SENSOR_OPERATE::OPEN;
	Data_Distance DataRecv;

	//Need Check Faraway
	memset(&DataRecv, NULL, sizeof(DataRecv));
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		&DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT), "BBAT Sensor Test: Light_Dis Sensor Open Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Light_Dis Sensor Open Pass", LEVEL_ITEM);
	_UiSendMsg("BBAT Sensor Test: Light_Dis Sensor", LEVEL_ITEM, DisLevel, DataRecv.Distance_Level, DisLevel, nullptr, -1, nullptr,
		"Distence Level is %d", DataRecv.Distance_Level);
	
	if (DisLevel == FARAWAY)
	{
		_UiSendMsg("BBAT Sensor Test: Light_Dis Sensor", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
			"Noise Value is %d", DataRecv.Noise_Val);
	}
	CHKRESULT(PrintErrorMsg(CheckLightDis(DataRecv, DisLevel),
		"BBAT Sensor Test: Light_Dis Sensor Fail", LEVEL_ITEM));
	
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Light_Dis Sensor Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::RunLightInt(int DisLevel)
{
	//打开光强度测试
//	MessageBox(nullptr, L"请移除传感器的遮挡,并点击确定按钮开始测试", L"提示", NULL);

	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_SENSOR;
	SensorValue DataSend;
	DataSend.SensorTest = SPECIAL;
	DataSend.iSensorID = BBAT_SENSOR_SPECIAL::Light_Int_Sensor;
	DataSend.Operation = BBAT_SENSOR_OPERATE::OPEN;

	Data_LightIntensity DataRecv;

	//Need Check Faraway
	memset(&DataRecv, NULL, sizeof(DataRecv));
	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		&DataRecv, sizeof(DataRecv), Timeout_PHONECOMMAND_BBAT), "BBAT Sensor Test: Light_Int Sensor Open Fail",
		LEVEL_ITEM));
	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Light_Int Sensor Open Pass", LEVEL_ITEM);

	int bIntVal = DataRecv.Light_Sensor_Val_High << 8 | DataRecv.Light_Sensor_Val_Low;
	_UiSendMsg("BBAT Sensor Test: Light_Int Sensor", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"Intensity Value is %d", bIntVal);

	CHKRESULT(PrintErrorMsg(CheckLightInt(bIntVal, DisLevel),
		"BBAT Sensor Test: Light_Int Sensor Fail", LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Sensor Test: Light_Int Sensor Pass", LEVEL_ITEM);

	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::CheckLightDis(Data_Distance DataRecv, int DisLevel)
{
	if (DataRecv.Distance_Level != DisLevel)
	{
		/*_UiSendMsg("Distance Test Fail", LEVEL_ITEM, 1, 0, 1, nullptr, -1, nullptr,
			"Distance Level is %d", DataRecv.Distance_Level);*/
		return SP_E_BBAT_VALUE_FAIL;
	}

	if (DataRecv.Distance_Level == FARAWAY && DataRecv.Noise_Val < m_NoiseVal)
	{
		_UiSendMsg("Distance Test Faraway Fail..", LEVEL_ITEM, m_NoiseVal, DataRecv.Noise_Val, INT_MAX, nullptr, -1, nullptr,
			"Noise Value is %d, Less Than %d", DataRecv.Noise_Val, m_NoiseVal);
		return SP_E_BBAT_VALUE_FAIL;
	}

	return SP_OK;
}

SPRESULT CAMU_Sensor_Test::CheckLightInt(int IntVal, int DisLevel)
{

	if (DisLevel == FARAWAY && IntVal > m_IntVal)
	{
		/*_UiSendMsg("Faraway Test Success.", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
			"Light Intensity is %d", m_IntVal);*/
		return SP_OK;
	}
	if (DisLevel == NEARBY && IntVal == 0)
	{
		/*PrintSuccessMsg(SP_OK, "Nearby Test Success", LEVEL_ITEM);*/
		return SP_OK;
	}
	return SP_E_BBAT_VALUE_FAIL;
}

SPRESULT CAMU_Sensor_Test::CheckAcc(Data_Acc DataRecv)
{
	int32 X = 0, Y = 0, Z = 0;

	for (int i = 0; i < 3; i++)
	{
		X = X << 8 | DataRecv.X_Val[i];
		Y = Y << 8 | DataRecv.Y_Val[i];
		Z = Z << 8 | DataRecv.Z_Val[i];
	}

	X = DataRecv.X_Symbol == 0 ? X : 0 - X;
	Y = DataRecv.Y_Symbol == 0 ? Y : 0 - Y;
	Z = DataRecv.Z_Symbol == 0 ? Z : 0 - Z;

	/*_UiSendMsg("Axis Value", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"x = %d, y = %d, z = %d", X, Y, Z);*/
	float x = float(X) / 1000;
	float y = float(Y) / 1000;
	float z = float(Z) / 1000;

	/*_UiSendMsg("Axis Value from sensor", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"x = %.2f, y = %.2f, z = %.2f", x, y, z);

	_UiSendMsg("Axis Value from input", LEVEL_ITEM, 1, 1, 1, nullptr, -1, nullptr,
		"x = %.2f, y = %.2f, z = %.2f, offset = %.2f", m_AccX, m_AccY, m_AccZ, m_Offset);*/

	BOOL bAccPass = true;

	if (x < m_AccX - m_Offset || x > m_AccX + m_Offset)
	{
		_UiSendMsg("X Fail", LEVEL_ITEM, m_AccX - m_Offset, x, m_AccX + m_Offset, nullptr, -1, nullptr);
		bAccPass = false;
	}
	else
	{
		_UiSendMsg("X Pass", LEVEL_ITEM, m_AccX - m_Offset, x, m_AccX + m_Offset, nullptr, -1, nullptr);
	}

	if (y < m_AccY - m_Offset || y > m_AccY + m_Offset)
	{
		_UiSendMsg("Y Fail", LEVEL_ITEM, m_AccY - m_Offset, y, m_AccY - m_Offset, nullptr, -1, nullptr);
		bAccPass = false;
	}
	else
	{
		_UiSendMsg("Y Pass", LEVEL_ITEM, m_AccY - m_Offset, y, m_AccY + m_Offset, nullptr, -1, nullptr);
	}

	if (z < m_AccZ - m_Offset || z > m_AccZ + m_Offset)
	{
		_UiSendMsg("Z Fail", LEVEL_ITEM, m_AccZ - m_Offset, z, m_AccZ + m_Offset, nullptr, -1, nullptr);
		bAccPass = false;
	}
	else
	{
		_UiSendMsg("Z Pass", LEVEL_ITEM, m_AccZ - m_Offset, z, m_AccZ + m_Offset, nullptr, -1, nullptr);
	}

	if (bAccPass)
	{
		return SP_OK;
	}
	return SP_E_BBAT_VALUE_FAIL;

}
