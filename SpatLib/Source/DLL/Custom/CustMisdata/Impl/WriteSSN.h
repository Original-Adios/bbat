#pragma once
#include "ImpBase.h"

typedef enum
{
	Mode_EncryptSSNToS1 = 0,
	Mode_EncryptSSN,
	Mode_EncryptSSNToJoy2S,
	Mode_EncryptSSNToJoy3,
	Mode_EncryptSSNToM4,
	Mode_EncryptSSNToT10,
	Mode_EncryptSSNToQ1,
	Mode_EncryptSSNToK1PF,
	Mode_EncryptSSNToTeemo,
	TIMO_ENCRYPT_MODE_MAX
}ENCRYPT_MODE;

//////////////////////////////////////////////////////////////////////////
class CWriteSSN : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CWriteSSN)
public:
    CWriteSSN(void);
    virtual ~CWriteSSN(void);

protected:
    virtual SPRESULT __PollAction (void);
    
    // Load XML configuration of current class node from *.seq file
    virtual BOOL     LoadXMLConfig(void);
	SPRESULT LookupSSNKey(LPSTR *pkeyVal);
	ENCRYPT_MODE m_nEncryptMode;
	std::string m_strConfigPath;
	std::string m_strProName;
	LPSTR *m_pCryptKey;
	bool m_bLookupKey;
};
