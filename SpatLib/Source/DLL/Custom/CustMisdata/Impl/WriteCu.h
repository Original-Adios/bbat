#pragma once
#include "ImpBase.h"

#define TCT_CU_REF_LEN    (20)
#define SOFT_VERSION_LEN (100)
#pragma pack(push, 4)
struct MISCDATA_CU
{
	CHAR CU[TCT_CU_REF_LEN];
	CHAR reserved[4];
	INT  root_flag; // 0x52: root   0x4E: un-root
	BYTE reserved2[4];                      
	CHAR SOFTVERSION[SOFT_VERSION_LEN];     
	MISCDATA_CU()
	{
		memset(this,0,sizeof(MISCDATA_CU));
	}
};
#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////
class CWriteCu : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteCu)
public:
    CWriteCu(void);
    virtual ~CWriteCu(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);
	std::string m_strCu;

};
