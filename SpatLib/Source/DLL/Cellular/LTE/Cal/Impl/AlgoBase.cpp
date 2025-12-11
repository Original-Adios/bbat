#include "StdAfx.h"
#include "AlgoBase.h"

CAlgoBase::CAlgoBase( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
	m_pNal = NULL;
	m_pFileConfig = NULL;
	m_Ant = RF_ANT_1st;    //the ant that Phone Use, now only support Ant_1st, Ant_2rd
	m_UiAnt = RF_ANT_1st;  //the ant index from UI .  e.g. ant_1st,ant_2nd, ant_3rf....
	m_Ca = LTE_CA_PCC;

	m_ePortComp = LTE_RF_PortComp0;
}

CAlgoBase::~CAlgoBase(void)
{

}

SPRESULT CAlgoBase::PreInit()
{
    CHKRESULT(__super::PreInit());

    m_pFileConfig = (IFileConfig*)GetFunc(FUNC_FILE_CONFIG);

	return SP_OK;
}

SPRESULT CAlgoBase::Init()
{
    m_pNal = (INAL*)GetFunc(FUNC_INAL);
    __SetAntCa();

    return SP_OK;
}

void CAlgoBase::SetAntCa( RF_ANT_E Ant, LTE_CA_E Ca )
{
    m_UiAnt = Ant;
    m_Ant = (RF_ANT_E)(Ant % 2);
    m_Ca = Ca;
}

void CAlgoBase::__SetAntCa()
{

}

void CAlgoBase::SaveMaxMinPwr(LPCTSTR pLogName)
{
    CExtraLog extLog;
    extLog.Open(m_pSpatBase, pLogName);
    extLog.WriteStringA("Band,Channel,Max Power,Min Power");
    for (size_t i = 0; i < m_vecMaxMinPwr.size(); i++)
    {
        extLog.WriteStringA(m_vecMaxMinPwr[i].c_str());
    }
    extLog.Close();
}

void CAlgoBase::SetAntRfChain( RF_ANT_E Ant, RF_PortCompIndex_E portindex )
{
    m_UiAnt = Ant;
    m_Ant = (RF_ANT_E)Ant;
    m_ePortComp = portindex;
}