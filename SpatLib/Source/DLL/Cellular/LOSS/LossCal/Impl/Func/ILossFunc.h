#pragma once

#include "uni_error_def.h"
#include "rfdev_def.h"
#include "PhoneCommand.h"
#include <vector>
#include "ModeSwitch.h"

typedef enum
{
    MV_V2,
    MV_V3,
    MV_MAX
}E_MODEM_VER;

#define CHKLOSSRESULT_WITH_NOTIFY(statement, Itemname)  \
{										                    \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
    m_pImp->NOTIFY(Itemname, LEVEL_UI, 1, 0, 1);                \
    return __sprslt;                                    \
    }                                                       \
}	

class CImpBase;

class ILossFunc
{
public:
    ILossFunc(CImpBase *pSpat);
    virtual ~ILossFunc(void);

    virtual SPRESULT Run() = 0;
    virtual SPRESULT Init() = 0;
    virtual SPRESULT Release() = 0;
    virtual void Serialization(std::vector<uint8>* parrData);
    virtual void Deserialization(std::vector<uint8>* parrData);
    virtual void ConfigFreq() {};
	virtual void SetMode(SP_MODE_INFO eMode){m_eMode = eMode;} ;
    virtual SPRESULT LoadBandInfo();
protected:
    CImpBase *m_pImp;
	SP_MODE_INFO m_eMode;
};
