#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"

typedef struct _tagIMEI_T
{
    BOOL bEnable;
    CHAR szPrefix[MAX_IMEI_STR_LENGTH];
    BC_INDEX emBarcodeIndex;
    CHAR szCheckCodesFormUI[ShareMemory_SN_SIZE];
    CHAR szCheckCodesInDUT[ShareMemory_SN_SIZE];

    STRUCT_INITIALIZE(_tagIMEI_T);
}IMEI_T;

class CCheckCodes : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CCheckCodes)
public:
    CCheckCodes(void);
    virtual ~CCheckCodes(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    void LoadConfig(void);

protected:
	IMEI_T m_CheckCodes[BC_MAX_NUM];
	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
	BOOL m_bCheckCodesByInput;
};
