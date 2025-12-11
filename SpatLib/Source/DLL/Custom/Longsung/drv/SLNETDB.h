#if !defined(SLNETDB)
#define SLNETDB

#ifdef SLNETDB_EXPORTS
#define SLNETDB_API __declspec(dllexport)
#else
#define SLNETDB_API __declspec(dllimport)
#endif
#define SLCALL __stdcall
//#ifdef __cplusplus
extern "C" {
//#endif
#define BUFF_METHOD_SENDONLY 0x10
#define BUFF_METHOD_QUERY 0x20
#define DLLVERSION "L"
#define MAXTHREAD 8
	
#define PROTOCOL_EXT 1//数据包头扩展(SLServerV1.2以上支持) 1 增加项目名称 2 增加用户名称 4 增加软件版本

typedef enum SL_StatusTag
{
	SL_CONNECTOK,
	SL_CONNECTFAIL,
	SL_ERROR
}SL_Status;
typedef enum SL_StationTag
{
	SL_SP_DOWNLOAD = 1,
	SL_SP_SN,
	SL_SP_BT,
	SL_SP_FT,
	SL_SP_CIT,
	SL_SP_BLT,
	SL_SP_IMEI,
	SL_SP_PACK,
	SL_SP_QA,
	SL_SP_QAC,
	SL_SP_A,
	SL_SP_B,
	SL_SP_C,
	SL_SP_D,
	SL_SP_E,
	SL_SP_F,
	SL_SP_G,
	SL_SP_H,
	SL_SP_I,
	SL_SP_J,
	SL_SP_K,
	SL_SP_L,
	SL_SP_M,
	SL_SP_N
}SL_Station;

typedef enum SL_CMDTag
{
CMD_0,
CMD_PLAM		=0x10,
CMD_MODEL,
CMD_ITEM,
CMD_SN,
CMD_IMEI		=0x20,
CMD_CODE1,
CMD_CODE2,
CMD_CODEOTH,
CMD_IMEI2,
CMD_MAC,
CMD_MEID,
CMD_CODE3,
CMD_CODE4,
CMD_CODE5,
CMD_CODE6,
CMD_CODE7,
CMD_CODE8,
CMD_CODE9,
CMD_CODE10,
CMD_PIPELINE	=0x3A,
CMD_REFPIPELINE,
CMD_FINISHSTAT,
CMD_PACKET,
CMD_REPAIR,
CMD_ISCHECK,
CMD_00			=0x60,	//删除指定的SN
CMD_01,					//查询IMEI号是否存在
CMD_END
}SL_CMD;

struct SL_TCMD
{
	SL_CMD cmd;
    char  val[254];
};

//extern SLNETDB_API int nSLNETDB;
typedef void (*SL_Status_callback)(int status, void * ext);
typedef DWORD (*SL_Receive_callback)(DWORD did, char *buf,int len);//成功返回 0
SLNETDB_API bool	SLCALL SL_init(char *ip, int port = 3395, SL_Receive_callback proc1 = NULL, SL_Status_callback proc2 = NULL,int steptime = 500, int buf_send_Max = 300, int buf_recv_Max = 300, char *bufpath = NULL);
SLNETDB_API DWORD	SLCALL SL_send_Ex(const char FAR *buf, int len, int method = BUFF_METHOD_SENDONLY);//返回ID
SLNETDB_API int		SLCALL SL_command_Ex(const char FAR *buf, int len, char *recbuf, int reclen, int timeout);//返回实际接收Byte
SLNETDB_API bool 	SLCALL SL_quit();

SLNETDB_API bool	SLCALL SL_GetLastError(char *err = NULL);	//return iserror?
SLNETDB_API bool	SLCALL SL_ClearLastError();
SLNETDB_API bool	SLCALL SL_isConnected();			//return isconnected?
SLNETDB_API bool	SLCALL SL_isAutoLogind();			//return SL_UserSoftver status

SLNETDB_API bool	SLCALL SL_AddnewPhone(const char* sn, int timeout = 5000);//timeout>0，同步模式，=0异步模式

/*********CMDType*********
0  SN
1  IMEI
2  CODE1
3  CODE2
4  CODEOTH(xx=xx)
5  CODE3
6  CODE4
7  CODE5
8  CODE6
9  CODE7
10 CODE8
11 IMEI2
12 MAC
13 MEID
*************************/
SLNETDB_API bool	SLCALL SL_QueryPhoneOne(const char* sn, SL_CMDTag cmd, char* value, int timeout = 5000, int CMDType = 0);
SLNETDB_API bool	SLCALL SL_SetPhoneOne(const char* sn, SL_CMDTag cmd, const char* value, int timeout = 5000);

SLNETDB_API bool	SLCALL SL_QueryPhone(const char* sn, SL_TCMD tcmd[], int timeout = 5000, int CMDType = 0);
SLNETDB_API bool	SLCALL SL_SetPhone(const char* sn, const SL_TCMD tcmd[], int timeout = 5000);
SLNETDB_API bool	SLCALL SL_PutDatatoPhone(const char* name, bool success, int taketime, const char* head, const char* field, const char* values, const char* title = NULL, int index = 0, int tread = 0);
SLNETDB_API bool	SLCALL SL_ClrDatatoPhone(int tread = 0);
SLNETDB_API bool	SLCALL SL_ExtDatatoPhone(SL_Station Stationattr, const char* sn, \
									  int success, int taketime, const char* head, \
									  int timeout = 0, int tread = 0, char* decdatas = NULL);
SLNETDB_API bool	SLCALL SL_ReadDatafromPhone(SL_Station Stationattr, int LineID, int maxrecord, int decindex, char* recvdata, int recvdatalen, int timeout = 5000);

SLNETDB_API bool	SLCALL SL_UserSoftver(const char* itemname_in, const char* linename, const char* softname, \
								   const char* PCinfo, const char* lastconfigdate,\
								   int *lineid = NULL, int *itemid = NULL, char* itemname = NULL,\
								   int timeout = 5000\
								   );
SLNETDB_API bool	SLCALL SL_ItemRecord(const char* itemname, const char* softname, SL_Station Stationattr, int timeout = 5000);

SLNETDB_API bool	SLCALL SL_UserLogin(const char* user, const char* pass, char* name = NULL, char* accredit = NULL, char* lastlogintime = NULL, char* lastloginip = NULL, int timeout = 5000);

SLNETDB_API bool	SLCALL SL_SoftStatus(int threadid, bool startwork);
SLNETDB_API bool	SLCALL SL_DefineLog(int logtype, const char* content);//LOG上传接口 Logtype:0 新线损 1 程序变化 2 其他
SLNETDB_API bool	SLCALL SL_Request(int requestid, const char* request, char *recbuf = NULL, int reclen = 0,int timeout = 0);//服务端请求接口

SLNETDB_API bool	SLCALL SL_RepairStart(const char* sn, int status, const char* RMA, const char *description, int CMDType = 0,int timeout = 5000);
SLNETDB_API bool	SLCALL SL_RepairComplate(const char* sn, int status, char *recbuf, int reclen, int CMDType = 0,int timeout = 5000);

SLNETDB_API bool	SLCALL SL_UploadConfigfile(const char* filename);
SLNETDB_API bool	SLCALL SL_SQLQuery(const char* sql,char* xmlresult, int xmllen, int pageno = 0, int pagecount = 100,int timeout = 5000);
SLNETDB_API bool	SLCALL SL_SQLExecute(const char* sql, int *RecordsAffected = NULL, int timeout = 5000);
SLNETDB_API int		SLCALL SL_CreatePassword(const char* feed);
//#ifdef __cplusplus
}
//#endif

#endif //SLNETDB
