// TINNO_MES.h : TINNO_MES_Interface DLL 头文件
//

#pragma once


#define DLL_EXPORT 

#ifdef DLL_EXPORT
   #define DLLPOINT _declspec(dllexport)
#else
   #define DLLPOINT _declspec(dllimport)
#endif

#ifndef TINNO_MES_H
	#define TINNO_MES_H

	#ifdef __cplusplus
	extern "C"
	{
	#endif
		//************************************************************
		/*====MES_Login()
		功能描述：MES登录
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)
		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_Login(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
//		int WINAPI MES_Login(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
		
		//************************************************************
		/*====MES_Login_out()
		功能描述：用于MES登录界面完成登录后退出，因为每个个函数中都有login和logout
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)
		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_Login_out(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
//		int WINAPI MES_Login_out(char *in_mesnote,char *in_username,char *in_userpwd,char *out_ReturnMessage);
		
		//************************************************************

		/*==== MES_solist()
		功能描述：根据订单类型，获取订单名清单
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_sotype：订单类型：130004 销售订单
		//====输出参数
		out_solist：订单名清单数组
		i_solist:订单名清单数组大小
		out_ReturnMessage：返回信息(错误信息或提示信息)
		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int  MES_solist(char *in_mesnote,char *in_username,char *in_userpwd,char *in_sotype,char ***out_solist,unsigned int *i_solist,char *out_ReturnMessage);

		//************************************************************
		/*==== MES_molist()
		功能描述：根据工单类型，获取工单名清单
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_motype：工单类型：130001 SMT工单，130002 组装工单，130003 包装工单
		//====输出参数
		out_molist：工单名清单数组
		i_molist：工单名清单数组大小
		out_ReturnMessage：返回信息(错误信息或提示信息)
		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int  MES_molist(char *in_mesnote,char *in_username,char *in_userpwd,char *in_motype,char ***out_molist,unsigned int *i_molist,char *out_ReturnMessage);

		//************************************************************
		/*==== MES_GetinfoSN()
		功能描述：根据PCBA SN号（Barcode），获取跟Barcode对应的数据信息
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_PCBSN：PCBA SN号（主板BARCODE）
		in_MONO：工单号(空则不校验工单)
		//====输出参数
		out_IMEI_M：主IMEI
		out_IMEI_S：副IMEI
		out_SN：背标SN
		out_MAC：wifi地址
		out_BT：蓝牙地址
		out_SpecialSN：客户SN
		out_OTPpwd：OTP密码
		out_imeimo：工单号
		out_RoutingStep：工艺路线，当前节点
		out_RoutingStep_Pre：工艺路线，上一节点
		out_RoutingStep_Next：工艺路线，下一节点
		//--2017-06-13 added ,08-28 changed
		out_MONOSMT:SMT工单
		out_MONOPack：包装工单
		out_MONOAssy： 组装工单
		out_SONO:包装订单

		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_GetinfoSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SONO,char *out_ReturnMessage);
//		int WINAPI MES_GetinfoSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SONO,char *out_ReturnMessage);
		
		//************************************************************
		/*==== MES_GetinfoIMEI()
		功能描述：根据IMEI号，获取跟IMEI对应的数据信息
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_LotPKSN：整机SN、主IMEI、副IMEI
		in_PCBSN：PCBA SN（空则不校验主板号）
		in_MONO：包装工单号（空则不校验IMEI与包装工单号一致性）
		//====输出参数
		out_IMEI_M：主IMEI
		out_IMEI_S：副IMEI
		out_SN：背标SN
		out_MAC：wifi地址
		out_BT：蓝牙地址
		out_SpecialSN：客户SN
		out_OTPpwd：OTP密码
		out_imeimo：工单号
		out_RoutingStep：工艺路线，当前节点
		out_RoutingStep_Pre：工艺路线，上一节点
		out_RoutingStep_Next：工艺路线，下一节点
		//--2017-04-10 add
		out_Color: 颜色
		out_MEID: MEID号码
		out_pESN: pESN号码
		//--2017-06-13 add
		out_SONO:包装订单
		out_MONOSMT:SMT工单
		out_MONOPack：包装工单
		out_MONOAssy： 组装工单

		out_SIMLockCode: SIMLock码

		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_GetinfoIMEI(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotPKSN,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_Color,char *out_MEID,char *out_pESN,char *out_SONO,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SIMLockCode,char *out_ReturnMessage);
		//int WINAPI MES_GetinfoIMEI(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotPKSN,char *in_PCBSN,char *in_MONO,char *out_IMEI_M,char *out_IMEI_S,char *out_SN ,char *out_MAC,char *out_BT,char *out_SpecialSN,char *out_OTPpwd,char *out_imeimo,char *out_RoutingStep,char *out_RoutingStep_Pre,char *out_RoutingStep_Next,char *out_Color,char *out_MEID,char *out_pESN,char *out_SONO,char *out_MONOSMT,char *out_MONOPack,char *out_MONOAssy,char *out_SIMLockCode,char *out_ReturnMessage);

		//************************************************************
		/*==== MES_GetinfoSOMO()
		功能描述：根据订单号，获取跟该订单对应的数据信息
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_MONO：订单、工单号*包装工单号或者订单号
		in_ISSO：是否订单*工单号还是订单号：Y：SO，N：MO
		//====输出参数
		out_SONO：订单号
		out_MONO：工单号
		out_SoftWareVarTinno：内部软件版本
		out_SoftWareVarCustomer：客户软件版本
		out_DatabaseFTPUrl：FTP服务器IP路径（MTK database文件保存路径）
		out_ftpuser：FTP用户名
		out_ftppwd：FTP密码
		out_IsDualCard：是否双SIM卡
		out_IsWriteWifi：是否写WIFI
		out_IsWriteBT：是否写BT
		out_IsWriteSN：是否写SN
		out_GiftWeightLow：彩盒重量下限
		out_GiftWeightTop：彩盒重量上限
		out_CartonWeightLow：卡通箱重量下限
		out_CartonWeightTop：卡通箱量上限
		//--2017-04-10 add
		out_FeatureOrSmart:手机类型
		out_Platform:手机方案平台
		out_IsWriteMEID:是否写MEID
		out_IsWritePESN:是否写pESN

		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_GetinfoSOMO(char *in_mesnote,char *in_username,char *in_userpwd,char *in_MONO,char *in_ISSO,char *out_SONO,char *out_MONO,char *out_SoftWareVarTinno,char *out_SoftWareVarCustomer,char *out_DatabaseFTPUrl,char *out_ftpuser,char *out_ftppwd,char *out_IsDualCard,char *out_IsWriteWifi,char *out_IsWriteBT,char *out_IsWriteSN,char *out_GiftWeightLow,char *out_GiftWeightTop,char *out_CartonWeightLow,char *out_CartonWeightTop,char *out_FeatureOrSmart,char *out_Platform,char *out_IsWriteMEID,char *out_IsWritePESN,char *out_ReturnMessage);
		//int WINAPI MES_GetinfoSOMO(char *in_mesnote,char *in_username,char *in_userpwd,char *in_MONO,char *in_ISSO,char *out_SONO,char *out_MONO,char *out_SoftWareVarTinno,char *out_SoftWareVarCustomer,char *out_DatabaseFTPUrl,char *out_ftpuser,char *out_ftppwd,char *out_IsDualCard,char *out_IsWriteWifi,char *out_IsWriteBT,char *out_IsWriteSN,char *out_GiftWeightLow,char *out_GiftWeightTop,char *out_CartonWeightLow,char *out_CartonWeightTop,char *out_FeatureOrSmart,char *out_Platform,char *out_IsWriteMEID,char *out_IsWritePESN,char *out_ReturnMessage);

		//************************************************************
		/*==== MES_NextSN()
		功能描述：根据PCBA SN（Barcode）过节点
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_LotSN：PCBA SN号（若非需要保存IMEI与SN对应关系的写号节点，此项为空）
		in_ShiftID：班次（空则不保存班次信息）
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_NextSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotSN,char *in_ShiftID,char *out_ReturnMessage);
//		int WINAPI MES_NextSN(char *in_mesnote,char *in_username,char *in_userpwd,char *in_LotSN,char *in_ShiftID,char *out_ReturnMessage);
		
		//************************************************************
		/*====MES_NextIMEI()
		功能描述：根据IMEI过节点
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_IMEI_M_S_SN：主副IMEI或者背标SN
		in_LotSN：PCBA SN号（若非需要保存IMEI与SN对应关系的写号节点，此项为空）
		in_ShiftID：班次（空则不保存班次信息）
		in_SoftWareVarTinno：内部软件版本（若非需要保存IMEI与SN对应关系的写号节点，此项为空）
		in_SoftWareVarCustomer：客户软件版本（若非需要保存IMEI与SN对应关系的写号节点，此项为空）
		in_Remark:  备注信息，该信息将写入包装过站时的事务表备注中（长度不可以超过200字符串）,如对号写号可写入：(写IMEI 采用扫描SN,校验SN； 
					校验版本，CIT：Y，ANT：Y，GPS/WIFI：Y；GPS Data：N；IMEI LOCK：N；ATTKFlag：N；Reset：Y；OTP写号异常处理：N；)标记信息 
					可为空在字段，若不用，可以直接不传该参数。 

		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_NextIMEI(char *in_mesnote,char *in_username,char *in_userpwd,char *in_IMEI_M_S_SN,char *in_LotSN,char *in_ShiftID,char *in_SoftWareVarTinno,char *in_SoftWareVarCustomer,char *in_Remark,char *out_ReturnMessage);
//		int WINAPI MES_NextIMEI(char *in_mesnote,char *in_username,char *in_userpwd,char *in_IMEI_M_S_SN,char *in_LotSN,char *in_ShiftID,char *in_SoftWareVarTinno,char *in_SoftWareVarCustomer,char *in_Remark,char *out_ReturnMessage);
		
		//************************************************************
		/*====FTP_GetFTPInformation()
		功能描述：从当前应用程序所在路径下的Config/MES.ini中读取FTP ip，username，password
		//====输入参数
		 
		//====输出参数
		out_ftp_ip:返回 FTP ip
		out_ftp_user:返回 FTP user
		out_ftp_password:返回 FTP password
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_GetFTPInformation(char *out_ftp_ip,char *out_ftp_user,char *out_ftp_password,char *out_ReturnMessage);
//		int WINAPI FTP_GetFTPInformation(char *out_ftp_ip,char *out_ftp_user,char *out_ftp_password,char *out_ReturnMessage);

		//************************************************************
		/*====FTP_ConnectFTPServer()
		功能描述：连接FTP Server
		//====输入参数
		in_FTP_IP: FTP ip
		in_FTP_UserName: FTP user
		in_FTP_Password: FTP password		
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_ConnectFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
//		int WINAPI FTP_ConnectFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_UpLoad_TestLogFile_To_FTPServer()
		功能描述：上传测试LOG至 FTP Server
		//====输入参数
		in_OrderID:生产工单号（暂时未使用，可为空）
		in_LocalFilePathName : the path and file name saved in local PC;
		in_LogFileName : the file name saved in FTP server and local PC;
        in_FTP_Directory: the test log file saved path in FTP,max include 4 subdirectory,not include file name; 	
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_UpLoad_TestLogFile_To_FTPServer(char* in_OrderID,char* in_LocalFilePathName, char* in_LogFileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_UpLoad_TestLogFile_To_FTPServer(char* in_OrderID,char* in_LocalFilePathName, char* in_LogFileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_DownLoad_ConfigureFile_From_FTPServer()
		功能描述：从 FTP Server上下载configure file
		//====输入参数
		in_OrderID:生产工单号（暂时未使用，可为空）
	    in_LocalSavedFilePathName : the path and file name saved in local PC;
	    in_ConfigureFileName : the file name saved in FTP server and local PC;
	    in_FTP_Directory: the configure file saved path in FTP,max include 4 subdirectory,not include file name; 
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_DownLoad_ConfigureFile_From_FTPServer(char* in_OrderID,char* in_LocalSavedFilePathName, char* in_ConfigureFileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_DownLoad_ConfigureFile_From_FTPServer(char* in_OrderID,char* in_LocalSavedFilePathName, char* in_ConfigureFileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_RemoveFile_From_FTPServer()
		功能描述：从 FTP Server删除文件
		//====输入参数
	    in_ConfigureFileName : the file name saved in FTP server and local PC;
	    in_FTP_Directory: the configure file saved path in FTP,max include 4 subdirectory,not include file name; 
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_RemoveFile_From_FTPServer(char* in_ConfigureFileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_RemoveFile_From_FTPServer(char* in_ConfigureFileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_DisConnectFTPServer()
		功能描述：断开FTP Server连接
		//====输入参数
		//无
		//====输出参数
		//无

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_DisConnectFTPServer();
//		int WINAPI FTP_DisConnectFTPServer();

		//************************************************************
		/*====MES_SaveTestRecord()
		功能描述：添加测试记录或者更新记录
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码

		in_SN：PCBSN号 
		in_ShiftID：班次（空则不保存班次信息）
		in_StartDateTime：测试开始时间
		in_EndDateTime：测试结束时间
		in_TestStatu：测试结果，pass or fail
		in_TestComment：备注信息
		in_Type：添加记录或者更新记录 ，ADD or Update

		in_TestName：测试站名称
		in_FixtureNo：治具编号
		in_RFCableNo：RF线编号
		in_InstrumentNo:仪器编号
		in_ToolVersion：工具版本（平台版本_插件版本）
		in_platform：手机芯片平台
		in_failCode：失败编码
		in_failCodeDesc：失败编码描述

		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)
		
		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_SaveTestRecord(char *in_mesnote,char *in_username,char *in_userpwd,char *in_SN,char *in_ShiftID,char *in_StartDateTime,char *in_EndDateTime,char *in_TestStatu,char *in_TestComment,char *in_Type,char *in_TestName,char *in_FixtureNo,char *in_RFCableNo,char *in_InstrumentNo,char *in_ToolVersion,char *in_platform,char *in_failCode,char *in_failCodeDesc,char *out_ReturnMessage);
//		int WINAPI MES_SaveTestRecord(char *in_mesnote,char *in_username,char *in_userpwd,char *in_SN,char *in_ShiftID,char *in_StartDateTime,char *in_EndDateTime,char *in_TestStatu,char *in_TestComment,char *in_Type,char *in_TestName,char *in_FixtureNo,char *in_RFCableNo,char *in_InstrumentNo,char *in_ToolVersion,char *in_platform,char *in_failCode,char *in_failCodeDesc,char *out_ReturnMessage);
		
		//************************************************************
		/*====MES_GetTestStatus()
		功能描述：获取指定barcode在指定节点的测试状态
		//====输入参数
		in_mesnote：当前节点，如025
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码

		in_Barcode：Barcode
	
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_GetTestStatus(char *in_mesnote,char *in_username,char *in_userpwd,char *in_Barcode,char *out_ReturnMessage);
//		int WINAPI MES_GetTestStatus(char *in_mesnote,char *in_username,char *in_userpwd,char *in_Barcode,char *out_ReturnMessage);
		
		//************************************************************
		//----2017-04-10 Add

		/*====MES_GetAPDBandBPLGFile()
		功能描述：从MES配置的FTP上自动下载APDB，BPLG文件到本地制定路径下
		//====输入参数
		in_mesnote：当前节点，如011
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_MONO：订单、工单号*包装工单号或者订单号
		in_ISSO：是否订单*工单号还是订单号：Y：SO，N：MO
		in_LocalSavePath：APDB，BPLG文件保存到本地的路径 ，如D:\Test

		//====输出参数
		out_APDBFile：MTK databse file(APDB File)
		out_BPLG_LWG_File：MTK databse file(BPLG LWG File)
		out_BPLG_LTG_File：MTK databse file(BPLG LTG File)
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int MES_GetAPDBandBPLGFile(char *in_mesnote,char *in_username,char *in_userpwd,char *in_MONO,char *in_ISSO,char *in_LocalSavePath,char *out_APDBFile,char *out_BPLG_LWG_File,char *out_BPLG_LTG_File,char *out_ReturnMessage);
//		int WINAPI MES_GetAPDBandBPLGFile(char *in_mesnote,char *in_username,char *in_userpwd,char *in_MONO,char *in_ISSO,char *in_LocalSavePath,char *out_APDBFile,char *out_BPLG_LWG_File,char *out_BPLG_LTG_File,char *out_ReturnMessage);

		//************************************************************
		
		//--20170620 add for Wchart server
				
		//************************************************************
		/*====FTP_Get_FileInteractiveFTPInformation()
		功能描述：从当前应用程序所在路径下的Config/MES.ini中读取File Interactive FTP ip，username，password
		//====输入参数
		 
		//====输出参数
		out_ftp_ip:返回 FTP ip
		out_ftp_user:返回 FTP user
		out_ftp_password:返回 FTP password
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Get_FileInteractiveFTPInformation(char *out_ftp_ip,char *out_ftp_user,char *out_ftp_password,char *out_ReturnMessage);
//		int WINAPI FTP_Get_FileInteractiveFTPInformation(char *out_ftp_ip,char *out_ftp_user,char *out_ftp_password,char *out_ReturnMessage);

		//************************************************************
		/*====FTP_Connect_FileInteractiveFTPServer()
		功能描述：连接File Interactive FTP Server
		//====输入参数
		in_FTP_IP: FTP ip
		in_FTP_UserName: FTP user
		in_FTP_Password: FTP password		
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Connect_FileInteractiveFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
//		int WINAPI FTP_Connect_FileInteractiveFTPServer(char* in_FTP_IP, char* in_FTP_UserName, char* in_FTP_Password, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_Upload_To_FileInteractiveFTPServer()
		功能描述：上传测试请求文件至 File Interactive FTP Server
		//====输入参数
		in_LocalFilePathName : the path and file name saved in local PC;
		in_RequireFileName : the file name saved in FTP server and local PC;
        in_FTP_Directory: the test log file saved path in FTP,max include 4 subdirectory,not include file name; 	
		//====输出参数
		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Upload_To_FileInteractiveFTPServer(char* in_LocalFilePathName, char* in_RequireFileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_Upload_To_FileInteractiveFTPServer(char* in_LocalFilePathName, char* in_RequireFileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_Download_From_FileInteractiveFTPServer()
		功能描述：从 File Interactive FTP Server上下载测试应答文件
		//====输入参数
	    in_LocalSavedFilePathName : the path and file name saved in local PC;
	    in_ResponseFileName : the file name saved in FTP server and local PC;
	    in_FTP_Directory: the configure file saved path in FTP
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Download_From_FileInteractiveFTPServer(char* in_LocalSavedFilePathName, char* in_ResponseFileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_Download_From_FileInteractiveFTPServer(char* in_LocalSavedFilePathName, char* in_ResponseFileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_RenameFile_In_FileInteractiveFTPServer()
		功能描述：把 File Interactive FTP Server上的指定文件，转移到另一个路径下，且可重命名
		//====输入参数
	    in_OldFilePathName : FTP上待转移文件名及路径;
	    in_NewFilePathName : FTP上转移后的文件名及路径;
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_RenameFile_In_FileInteractiveFTPServer(char* in_OldFilePathName, char* in_NewFilePathName, char* out_ReturnMessage);
//		int WINAPI FTP_RenameFile_In_FileInteractiveFTPServer(char* in_OldFilePathName, char* in_NewFilePathName, char* out_ReturnMessage);

		//************************************************************
		/*====FTP_Check_FileIfExisted_in_FileInteractiveFTPServer()
		功能描述：检查指定文件在FileInteractiveFTPServer上是否存在
		//====输入参数
	    in_FileName : the file name saved in FTP server and local PC;
	    in_FTP_Directory: the configure file saved path in FTP
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Check_FileIfExisted_in_FileInteractiveFTPServer(char* in_FileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_Check_FileIfExisted_in_FileInteractiveFTPServer(char* in_FileName, char* in_FTP_Directory, char* out_ReturnMessage);

		//************************************************************
		/*====FTP_Remove_From_FileInteractiveFTPServer()
		功能描述：从 File Interactive FTP Server删除文件
		//====输入参数
	    in_FileName : the file name saved in FTP server and local PC;
	    in_FTP_Directory: the configure file saved path in FTP
	    //====输出参数
		out_ReturnMessage: 返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_Remove_From_FileInteractiveFTPServer(char* in_FileName, char* in_FTP_Directory, char* out_ReturnMessage);
//		int WINAPI FTP_Remove_From_FileInteractiveFTPServer(char* in_FileName, char* in_FTP_Directory, char* out_ReturnMessage);
		
		//************************************************************
		/*====FTP_DisConnect_FileInteractiveFTPServer()
		功能描述：断开File Interactive FTP Server连接
		//====输入参数
		//无
		//====输出参数
		//无

		//====函数返回
		成功：1
		失败：-1
		*/
		DLLPOINT int FTP_DisConnect_FileInteractiveFTPServer();
//		int WINAPI FTP_DisConnect_FileInteractiveFTPServer();

		//************************************************************
		//===V1.0.8
		/*====MES_GetEFuseFiles_MTK()
		功能描述：从MES配置的EFuseUrl FTP上自动下载Efuse文件到本地指定路径下
		//====输入参数
		in_mesnote：当前节点，如011
		in_username：当前操作用户名
		in_userpwd：当前操作用户密码
		in_MONO：订单、工单号*包装工单号或者订单号
		in_ISSO：是否订单*工单号还是订单号：Y：SO，N：MO
		in_LocalSavePath：EFuse文件保存到本地的路径 ，如D:\Test

		//====输出参数
		out_DA_SWSEC_BinFile：MTK EFuse DA SWSEC bin File
		out_EFuse_XmlFile：MTK EFuse_Xml File
		out_Scatter_TxtFile：MTK EFuse Scatter txt File
		out_PreLoader_BinFile：MTK EFuse PreLoader bin File

		out_ReturnMessage：返回信息(错误信息或提示信息)

		//====函数返回
		成功：1
		失败：-1
		*/

		DLLPOINT int MES_GetEFuseFiles_MTK(char *in_mesnote,char *in_username,char *in_userpwd,char *in_MONO,char *in_ISSO,char *in_LocalSavePath,char *out_DA_SWSEC_BinFile,char *out_EFuse_XmlFile,char *out_Scatter_TxtFile,char *out_PreLoader_BinFile,char *out_ReturnMessage);
		
		//************************************************************

	#ifdef __cplusplus
	}
	#endif
#endif //#ifndef TINNO_MES_H