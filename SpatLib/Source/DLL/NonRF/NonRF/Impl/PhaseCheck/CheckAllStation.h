#pragma once
#include "CheckStation.h"
//
#define MAX_STATION_NUM     (20)
//////////////////////////////////////////////////////////////////////////
class CCheckAllStation : public CCheckStation
{
    DECLARE_RUNTIME_CLASS(CCheckAllStation)
public:
    CCheckAllStation(void);
    virtual ~CCheckAllStation(void);

protected:
    virtual SPRESULT __PollAction(void);
    virtual BOOL    LoadXMLConfig(void);

private:
    BOOL        m_bAutoMode;  // TRUE: 检查UE内配置的所有站位， FALSE: 根据SEQ配置
    std::string m_arrStationName[MAX_STATION_NUM];
};
