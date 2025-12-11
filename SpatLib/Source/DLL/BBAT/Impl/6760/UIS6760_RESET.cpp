#include "StdAfx.h"
#include "UIS6760_RESET.h"

//
IMPLEMENT_RUNTIME_CLASS(CUIS6760_RESET)

//////////////////////////////////////////////////////////////////////////
CUIS6760_RESET::CUIS6760_RESET(void)
: m_dwUartNo(0)
, m_dwBaudrate(115200)
{
}

CUIS6760_RESET::~CUIS6760_RESET(void)
{
}
//#define LPNO1_OPEN[]  {0xaa,0x01,0x01}
#define LPNO1_CLOSE " aa 01 00\r\n"
#define LPNO1_QUERY " dd 01\r\n"
#define LPNO2_OPEN " aa 02 01\r\n"
#define LPNO2_CLOSE " aa 02 00\r\n"
#define LPNO2_QUERY " dd 02\r\n"

#define QUERY_NO1_OPEN "01 01"
#define QUERY_NO1_CLOSE "01 00"
#define QUERY_NO2_OPEN "02 01"
#define QUERY_NO2_CLOSE "02 00"


//和软件协商,测试命令为：
//上位机发 aa 01 01 控制1号继电器吸合
//上位机发 aa 01 00 控制1号继电器断开
//上位机发 aa 02 01 控制2号继电器吸合
//上位机发 aa 02 00 控制2号继电器断开
//上位机发 dd 01 查询1号继电器状态 返回01 01表示继电器1吸合, 01 00 表示继电器断开
//上位机发 dd 02 查询2号继电器状态 返回02 01表示继电器1吸合, 02 00 表示继电器断开

SPRESULT CUIS6760_RESET::__PollAction(void)
{
	ICommChannel* pCom = NULL;
	SPRESULT res = SP_OK;	
	if (!CreateChannel(&pCom, CHANNEL_TYPE_COM) || (NULL == pCom))
    {
        LogFmtStrA(SPLOGLV_ERROR, "Create COM channel fail");
        return SP_E_FAIL;
    }
	CHANNEL_ATTRIBUTE ca;
	ca.ChannelType    = CHANNEL_TYPE_COM;
	ca.Com.dwPortNum  = m_dwUartNo;
	ca.Com.dwBaudRate = m_dwBaudrate;
	//channel open
	pCom->Close();
	if (!pCom->Open(&ca))
	{
		ReleaseChannel(pCom);
		NOTIFY("OpenUART", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,"Port = %d; Baudrate = %d bps", m_dwUartNo, m_dwBaudrate);   
		return SP_E_BBAT_OPEN_CHANNEL_FAIL;
	}
	else
	{
		NOTIFY("OpenUART", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL,"Port = %d; Baudrate = %d bps", m_dwUartNo, m_dwBaudrate); 
	}
	
		pCom->Clear();
        unsigned char open[] = { 170,1,1 };
		unsigned char close[] = { 170,1,0 };
		res = Send(pCom, open);
		pCom->Clear();
		Sleep(3000);
		res = Send(pCom, close);
		res = Send(pCom, close);

	pCom->Close();
	ReleaseChannel(pCom);
	pCom = NULL;
	NOTIFY("RESET_Test", LEVEL_ITEM, 1,(SP_OK != res) ? 0 : 1, 1, NULL, -1, NULL, NULL); 
	return SP_OK;
}
BOOL CUIS6760_RESET::LoadXMLConfig(void)
{
	m_dwUartNo = GetConfigValue(L"Option:UART", 0);
	m_dwBaudrate = GetConfigValue(L"Option:Baudrate", 9600);

	if (m_dwBaudrate <= 0)
    {
        return FALSE;
    }
	return TRUE;
}

SPRESULT CUIS6760_RESET::Send(ICommChannel* pCom, unsigned char* lpData)
{
	CONST DWORD SIZE = sizeof(lpData);
	DWORD dwWrite = pCom->Write((LPVOID)lpData, SIZE);
	Sleep(200);
	//if(0 == dwWrite ) //比如想写100个字符，结果uart出现异常只写了80个字符，接dwWrite=80，不等于0，所以只用0判断失败，不准确
	if (dwWrite != SIZE)  
	{
        NOTIFY("RESET_Send", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Send %d bytes", dwWrite); 
		return SP_E_BBAT_UART_WRITE_FAIL;
	}
	return SP_OK;
}

SPRESULT CUIS6760_RESET::RecvAndCompare(ICommChannel* pCom,LPCSTR lpCompareData)
{
	CHAR szRevBuf[100] = {0};

	memset(szRevBuf,0,sizeof(szRevBuf));
	DWORD dwRead = pCom->Read(szRevBuf, sizeof(szRevBuf)-1, TIMEOUT_3S);

	if ( 0 == dwRead )
	{
        NOTIFY("RESET_Recv", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "0 bytes is read"); 
		return SP_E_BBAT_UART_READ_FAIL;
	}

	//此处不能用strstr，因为从串口读到的数据，开始时可能有脏数据，比如恰好有数据0，之后才是“01 01”，则strstr遇到0就认为字符串结束了。
	//这里可以用std::search，或者for和memcmp组合来实现搜索。
	char* pFind = szRevBuf;
	bool bFind = false;
	for(int i = 0;i<(int)dwRead;i++)
	{
		if(memcmp(pFind,lpCompareData,strlen(lpCompareData)) == 0)
		{
			bFind = true; //找到了
		}
		pFind++;
	}

	if(bFind)
	{
		NOTIFY("RESET_Recv", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);  //pass
	}
	else
	{		
		NOTIFY("RESET_Recv", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);   
		return SP_E_BBAT_UART_VALUE_FAIL;
	}
	return SP_OK;
}
