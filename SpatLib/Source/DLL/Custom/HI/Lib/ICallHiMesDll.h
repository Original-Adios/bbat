#ifndef __HIMESDLLL_H__
#define __HIMESDLL_H__

#pragma once

#ifdef HIMESDLL_EXPORTS
#define HIMESDLL  extern "C" __declspec(dllexport) 
#else
#define HIMESDLL  extern "C" __declspec(dllimport) 
#endif

#define  MAX_ERROR_MSG_LENGTH 512
typedef unsigned int uint32;
typedef enum
{
	MES_SUCCESS = 0,			//SQL执行成功
	MES_FAIL,					//SQL执行失败
	MES_DISABLE_FAIL,			//MES离线状态
	MES_UNCONNECT_FAIL,			//SQL未连接
	MES_EOF_FAIL,				//记录集结束
}MES_RESULT;


HIMESDLL BOOL CreateMesDllObject(HANDLE pSpLog, HANDLE **pHandle);
HIMESDLL void ReleaseMesDllObject(HANDLE pHandle);

/**************************************************************************
*  @brief     检查AOI 是否有过站测试									  *
*  @param     lpcSn														  *
*  @note	  返回内容:"002:成功!";或 return "003:错误信息!"			  *	
**************************************************************************/
HIMESDLL MES_RESULT HiAoiCheck(HANDLE pHandle, LPCSTR lpcSn);

/**************************************************************************
*  @brief     添加测试信息,保存MAC等芯片信息							  *
*  @param     lpcSn---PHONEID											  *
*  lpcStationName ---站位名												  *
*  lpcMac ---格式:M:88BD78044ADC|B:88BD78044ADD (M代表MAC;B代表蓝牙)	  * 
*  lpcFilePath ---Log文件路径											  *
*  lpcFlag -----标记位：0：良品  1:不良品								  * 
*  @note	---"002:成功!";或 return "003:"+异常信息					  *
***************************************************************************/
HIMESDLL MES_RESULT HiAddTextTestInfo(HANDLE pHandle, LPCSTR lpcSn, LPCSTR lpcStationName, LPCSTR lpcMac, LPCSTR lpcFilePath, LPCSTR lpcFlag);

/**************************************************************************
*  @brief     检查SN与MAC是否写重										  *
*  @param     lpcSn---PHONEID											  *
*  lpcSn ---格式:M:88BD78044ADC|B:88BD78044ADD (M代表MAC;B代表蓝牙)		  * 
*  @note	---"002:成功!";或 return "003:"+异常信息					  *
**************************************************************************/
HIMESDLL MES_RESULT HiAddTextTestInfoCheck(HANDLE pHandle, LPCSTR lpcSn, LPCSTR lpcMac);

/**************************************************************************
*  @brief     获取SN绑定的芯片信息										  *
*  @param     lpcSn---PHONEID											  *
*  lpcSn ---格式:M:88BD78044ADC|B:88BD78044ADD (M代表MAC;B代表蓝牙)		  * 
*  @note	---"OK:|M:88BD78044ADC|B:88BD78044ADD";或 "NG:"+异常信息	  *
**************************************************************************/
HIMESDLL MES_RESULT HiGetChipInfo(HANDLE pHandle, LPCSTR lpcSn);

#endif // __IMESSDLL_H__