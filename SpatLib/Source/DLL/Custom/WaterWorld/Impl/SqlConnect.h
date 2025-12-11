#pragma once
#include "ImpBase.h"


//////////////////////////////////////////////////////////////////////////
class CSqlConnect : public CImpBase
{
	DECLARE_RUNTIME_CLASS(CSqlConnect)
public:
	CSqlConnect(void);
	virtual ~CSqlConnect(void);

protected:
	virtual BOOL    LoadXMLConfig(void);
	virtual SPRESULT __PollAction(void);


protected:
	std::string m_strServerType;
	std::string m_strServerIP;
	std::string m_strServerDatabase;
	std::string m_strServerUser;
	std::string m_strServerPassword;
	std::string m_strServerDriver;

	std::string m_strUrl;
	std::string m_strWebApi;
	std::string m_strOTAWebApi;
	std::string m_strToken;
};