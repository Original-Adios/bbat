#include "StdAfx.h"
#include "WlanLoadLossXML.h"

CWlanLoadLossXML::CWlanLoadLossXML(CSpatBase *pSpatBase)
: m_pSpatBase(pSpatBase)
{
}

CWlanLoadLossXML::~CWlanLoadLossXML(void)
{
    if (NULL != m_pSpatBase)
    {
        m_pSpatBase = NULL;
    }
}

SPRESULT CWlanLoadLossXML::WlanLoadLossXmlFile(vector<WlanMeasParamBand> &VecWlanParamBandImp, WLAN_LOSS_PARAM &wlanLossParam)
{
    if (!VecWlanParamBandImp.empty())
    {
        VecWlanParamBandImp.clear();
    }

    //Get config information from *.XML
    WlanMeasParamBand stWlanParamBand;
    SPWI_WLAN_PARAM_GROUP stChanParam;
	int nCount = 0;
    //Get wlan band config
    BOOL bEnable = FALSE;
	//m_dPreLoss = GetConfigValue(L"Option:PreLoss", 10);
	//m_Tolernece = GetConfigValue(L"Option:Tolernece", 6);
    wstring strBandSel = L"Option:Band:";
    wstring strParamChan = L"Param:";
    wstring strParamXml = L"";
	//std::wstring strTxCommon = L"Param:Common";
	//int nAvgCount = m_pSpatBase->GetConfigValue((strTxCommon + L":AvgCount").c_str(), 3);
	double m_dPreLoss = m_pSpatBase->GetConfigValue( L"Option:PreLoss", 0.5);
	double m_Tolernece = m_pSpatBase->GetConfigValue( L"Option:Tolernece", 0.3);
	//wstring sTvalue = m_pSpatBase->GetConfigValue(L"Param:Specification:TxLoss", L"0,50");
	//wstring sRvalue = m_pSpatBase->GetConfigValue(L"Param:Specification:RxLoss", L"0,50");
	wstring svalue = m_pSpatBase->GetConfigValue(L"Param:Specification:Loss", L"0,50");
	//stWlanParamBand.m_dPreLoss = m_dPreLoss;
	//stWlanParamBand.m_Tolernece = m_Tolernece;
	wlanLossParam.m_dPreLoss = m_dPreLoss;
	wlanLossParam.m_Tolernece = m_Tolernece;
	/*
	double *pDoubleTxLossRlt = m_pSpatBase->GetTokenDoubleW(sTvalue.c_str(), DEFAULT_DELIMITER_W, nCount);
	wlanLossParam.m_TxLoss_Low = pDoubleTxLossRlt[0];
	wlanLossParam.m_TxLoss_Up = pDoubleTxLossRlt[1];

	double *pDoubleLossRlt = m_pSpatBase->GetTokenDoubleW(sRvalue.c_str(), DEFAULT_DELIMITER_W, nCount);
	wlanLossParam.m_RxLoss_Low = pDoubleLossRlt[0];
	wlanLossParam.m_RxLoss_Up = pDoubleLossRlt[1];
	*/
	double *pDoubleLossRlt = m_pSpatBase->GetTokenDoubleW(svalue.c_str(), DEFAULT_DELIMITER_W, nCount);
	wlanLossParam.m_Loss_Low = pDoubleLossRlt[0];
	wlanLossParam.m_Loss_Up = pDoubleLossRlt[1];
    int nChanCount = 0;
    
    double* parrDouble = NULL;
    int* parrInt = NULL;
	SPRESULT eConFileState = SP_OK;

	std::wstring strTxCommon = L"Param:Common";
	int nTotalPackets = m_pSpatBase->GetConfigValue((strTxCommon + L":RxPacketsCount").c_str(), 1000);
	int nAvgCount = m_pSpatBase->GetConfigValue((strTxCommon + L":AvgCount").c_str(), 3);
	//cw
	bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"WlanCW").c_str(), FALSE);
	if (bEnable)
	{
		stWlanParamBand.eMode = WIFI_CW_SPECTRUM;
		strParamChan.clear();
		strParamChan = L"Param:WlanCW";
		//Get Chan 
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,7,13");
		parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if(nCount < 1)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}

		int nFreqOff = m_pSpatBase->GetConfigValue((strParamChan + L":PriChan").c_str(), 0);
		int nCenChan = 0;
		int nPrimChan = 0;
		stChanParam.vectChan.resize(nCount);
		for (int nChTmp=0;nChTmp<nCount;nChTmp++)
		{
			nCenChan = parrInt[nChTmp];
			stChanParam.vectChan[nChTmp].nCenChan = nCenChan;
			nPrimChan = nCenChan + nFreqOff*2;
			stChanParam.vectChan[nChTmp].nPriChan = nPrimChan;
		}
		stChanParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
		stChanParam.stAlgoParamGroupSub.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
		//Test item
		stChanParam.stAlgoParamGroupSub.dwMask = WIFI_CW;
		stChanParam.stAlgoParamGroupSub.eAnt = ANT_PRIMARY;
		stWlanParamBand.vecConfParamGroup.push_back(stChanParam);
		//Get specification
		//TXP spec
		strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":Specification:TXP:TXP").c_str(), L"8,20");
		parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
		if(nCount < 2)
		{
			m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
			return SP_E_WCN_INVALID_XML_CONFIG;
		}
		stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
		stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
		stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
		stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];

		VecWlanParamBandImp.push_back(stWlanParamBand);
	}
    //11b
    bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"Wlan11b").c_str(), FALSE);
    if (TRUE == bEnable)
    {
		stWlanParamBand.vecConfParamGroup.clear();
        stWlanParamBand.eMode = WIFI_802_11b;
        //Get channel config
        strParamChan = L"Param:Wlan11b:GroupCount";
        char szBuf[32] = {0};

        nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

        for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
        {
            strParamChan.clear();
			stChanParam.Init();
            strParamChan = L"Param:Wlan11b:Group";
            sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex+1);
            strParamChan += m_pSpatBase->_A2CW(szBuf);
            //Get Chan 
			strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,7,13");
			parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			int nFreqOff = m_pSpatBase->GetConfigValue((strParamChan + L":PriChan").c_str(), 0);
			int nCenChan = 0;
			int nPrimChan = 0;
			stChanParam.vectChan.resize(nCount);
			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				nCenChan = parrInt[nChTmp];
				stChanParam.vectChan[nChTmp].nCenChan = nCenChan;
				nPrimChan = nCenChan + nFreqOff*2;
				stChanParam.vectChan[nChTmp].nPriChan = nPrimChan;
			}
			stChanParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
			stChanParam.stAlgoParamGroupSub.nTotalPackets = nTotalPackets;
            //Get modulation type
			LPCSTR lpVal = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"CCK11"));
			E_WLAN_RATE eRate = CwcnUtility::WlanGetRate(lpVal);
			if(INVALID_WLAN_RATE == eRate)
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stChanParam.stAlgoParamGroupSub.eRate = eRate;
			stChanParam.stAlgoParamGroupSub.eAnt = ANT_PRIMARY;
            //Get per rx level
            stChanParam.stAlgoParamGroupSub.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
            RSLT_RANGE_CHECK(stChanParam.stAlgoParamGroupSub.dVsgLvl, -100, -25, eConFileState, SP_E_WCN_INVALID_XML_CONFIG);
            //Get ref level
            stChanParam.stAlgoParamGroupSub.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
            //Get test item
            strParamChan += L":TestItem:";
            //TXP
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_TXP;
            }
            //TCFT
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TCFT").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_FER;
            }
            //EVM
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"EVM").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_EVM;
            }
            //SPECMASK
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"SPECMASK").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_MASK;
            }
			//CW
			bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"CW").c_str(), FALSE);
			if (bEnable)
			{
				stChanParam.stAlgoParamGroupSub.dwMask = WIFI_CW;
			}
            //PER
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"PER").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_PER;
            }

            stWlanParamBand.vecConfParamGroup.push_back(stChanParam);
        }
        //Get specification
        strParamChan.clear();
        strParamChan = L"Param:Wlan11b:Specification";
        //TXP spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"8,20");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
        stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
		stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
		stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
        //EVM spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":EVM:EVM").c_str(), L"-5,-8,-10,-13,-16,-19,-22,-25");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 4)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid 11b EVM spec");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
		int nIndex = 0;
        for (int nEvmIndx = DSSS_1; nEvmIndx <= CCK_11; nEvmIndx++)
        {
            stWlanParamBand.stSpec.dEvm[nEvmIndx].low = 0;
            stWlanParamBand.stSpec.dEvm[nEvmIndx].upp = parrDouble[nIndex++];
        }
        //Fer spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCFT:TCFT").c_str(), L"-15,15");
        parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount, 10);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid FER limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dFer.low = parrInt[0];
        stWlanParamBand.stSpec.dFer.upp = parrInt[1];
        //PER spec
        stWlanParamBand.stSpec.dPer = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), 10.0);

        VecWlanParamBandImp.push_back(stWlanParamBand);
    }

    //11g
    bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"Wlan11g").c_str(), FALSE);
    if (TRUE == bEnable)
    {
        stWlanParamBand.vecConfParamGroup.clear();
        stWlanParamBand.eMode = WIFI_802_11g;
        //Get channel config
        strParamChan = L"Param:Wlan11g:GroupCount";
        char szBuf[32] = {0};

        nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

        for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
        {
            strParamChan.clear();
			stChanParam.Init();
            strParamChan = L"Param:Wlan11g:Group";
            sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex+1);
            strParamChan += m_pSpatBase->_A2CW(szBuf);
            //Get Chan 
			strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,7,13");
			parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			int nFreqOff = m_pSpatBase->GetConfigValue((strParamChan + L":PriChan").c_str(), 0);
			int nCenChan = 0;
			int nPrimChan = 0;
			stChanParam.vectChan.resize(nCount);
			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				nCenChan = parrInt[nChTmp];
				stChanParam.vectChan[nChTmp].nCenChan = nCenChan;
				nPrimChan = nCenChan + nFreqOff*2;
				stChanParam.vectChan[nChTmp].nPriChan = nPrimChan;
			}
			stChanParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
			stChanParam.stAlgoParamGroupSub.nTotalPackets = nTotalPackets;
            //Get modulation type
            // stChanParam.dRate = m_Map11gRate.find(m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"OFDM-54")))->second;
			LPCSTR lpVal = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"OFDM-48"));
			E_WLAN_RATE eRate= CwcnUtility::WlanGetRate( lpVal );
			if(INVALID_WLAN_RATE == eRate)
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stChanParam.stAlgoParamGroupSub.eRate = eRate;
			stChanParam.stAlgoParamGroupSub.eAnt = ANT_PRIMARY;
            //Get per rx level
            stChanParam.stAlgoParamGroupSub.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
            RSLT_RANGE_CHECK(stChanParam.stAlgoParamGroupSub.dVsgLvl, -100, -25, eConFileState, SP_E_WCN_INVALID_XML_CONFIG);
            //Get ref level
            stChanParam.stAlgoParamGroupSub.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
            //Get test item
            strParamChan += L":TestItem:";
            //TXP
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_TXP;
            }
            //TCFT
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TCFT").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_FER;
            }
            //EVM
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"EVM").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_EVM;
            }
            //SPECMASK
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"SPECMASK").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_MASK;
            }
            //RMIS
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"PER").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_PER;
            }

            stWlanParamBand.vecConfParamGroup.push_back(stChanParam);
        }
        //Get specification
        strParamChan.clear();
        strParamChan = L"Param:Wlan11g:Specification";
        //TXP spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"8,20");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
        stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
		stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
		stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
        //EVM spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":EVM:EVM").c_str(), L"-5,-8,-10,-13,-16,-19,-22,-25");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 8)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid 11g EVM spec");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
		int nIndex = 0;
        for (int nEvmIndx = OFDM_6; nEvmIndx <= OFDM_54; nEvmIndx++)
        {
            stWlanParamBand.stSpec.dEvm[nEvmIndx].low = -999;
            stWlanParamBand.stSpec.dEvm[nEvmIndx].upp = parrDouble[nIndex++];
        }
        //Fer spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCFT:TCFT").c_str(), L"-15,15");
        parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount, 10);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid FER limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dFer.low = parrInt[0];
        stWlanParamBand.stSpec.dFer.upp = parrInt[1];
        //PER spec
        //strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), L"8");
        stWlanParamBand.stSpec.dPer = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), 10.0);

        VecWlanParamBandImp.push_back(stWlanParamBand);
    }
    //11n
    bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"Wlan11n").c_str(), FALSE);
    if (TRUE == bEnable)
    {
        stWlanParamBand.vecConfParamGroup.clear();

        stWlanParamBand.eMode = WIFI_802_11n;
        //Get channel config
        strParamChan = L"Param:Wlan11n:GroupCount";
        char szBuf[32] = {0};

        nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

        for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
        {
            strParamChan.clear();
			stChanParam.Init();
            strParamChan = L"Param:Wlan11n:Group";
            sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex+1);
            strParamChan += m_pSpatBase->_A2CW(szBuf);
            //Get Chan 
			strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,7,13");
			parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			int nFreqOff = m_pSpatBase->GetConfigValue((strParamChan + L":PriChan").c_str(), 0);
			int nCenChan = 0;
			int nPrimChan = 0;
			stChanParam.vectChan.resize(nCount);
			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				nCenChan = parrInt[nChTmp];
				stChanParam.vectChan[nChTmp].nCenChan = nCenChan;
				nPrimChan = nCenChan + nFreqOff*2;
				stChanParam.vectChan[nChTmp].nPriChan = nPrimChan;
			}
			stChanParam.stAlgoParamGroupSub.nAvgCount = nAvgCount;
			stChanParam.stAlgoParamGroupSub.nTotalPackets = nTotalPackets;
            //Get modulation type
            //stChanParam.dRate = m_Map11nRate.find(m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"MCS-7")))->second;
			LPCSTR lpVal = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"MCS-7"));
			E_WLAN_RATE eRate = CwcnUtility::WlanGetRate( lpVal );
			if(INVALID_WLAN_RATE == eRate)
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stChanParam.stAlgoParamGroupSub.eRate = eRate;
			stChanParam.stAlgoParamGroupSub.eAnt = ANT_PRIMARY;
            //Get per rx level
            stChanParam.stAlgoParamGroupSub.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
            RSLT_RANGE_CHECK(stChanParam.stAlgoParamGroupSub.dVsgLvl, -100, -25, eConFileState, SP_E_WCN_INVALID_XML_CONFIG);
            //Get ref level
            stChanParam.stAlgoParamGroupSub.dRefLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RefLVL").c_str(), 10);
            //Get test item
            strParamChan += L":TestItem:";
            //TXP
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_TXP;
            }
            //TCFT
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TCFT").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_FER;
            }
            //EVM
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"EVM").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_EVM;
            }
            //SPECMASK
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"SPECMASK").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_MASK;
            }
            //RMIS
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"PER").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_PER;
            }

            stWlanParamBand.vecConfParamGroup.push_back(stChanParam);
        }
        //Get specification
        strParamChan.clear();
        strParamChan = L"Param:Wlan11n:Specification";
        //TXP spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"8,20");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
        stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
		stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
		stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
        //EVM spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":EVM:EVM").c_str(), L"-5,-10,-13,-16,-19,-22,-25,-28");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 8)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid 11n EVM spec");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
		int nIndex = 0;
        for (int nEvmIndx = MCS_0; nEvmIndx <= MCS_7; nEvmIndx++)
        {
            stWlanParamBand.stSpec.dEvm[nEvmIndx].low = -999;
            stWlanParamBand.stSpec.dEvm[nEvmIndx].upp = parrDouble[nIndex++];
        }
        //Fer spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCFT:TCFT").c_str(), L"-15,15");
        parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount, 10);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid FER limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dFer.low = parrInt[0];
        stWlanParamBand.stSpec.dFer.upp = parrInt[1];
        //PER spec
        //strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), L"8");
        stWlanParamBand.stSpec.dPer = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), 10.0);
        VecWlanParamBandImp.push_back(stWlanParamBand);

    }
    //11ac
    bEnable = (BOOL)m_pSpatBase->GetConfigValue((strBandSel + L"Wlan11ac").c_str(), FALSE);
    if (TRUE == bEnable)
    {
        stWlanParamBand.vecConfParamGroup.clear();

        stWlanParamBand.eMode = WIFI_802_11ac;
        //Get channel config
        strParamChan = L"Param:Wlan11ac:GroupCount";
        char szBuf[32] = {0};

        nChanCount = m_pSpatBase->GetConfigValue(strParamChan.c_str(), 3);

        for (int nChanIndex = 0; nChanIndex < nChanCount; nChanIndex++)
        {
            strParamChan.clear();
			stChanParam.Init();
            strParamChan = L"Param:Wlan11ac:Group";
            sprintf_s(szBuf, sizeof(szBuf), "%d", nChanIndex+1);
            strParamChan += m_pSpatBase->_A2CW(szBuf);
            //Get Chan 
			strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCH").c_str(), L"1,7,13");
			parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
			if(nCount < 1)
			{
				m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid meas chan config!");
				return SP_E_WCN_INVALID_XML_CONFIG;
			}

			int nFreqOff = m_pSpatBase->GetConfigValue((strParamChan + L":PriChan").c_str(), 0);
			int nCenChan = 0;
			int nPrimChan = 0;
			stChanParam.vectChan.resize(nCount);
			for (int nChTmp=0;nChTmp<nCount;nChTmp++)
			{
				nCenChan = parrInt[nChTmp];
				stChanParam.vectChan[nChTmp].nCenChan = nCenChan;
				nPrimChan = nCenChan + nFreqOff*2;
				stChanParam.vectChan[nChTmp].nPriChan = nPrimChan;
			}
            //Get modulation type
			LPCSTR lpVal = m_pSpatBase->_W2CA(m_pSpatBase->GetConfigValue((strParamChan + L":Modulation").c_str(), L"MCS7_1SS"));
			E_WLAN_RATE eRate = CwcnUtility::WlanGetRate( lpVal );
			if(INVALID_WLAN_RATE == eRate)
			{
				return SP_E_WCN_INVALID_XML_CONFIG;
			}
			stChanParam.stAlgoParamGroupSub.eRate = eRate;
			stChanParam.stAlgoParamGroupSub.eAnt = ANT_PRIMARY;
             //Get per rx level
            stChanParam.stAlgoParamGroupSub.dVsgLvl = m_pSpatBase->GetConfigValue((strParamChan + L":RXLVL").c_str(), -65.0);
            //Get test item
            strParamChan += L":TestItem:";
            //TXP
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TXP").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_TXP;
            }
            //TCFT
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"TCFT").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_FER;
            }
            //EVM
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"EVM").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_EVM;
            }
            //SPECMASK
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"SPECMASK").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_MASK;
            }
            //RMIS
            bEnable = (BOOL)m_pSpatBase->GetConfigValue((strParamChan + L"PER").c_str(), FALSE);
            if (bEnable)
            {
                stChanParam.stAlgoParamGroupSub.dwMask |= WIFI_PER;
            }

            stWlanParamBand.vecConfParamGroup.push_back(stChanParam);
        }
        //Get specification
        strParamChan.clear();
        strParamChan = L"Param:Wlan11ac:Specification";
        //TXP spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TXP:TXP").c_str(), L"8,20");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid TXP limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dTxp.low = parrDouble[0];
        stWlanParamBand.stSpec.dTxp.upp = parrDouble[1];
		stWlanParamBand.stSpec.dTXPant2nd.low = parrDouble[0];
		stWlanParamBand.stSpec.dTXPant2nd.upp = parrDouble[1];
        //EVM spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":EVM:EVM").c_str(), L"0,35");
        parrDouble = m_pSpatBase->GetTokenDoubleW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid EVM limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dEvm[0].low = parrDouble[0];
        stWlanParamBand.stSpec.dEvm[0].upp = parrDouble[1];
        //Fer spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":TCFT:TCFT").c_str(), L"-15,15");
        parrInt = m_pSpatBase->GetTokenIntegerW(strParamXml.c_str(), DEFAULT_DELIMITER_W, nCount, 10);
        if(nCount < 2)
        {
            m_pSpatBase->LogFmtStrA(SPLOGLV_ERROR, "[Wlan FT]Invalid FER limit");
            return SP_E_WCN_INVALID_XML_CONFIG;
        }
        stWlanParamBand.stSpec.dFer.low = parrInt[0];
        stWlanParamBand.stSpec.dFer.upp = parrInt[1];
        //PER spec
        strParamXml = m_pSpatBase->GetConfigValue((strParamChan + L":PER:PER").c_str(), L"8");
        stWlanParamBand.stSpec.dPer = m_pSpatBase->GetConfigValue(strParamXml.c_str(), 10.0);
        VecWlanParamBandImp.push_back(stWlanParamBand);
    }

  //  pWlanParamBandImp = &m_VecWlanParamBandImp;
    return SP_OK;
}
