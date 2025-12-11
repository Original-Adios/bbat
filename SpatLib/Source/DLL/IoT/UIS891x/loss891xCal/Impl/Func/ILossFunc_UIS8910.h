#pragma once

#include "uni_error_def.h"
#include "rfdev_def.h"
#include "PhoneCommand.h"
#include <vector>
#include "ModeSwitch.h"

#define CHKLOSSRESULT_WITH_NOTIFY(statement, Itemname)  \
{										                    \
    SPRESULT __sprslt = (statement);                        \
    if (SP_OK != __sprslt)                                  \
    {                                                       \
    m_pImp->NOTIFY(Itemname, LEVEL_UI, 1, 0, 1);                \
    return __sprslt;                                    \
    }                                                       \
}	

class CImpBaseUIS8910;

class ILossFunc_UIS8910
{
public:
    ILossFunc_UIS8910(CImpBaseUIS8910 *pSpat);
    virtual ~ILossFunc_UIS8910(void);

    virtual SPRESULT Run() = 0;
    virtual SPRESULT Init() = 0;
    virtual SPRESULT Release() = 0;
    virtual void Serialization(std::vector<uint8>* parrData);
    virtual void Deserialization(std::vector<uint8>* parrData);
    virtual void ConfigFreq() {};
	virtual void SetMode(SP_MODE_INFO eMode){m_eMode = eMode;} ;
protected:
    CImpBaseUIS8910 *m_pImp;
	SP_MODE_INFO m_eMode;
};
