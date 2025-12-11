#pragma once
#include "CheckStation.h"
#define MAX_STATION_NUM     (20)
//////////////////////////////////////////////////////////////////////////
class CResetStations : public CCheckStation
{
    DECLARE_RUNTIME_CLASS(CResetStations)
public:
    CResetStations(void);
    virtual ~CResetStations(void);

protected:
    virtual SPRESULT __PollAction(void);
	virtual BOOL    LoadXMLConfig(void);

private:
	std::string m_arrStationName[MAX_STATION_NUM];
};
