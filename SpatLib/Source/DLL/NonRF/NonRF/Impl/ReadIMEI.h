#pragma once
#include "ImpBase.h"
#include "SharedDefine.h"

typedef struct _tagIMEI_T
{
    BOOL bEnable;
    //BOOL bCheck;
    CHAR szPrefix[MAX_IMEI_STR_LENGTH];
    EM_BARCODE_SOURCE emBarcodeSource;
    CHAR szCheckCodesFormUI[MAX_IMEI_STR_LENGTH];
    CHAR szReadIMEI[MAX_IMEI_STR_LENGTH];

    STRUCT_INITIALIZE(_tagIMEI_T);
}IMEI_T;

class CReadIMEI : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CReadIMEI)
public:
    CReadIMEI(void);
    ~CReadIMEI(void);

protected:
    virtual BOOL LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    IMEI_T m_ReadIMEI[4];

};
