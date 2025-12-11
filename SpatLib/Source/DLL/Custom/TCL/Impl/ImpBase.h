#pragma once
#include "SpatBase.h"

#define TCL_CU_REF_LEN    (20)
#define SOFT_VERSION_LEN (100)
#pragma pack(push, 4)
struct MISCDATA_TCL
{
    CHAR CU[TCL_CU_REF_LEN];
    CHAR reserved[4];
    INT  root_flag; // 0x52: root   0x4E: un-root
    CHAR SOFTVERSION[SOFT_VERSION_LEN];     // V1.0 SPCSS00584369
    INT  inproduction;  			        // V1.1 SPCSS00595271
    MISCDATA_TCL()
    {
        memset(this,0,sizeof(MISCDATA_TCL));
    }
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);
};
