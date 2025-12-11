#include "StdAfx.h"
#include "GetInputInfo.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_CLASS( CGetInputInfo )

//
CGetInputInfo::CGetInputInfo()
{
}

CGetInputInfo::~CGetInputInfo()
{

}

BOOL CGetInputInfo::LoadXMLConfig( void )
{
    m_strFilePath = _W2CA( GetConfigValue( L"Option:FilePath", L"" ) );
    return TRUE;
}

SPRESULT CGetInputInfo::__PollAction( void )
{
    if ( !PathFileExistsA( m_strFilePath.data() ) )
    {
        NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "\"%s\"  file does not exist.", m_strFilePath.data() );
        return SP_E_FAIL;
    }
    
    std_que_str input_file;
    if ( !ReadInputFile( m_strFilePath.data(), input_file ) )
    {
        NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ReadInputFile Fail." );
        return SP_E_FAIL;
    }
    if ( input_file.size() < 5 )
    {
        NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Please confirm whether the input file contains the SN|BT|WIFI|IMEI1|IMEI2." );
        return SP_E_FAIL;
    }
    
    ZeroMemory( m_InputSN, sizeof( m_InputSN ) );
    
    for ( int i = 0; i < 5; i++ )
    {
        string strData = input_file.front();
        input_file.pop();
        string::size_type pos( 0 );
        BC_INDEX bc_index;
        if ( ( pos = strData.find( "BT_MAC_" ) ) != string::npos )
        {
            strData.replace( pos, 7, "" );
            strData = replace_all( strData );
            bc_index = BC_BT;
            if ( ( int )strData.length() == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 )
            {
                m_InputSN[bc_index].bEnable = true;
                strcpy_s( m_InputSN[bc_index].szCode, sizeof( m_InputSN[bc_index].szCode ), strData.data() );
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "BT : %s", strData.data() );
            }
            else
            {
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "BT Length must be %d", CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 );
                return SP_E_FAIL;
            }
        }
        else if ( ( pos = strData.find( "MAC_" ) ) != string::npos )
        {
            strData.replace( pos, 4, "" );
            strData = replace_all( strData );
            bc_index = BC_WIFI;
            if ( ( int )strData.length() == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 )
            {
                m_InputSN[bc_index].bEnable = true;
                strcpy_s( m_InputSN[bc_index].szCode, sizeof( m_InputSN[bc_index].szCode ), strData.data() );
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "WIFI : %s", strData.data() );
            }
            else
            {
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "WIFI Length must be %d", CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 );
                return SP_E_FAIL;
            }
        }
        else if ( ( pos = strData.find( "SerNr_" ) ) != string::npos )
        {
            strData.replace( pos, 6, "" );
            strData = replace_all( strData );
            bc_index = BC_SN1;
            if ( ( ( int )strData.length() > CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength ) )
            {
                m_InputSN[bc_index].bEnable = true;
                strcpy_s( m_InputSN[bc_index].szCode, sizeof( m_InputSN[bc_index].szCode ), strData.data() );
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "SN : %s", strData.data() );
            }
            else
            {
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "SN Length must be greater than 0" );
                return SP_E_FAIL;
            }
        }
        else if ( ( pos = strData.find( "IMEI1_" ) ) != string::npos )
        {
            strData.replace( pos, 6, "" );
            strData = replace_all( strData );
            bc_index = BC_IMEI1;
            if ( ( int )strData.length() == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 )
            {
                m_InputSN[bc_index].bEnable = true;
                strcpy_s( m_InputSN[bc_index].szCode, sizeof( m_InputSN[bc_index].szCode ), strData.data() );
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "IMEI1 : %s", strData.data() );
            }
            else
            {
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "IMEI1 Length must be %d", CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 );
                return SP_E_FAIL;
            }
        }
        else if ( ( pos = strData.find( "IMEI2_" ) ) != string::npos )
        {
            strData.replace( pos, 6, "" );
            strData = replace_all( strData );
            bc_index = BC_IMEI2;
            if ( ( int )strData.length() == CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 )
            {
                m_InputSN[bc_index].bEnable = true;
                strcpy_s( m_InputSN[bc_index].szCode, sizeof( m_InputSN[bc_index].szCode ), strData.data() );
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL, "IMEI2 : %s", strData.data() );
            }
            else
            {
                NOTIFY( "GetInputInfo", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "IMEI1 Length must be %d", CBarCodeUtility::m_BarCodeInfo[bc_index].SNlength - 1 );
                return SP_E_FAIL;
            }
        }
    }
    
    char szPath[512] = {0};
    strcpy_s( szPath, m_strFilePath.data() );
    CHKRESULT( SetShareMemory( ShareMemory_Path_Input_Txt, ( const void* )szPath, sizeof( szPath ) ) );
    CHKRESULT( SetShareMemory( ShareMemory_My_UserInputSN, ( const void* )&m_InputSN, sizeof( INPUT_CODES_T ) * BC_MAX_NUM ) );
    
    return SP_OK;
}

string& CGetInputInfo::replace_all( string& str )
{
#pragma warning(disable:4127)
    while ( TRUE )
#pragma warning(default:4127)
    {
        string::size_type pos( 0 );
        if ( ( pos = str.find( "	" ) ) != string::npos )
        {
            str.replace( pos, 1, "" );
        }
        else if ( ( pos = str.find( "    " ) ) != string::npos )
        {
            str.replace( pos, 4, "" );
        }
        else if ( ( pos = str.find( "   " ) ) != string::npos )
        {
            str.replace( pos, 3, "" );
        }
        else if ( ( pos = str.find( "  " ) ) != string::npos )
        {
            str.replace( pos, 2, "" );
        }
        else if ( ( pos = str.find( " " ) ) != string::npos
                  || ( pos = str.find( "\r" ) ) != string::npos
                  || ( pos = str.find( "\n" ) ) != string::npos )
        {
            str.replace( pos, 1, "" );
        }
        else
        {
            break;
        }
    }
    return str;
}