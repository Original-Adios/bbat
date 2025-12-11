#pragma once
#include "IAlgo.h"
#include "FuncBase.h"
#include "INalCal.h"
#include "IFileConfig.h"
#include <sstream>

class CAlgoBase :
    public CFuncBase,
	public IAlgo
{
public:
	CAlgoBase(LPCWSTR lpName, CFuncCenter* pFuncCenter);
	virtual ~CAlgoBase(void);

    virtual SPRESULT PreInit();
    virtual SPRESULT Init();
    virtual void SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca );

	void SetAntRfChain( RF_ANT_E Ant, RF_PortCompIndex_E portindex);

protected:
    virtual void __SetAntCa();
    void SaveMaxMinPwr(LPCTSTR pLogName);

protected:
    INAL* m_pNal;
    IFileConfig* m_pFileConfig;

    RF_ANT_E m_Ant;    //the ant that Phone Use, now only support Ant_1st, Ant_2rd
    RF_ANT_E m_UiAnt;  //the ant index from UI .  e.g. ant_1st,ant_2nd, ant_3rf....
    LTE_CA_E m_Ca;
    vector<string>    m_vecMaxMinPwr;

public:
	RF_PortCompIndex_E m_ePortComp;
};
