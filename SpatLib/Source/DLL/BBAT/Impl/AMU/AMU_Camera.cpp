#include "Stdafx.h"
#include "AMU_Camera.h"

IMPLEMENT_RUNTIME_CLASS(CAMU_Camera)

CAMU_Camera::CAMU_Camera(void)
{

}

CAMU_Camera::~CAMU_Camera(void)
{
}

SPRESULT CAMU_Camera::__PollAction(void)
{
	CHKRESULT(CameraOpen());	
	Sleep(1000);
	if (m_Photo)
	{
		CHKRESULT(CameraRead());
		Sleep(1000);
		CHKRESULT(SaveFile());
	}
	
	CHKRESULT(CameraClose());
	
	return SP_OK;		
}

BOOL CAMU_Camera::LoadXMLConfig(void)
{
	m_Photo = GetConfigValue(L"Option:Photo", 0);
	return TRUE;
}

SPRESULT CAMU_Camera::CameraOpen()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CAMERA_SPI;
	CameraValue DataSend;
	DataSend.iOperate = BBAT_MIPI_CAMERA::OPEN;

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, Timeout_PHONECOMMAND_BBAT), "BBAT Camera Test: Camera Open Fail",
		LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Camera Test: Camera Open Pass", LEVEL_ITEM);
	return SP_OK;
}

SPRESULT CAMU_Camera::CameraRead()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CAMERA_SPI;
	CameraValue DataSend;
	DataSend.iOperate = BBAT_MIPI_CAMERA::READ;

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, Timeout_PHONECOMMAND_BBAT), "BBAT Camera Test: Camera Read Fail",
		LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Camera Test: Camera Read Pass", LEVEL_ITEM);
	return SP_OK;

}

SPRESULT CAMU_Camera::SaveFile()
{
	TCHAR m_szCameraPicPath[_MAX_PATH];
	TCHAR m_szSModulePath[_MAX_PATH];
	DWORD dwRet = ::GetModuleFileName(nullptr, m_szSModulePath, _MAX_PATH);
	if (!dwRet)
	{
		return SP_E_BBAT_BASE_ERROR;
	}

	TCHAR* pResult = _tcsrchr(m_szSModulePath, _T('\\'));
	if (pResult == nullptr)
	{
		return SP_E_BBAT_BASE_ERROR;
	}
	*pResult = 0;

	_tcscpy_s(m_szCameraPicPath, m_szSModulePath);
	_tcscat_s(m_szCameraPicPath, _T("\\CameraData.bmp"));

	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CAMERA_SPI;
	unsigned char szRecvATV[10000];
	
	char* m_pnItemvalue;
	m_pnItemvalue = new char[8192 * 1000]; //8192*1000
	memset(m_pnItemvalue, 0, sizeof(char) * (8192 * 1000));

	CameraValue bySend;
	bySend.iOperate = BBAT_MIPI_CAMERA::SEND_CAMERA_DATA;
	int iTemp;
	int bPass = 0;
	for (iTemp = 0; iTemp < 1000; iTemp++)
	{
		memset(szRecvATV, 0xff, sizeof(szRecvATV));
		SPRESULT nStatus = SP_AutoBBTest(m_hDUT, SubType, &bySend, sizeof(bySend), szRecvATV, 8192, Timeout_PHONECOMMAND_BBAT);
		if (SP_OK != nStatus)
		{
			wchar_t s[50];
			swprintf(s,50, L"收到fail命令，已经接收了%d条命令", iTemp);
			LPCWSTR strMsg(s);
			bPass = 0;
			MessageBox(nullptr, strMsg, L"提示", NULL);
			break;
		}
		else
		{
			wchar_t s[50];
			swprintf_s(s, L"收到pass命令，已经接收了%d条命令", iTemp + 1);
			LPCWSTR strMsg(s);
			bPass = 1;
			if (iTemp == 999)
				MessageBox(nullptr, strMsg, L"提示", NULL);
		}

		for (int iCount = 0; iCount < 8192; iCount++)
		{
			m_pnItemvalue[iTemp * 8192 + iCount] = szRecvATV[iCount];
		}
	}
	::DeleteFile(m_szCameraPicPath);

	HANDLE hFile;
	hFile = CreateFile(m_szCameraPicPath, GENERIC_WRITE,
		0,
		nullptr,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (!hFile)
	{
		if (NULL != m_pnItemvalue)
		{
			delete[] m_pnItemvalue;
			m_pnItemvalue = NULL;
		}
		return SP_OK; //open fail
	}
	DWORD Written;

    BOOL bWrite = WriteFile(hFile, m_pnItemvalue, 8192 * (iTemp + bPass), &Written, nullptr);

	if (NULL != m_pnItemvalue)
	{
		delete[] m_pnItemvalue;
		m_pnItemvalue = NULL;
	}

    if (!bWrite)
    {
        LogFmtStrA(SPLOGLV_ERROR, "WriteFile failed, Windows Error = 0x%X", GetLastError());
        CloseHandle(hFile);
        return SP_E_BBAT_BASE_ERROR;
    }

	CloseHandle(hFile);

	MessageBox(nullptr, L"写文件完成,请在工具根目录查找文件", L"提示", NULL);

	return SP_OK;
}

SPRESULT CAMU_Camera::CameraClose()
{
	DEVICE_AUTOTEST_ID_E SubType = DEVICE_AUTOTEST_CAMERA_SPI;
	CameraValue DataSend;
	DataSend.iOperate = BBAT_MIPI_CAMERA::CLOSE;

	CHKRESULT(PrintErrorMsg(SP_AutoBBTest(m_hDUT, SubType, &DataSend, sizeof(DataSend),
		nullptr, 0, Timeout_PHONECOMMAND_BBAT), "BBAT Camera Test: Camera Close Fail",
		LEVEL_ITEM));

	PrintSuccessMsg(SP_OK, "BBAT Camera Test: Camera Close Pass", LEVEL_ITEM);
	return SP_OK;
}
