#pragma once
#include "ImpBase.h"

enum UPLOAD
{
	NONE  = 0,
	SYNC  = 1,
	ASYNC = 2
};

//Upload Data Log Level 
enum LOGLEVEL
{
	LEVLE_BASIC = 1,
	LEVLE_FT_CURRENT = 2,
	LEVLE_CAL = 4,
	LEVLE_NV = 8,
	LEVLE_MISC = 16,
	LEVLE_ALL = 9999
};
//////////////////////////////////////////////////////////////////////////
class CMES_Connect : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Connect)
public:
    CMES_Connect(void);
    virtual ~CMES_Connect(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

	std::wstring m_strDLL;
private:
	UPLOAD m_eUpload;
	LOGLEVEL m_eLoglevel;
	std::wstring m_strServerType;
	std::wstring m_strServerIP;
	std::wstring m_strServerDatabase;
	std::wstring m_strServerUser;
	std::wstring m_strServerPassword;

	std::string BatchName;
    std::string StationName;
};
