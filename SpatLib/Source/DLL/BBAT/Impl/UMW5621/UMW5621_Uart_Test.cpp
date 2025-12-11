#include "StdAfx.h"
#include "UMW5621_Uart_Test.h"

//
IMPLEMENT_RUNTIME_CLASS(CUMW5621_Uart_Test)

//////////////////////////////////////////////////////////////////////////
CUMW5621_Uart_Test::CUMW5621_Uart_Test(void)
: m_dwUartNo(0)
, m_dwBaudrate(115200)
{
}

CUMW5621_Uart_Test::~CUMW5621_Uart_Test(void)
{
}

SPRESULT CUMW5621_Uart_Test::__PollAction(void)
{
	ICommChannel* pCom = NULL;
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
	
	if (!pCom->Clear())
    {
        LogFmtStrA(SPLOGLV_ERROR, "Clear Channel fail");
        return SP_E_FAIL;
    }


	//和软件协商用hello hello传输，返回hello world字符作为判断
    LPCSTR STRING = "hello hello\r\n";
	CONST DWORD SIZE = strlen(STRING);

	DWORD dwWrite = pCom->Write((LPVOID)STRING, SIZE);
	//if(0 == dwWrite ) //比如想写100个字符，结果uart出现异常只写了80个字符，接dwWrite=80，不等于0，所以只用0判断失败，不准确
	if (dwWrite != SIZE)  
	{
        NOTIFY("SendUART", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Send %d bytes", dwWrite); 
		pCom->Close();
		ReleaseChannel(pCom);
		return SP_E_BBAT_UART_WRITE_FAIL;
	}

	//等待时间短了只能收到一部分字符,如果不sleep，下面会收到h he hel类似这样的字符就返回了
	Sleep(500);

    CHAR szRevBuf[100] = {0};

	memset(szRevBuf,0,sizeof(szRevBuf));
	DWORD dwRead = pCom->Read(szRevBuf, sizeof(szRevBuf)-1, TIMEOUT_3S);
    pCom->Close();
    ReleaseChannel(pCom);
    pCom = NULL;

	if ( 0 == dwRead )
	{
        NOTIFY("RecvUART", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "0 bytes is read"); 
		return SP_E_BBAT_UART_READ_FAIL;
	}

	//此处不能用strstr，因为从串口读到的数据，开始时可能有脏数据，比如敲好有数据0，之后才是“hello world”，则strstr遇到0就认为字符串结束了。
	//这里可以用std::search，或者for和memcmp组合来实现搜索。
	char* pFind = szRevBuf;
	bool bFind = false;
	for(int i = 0;i<(int)dwRead;i++)
	{
		if(memcmp(pFind,"hello world",strlen("hello world")) == 0)
		{
			bFind = true; //找到了
		}
		pFind++;
	}

	if(bFind)
	{
		NOTIFY("CheckUART", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);  //pass
	}
	else
	{		
		NOTIFY("CheckUART", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);   
		return SP_E_BBAT_UART_VALUE_FAIL;
	}

	//原来的代码
	/*if (strstr(szRevBuf, "hello world") == NULL)
	{
		NOTIFY("CheckUART", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);   
		return SP_E_BBAT_UART_VALUE_FAIL;
	}
	else
	{
		NOTIFY("CheckUART", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL,  "RecvData = %s", szRevBuf);  
	}*/

    return SP_OK;
}
BOOL CUMW5621_Uart_Test::LoadXMLConfig(void)
{
	m_dwUartNo = GetConfigValue(L"Option:UART", 0);
	m_dwBaudrate = GetConfigValue(L"Option:Baudrate", 115200);

	return TRUE;
}


	
