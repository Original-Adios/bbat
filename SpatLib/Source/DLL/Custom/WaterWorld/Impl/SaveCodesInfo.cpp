#include "StdAfx.h"
#include "SaveCodesInfo.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS( CSaveCodesInfo )

//
CSaveCodesInfo::CSaveCodesInfo()
{
}

CSaveCodesInfo::~CSaveCodesInfo()
{

}

BOOL CSaveCodesInfo::LoadXMLConfig( void )
{
    m_strLogFolder = _W2CA( GetConfigValue( L"Option:LogFolder", L"" ) );
    return TRUE;
}

SPRESULT CSaveCodesInfo::__PollAction( void )
{
    SPRESULT res = SP_OK;
    res = GetShareMemory( ShareMemory_My_UserInputSN, ( void* )&m_InputSN, sizeof( m_InputSN ) );
    if ( SP_OK != res )
    {
        NOTIFY( "GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN" );
        return res;
    }
    
    SPTEST_RESULT_T simba_result;
    res = GetShareMemory( InternalReservedShareMemory_ItemTestResult, ( void* )&simba_result, sizeof( simba_result ) );
    if ( SP_OK != res )
    {
        NOTIFY( "GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "GetShareMemory(TestResult)" );
        return res;
    }
    
    Creat_Dir( m_strLogFolder.data() );
    
    char szPath[512] = {0};
    SYSTEMTIME sTime;
    GetLocalTime( &sTime );
    
    sprintf_s( szPath, sizeof( szPath ), "%s\\%s_%s_%04d%02d%02d%02d%02d%02d.txt", m_strLogFolder.data(),
               m_InputSN[BC_SN1].szCode,
               simba_result.errCode == SP_OK ? "PASS" : "FAIL",
               sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond );
               
    FILE* pFileData = NULL;
    char szData[1024] = {0};
    do
    {
        fopen_s( &pFileData, szPath, "w" );
    } while ( pFileData == NULL );
    
    fseek( pFileData, 0, SEEK_SET );
    
    sprintf_s( szData, sizeof( szData ), "SN:%s,WIFI MAC:%s,BT MAC:%s,IMEI1:%s,IMEI2:%s,%s,%04d%02d%02d%02d%02d%02d",
               m_InputSN[BC_SN1].szCode,
               m_InputSN[BC_WIFI].szCode,
               m_InputSN[BC_BT].szCode,
               m_InputSN[BC_IMEI1].szCode,
               m_InputSN[BC_IMEI2].szCode,
               simba_result.errCode == SP_OK ? "PASS" : "FAIL",
               sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond );
               
    fputs( szData, pFileData );
    
    fclose( pFileData );
    
    if ( simba_result.errCode == SP_OK )
    {
        char szPath[512] = {0};
        GetShareMemory( ShareMemory_Path_Input_Txt, ( void* )szPath, sizeof( szPath ) );
        
        if ( !PathFileExistsA( szPath ) )
        {
            NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "\"%s\"  file does not exist.", szPath );
            return SP_E_FAIL;
        }
        
        std_que_str input_file;
        if ( !ReadInputFile( szPath, input_file ) )
        {
            NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ReadInputFile Fail." );
            return SP_E_FAIL;
        }
        if ( input_file.size() < 5 )
        {
            NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Please confirm whether the input file contains the SN|BT|WIFI|IMEI1|IMEI2." );
            return SP_E_FAIL;
        }
        
        for ( int i = 0; i < 5; i++ )
        {
            string strData = input_file.front();
            input_file.pop();
        }
        
        if ( input_file.size() == 0 )
        {
            DeleteFileA( szPath );
        }
        else
        {
            WriteInputFile( szPath, input_file );
        }
    }
    return SP_OK;
}
bool CSaveCodesInfo::Creat_Dir( const char* pszDir )
{
    string strDir( pszDir );
	string strPath = "";
    const string strFname( "\\" );
    if ( PathIsDirectoryA( strPath.data() ) )
    {
        return true;
    }
#pragma warning(disable:4127)
    while ( TRUE )
#pragma warning(default:4127)
    {
        string::size_type posS = strDir.find( strFname );
        if ( std::string::npos != posS )
        {
            strPath += strDir.substr( 0, posS );
            strPath += strFname;
            strDir = strDir.substr( posS + strFname.length(), strDir.length() - ( posS + strFname.length() ) );
            if ( !PathIsDirectoryA( strPath.data() ) )
            {
                if ( !CreateDirectoryA( strPath.data(), NULL ) )
                {
                    return false;
                }
            }
        }
        else
        {
            strPath +=  strDir;
            if ( !PathIsDirectoryA( strPath.data() ) )
            {
                if ( !CreateDirectoryA( strPath.data(), NULL ) )
                {
                    return false;
                }
            }
            break;
        }
    }
    return true;
}
