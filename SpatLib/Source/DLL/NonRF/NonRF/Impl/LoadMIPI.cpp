#include "StdAfx.h"
#include "LoadMIPI.h"
#include "ModeSwitch.h"
#include <vector>



IMPLEMENT_RUNTIME_CLASS( CLoadMIPI )

CLoadMIPI::CLoadMIPI( void )
{
    SetupDUTRunMode( RM_WCDMA_CALIBRATION_MODE );
    
}

CLoadMIPI::~CLoadMIPI( void )
{

}

SPRESULT CLoadMIPI::__PollAction( void )
{
    //发送WCDMAActive指令，读取MIPI必须做初始化。
    //判断WCDMAActive指令是否发送成功，若成功，则继续执行，若失败，则退出；
    
    CHKRESULT( SP_wcdmaActive( m_hDUT, TRUE ) );
    
    std::string strBuf = "";
    
    //AT指令循环3次，中间有5s timeout，若3次均失败，界面打印测试NG，返回错误代码；
    
    std::vector<int8> szBuf( 4096 );
    uint32 u32recvSize = 0;
    SPRESULT res = SP_SendATCommand( m_hDUT, "AT+SPCHKMIPI?", TRUE, szBuf.data(), szBuf.size(), &u32recvSize, TIMEOUT_5S );
    strBuf = szBuf.data();
        
    if ( SP_OK != res || NULL != strstr( strBuf.c_str(), "+CME ERROR" ) )
    {
        LogFmtStrA( SPLOGLV_VERBOSE, "AT response Fail" );
            
        NOTIFY( "LoadMIPI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Load MIPI fail" );
        return SP_E_SPAT_MIPI_LOAD_ERROR;
    }
        
    replace_all( strBuf, "\r", "" );
    replace_all( strBuf, "\n", "" );
    int nStrBufLength = strBuf.size();
        
    //判断返回值是否正常。若返回值长度低于10，则说明返回的并非正常MIPI值。正常MIPI值大于为5组数据，最小为13；
    if ( strBuf.size() < 10 )
    {
        NOTIFY( "LoadMIPI", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Invalid response,NULL: %s", strBuf.c_str() );
        return  SP_E_SPAT_MIPI_INVALID_MIPI;
    }
    
    //GetTokenStringA数据处理结果lpString 为去掉所有“，”的字符串数组
    int nCount = 0;
    //std::string strBuf = "0000,0003,7574,7834,0000,0001,7468,7834,0000,0001,7468,7834,998OK";//---打桩测试
    replace_all( strBuf, "OK", "" );
    LPSTR* lppString = CUtility::GetTokenStringA( strBuf.c_str(), ",", nCount );
    
    std::vector<std::string> arrString;
    for ( int i = 0; i < nCount; i++ )
    {
        arrString.push_back( lppString[i] );
    }
    
    //异常处理，若返回数据不足一组(一组数据4+1)，则报错；
    if ( nCount != ( arrString.back().size() * 4 + 1 ) )
    {
        NOTIFY( "LoadMIPI", LEVEL_ITEM, 1, 0, 1,
                NULL, -1, NULL,
                "Invalid response,NULL: %s",
                strBuf.c_str() );
        return  SP_E_SPAT_MIPI_INVALID_MIPI;
    }
    else
    {
        BOOL bResult = TRUE;
        for ( int i = 0; ( i + 3 ) < nCount; i = i + 4 )
        {
            MIPI* pMipi;
            pMipi = ( MIPI* )lppString[i];
            
            LogFmtStrA( SPLOGLV_VERBOSE, "PORT:0x%X | USID:0x%X | PID:0x%X | MID:0x%X",
                        atoi( pMipi->iPort ),
                        atoi( pMipi->iUsID ),
                        atoi( pMipi->iPID ),
                        atoi( pMipi->iMID ) );
                        
            LogFmtStrA( SPLOGLV_VERBOSE, "Result:%c", arrString.back()[i / 4] ); //strResult length=i/4;
            
            bResult = bResult && ( arrString.back()[i / 4] != '9' );
            //界面打印
            _UiSendMsg( "LoadMIPI", LEVEL_ITEM, 1, ( arrString.back()[i / 4] != '9' ), 1,
                        NULL, -1, NULL,
                        "PORT:0x%X | USID:0x%X | PID:0x%X | MID:0x%X",
                        atoi( pMipi->iPort ),
                        atoi( pMipi->iUsID ),
                        atoi( pMipi->iPID ),
                        atoi( pMipi->iMID ) );
        }
        
        if ( !bResult )
        {
            return SP_E_FAIL;
        }
    }
    return SP_OK;
}
