#include "StdAfx.h"
#include "FunUIS8910.h"

int g_GsmPclPwr[20]  = {33, 31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1, 0, 0, 0};
int g_EGsmPclPwr[20] = {33, 31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1, 0, 0, 0};
int g_DcsPclPwr[20]  = {30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8,  6, 4, 2, 0, 0, 0, 0, 0};
int g_PcsPclPwr[20]  = {30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8,  6, 4, 2, 0, 0, 0, 0, 0};

CFunUIS8910* CFunUIS8910::InitFun(CImpBaseUIS8910 *pImp ,LPVOID dResearved)
{
    UNREFERENCED_PARAMETER(dResearved);
    CFunUIS8910 *pFun;

    //BOOL b8960 = FALSE;
    //pImp->m_pRFTester->GetProperty(DP_GSM_FDT_LIMIT, 0, (LPVOID)&b8960);

    pFun = new CFunUIS8910(pImp);
   
    return pFun;
}

void CFunUIS8910::FinalFun(CFunUIS8910 **pFun)
{
	if (*pFun != NULL)
	{
		delete *pFun;
		*pFun = NULL;
	}
}


CFunUIS8910::CFunUIS8910(CImpBaseUIS8910 *pImp)
: m_pImp(pImp)
{
    m_hDUT = pImp->m_hDUT;
    m_pRFTester = pImp->m_pRFTester;
    m_lpOutputStrA = NULL;
    m_eTestMode = TM_INVALID;
    m_eSubMode =  SUB_INVALID;
}


CFunUIS8910::~CFunUIS8910(void)
{
}

SPRESULT CFunUIS8910::EnterFun(E_TEST_MODE eTestMode, E_SUB_TEST_MODE eSubMode)
{
	m_eTestMode = eTestMode;
    m_eSubMode = eSubMode;

    CHKRESULT(ResetTester(0));
    CHKRESULT(m_pRFTester->SetNetMode(NM_GSM));
    
#if 0  //wonderful 去除原代码
    INIT_PARAM  rf;
    rf.eBand        = (SP_BAND_INFO)BI_EGSM;
    rf.dCellPower	= -70.0;
    rf.nDlChan      = 61;
    rf.nUlChan      = 61;
    
    CHKRESULT(m_pRFTester->InitDev(eTestMode, eSubMode, &rf));

    if (m_eSubMode == SUB_CAL_COMMON)
    {
        CHKRESULT(m_pRFTester->SetGen(MM_MODULATION, TRUE));
    }

	CHKRESULT(SP_gsmActive(m_hDUT, TRUE));
	PC_TX_PARAM_T tp;
	tp.training_sequence    = 0;
	tp.coding_scheme        = TOOL_TX_CODE_MCS1;
	tp.data_type            = DSP_TX_TYPE_RANDOM;
	tp.puncture_type        = 0;
	SP_gsmSetTxParam(m_hDUT, tp);
#endif
    return SP_OK;
}

SPRESULT CFunUIS8910::SetAnt( int nAnt )
{
    int iAnt;
	iAnt = nAnt;
	RF_PORT rfPort;
    U_RF_PARAM param;

    param.pPort = &rfPort;

	rfPort.ePort =  RF_ANT_1st;
	rfPort.eStatus = RS_IN;
	CHKRESULT(m_pRFTester->SetParameter(PT_RF_PORT, param));

	rfPort.ePort =  RF_ANT_1st;
	rfPort.eStatus = RS_OUT;
	CHKRESULT(m_pRFTester->SetParameter(PT_RF_PORT, param));

	return SP_OK;
}

SPRESULT CFunUIS8910::LeaveFun()
{
#if 0  //wonderful 去除原代码	  
    if (m_eSubMode == SUB_CAL_COMMON)
    {
        CHKRESULT(m_pRFTester->SetGen(MM_MODULATION, FALSE));
    }
#endif 

	CHKRESULT(SP_gsmActive(m_hDUT, FALSE));
    CHKRESULT(ResetTester(0));

	return SP_OK;
}

SPRESULT CFunUIS8910::AntSwitch( int nOld, int nNew )
{
    int temp_nNew;
    int temp_nOld;
	temp_nNew = nNew;
	temp_nOld = nOld;

#if 0  //wonderful 去除原代码	
	if (nOld == nNew)
	{
		return SP_OK;
	}

	CHKRESULT(SP_gsmAntSwitch(m_hDUT, BI_EGSM, nNew ));
#endif

	return SP_OK;
}


//----------------------------------------------------------------------------公共函数 
void CFunUIS8910::DisplayORFSMCode(char band)
{
    char temp_band;
	temp_band = band;

    Sleep(100);
#if 0  //chengui
    if(0 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "GSMMODU #69");
    }
	else if(1 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "EGSMMODU #79");
    } 
	else if(2 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "DCSMODU #89");
    } 
	else if(3 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "PCSMODU #99");
    }
#endif
}


void CFunUIS8910::DisplayORFSSCode(char band)
{
    char temp_band;
	temp_band = band;

    Sleep(100);
#if 0  //chengui
    if(0 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "GSMSWITCH #68");
    } 
	else if(1 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "EGSMSWITCH #78");
    } 
	else if(2 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "DCSSWITCH #88");
    } 
	else if(3 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "PCSSWITCH #98");
    }
#endif
}


void CFunUIS8910::DisplayPVTCode(char band)
{
    char temp_band;
	temp_band = band;

    Sleep(100);
#if 0  //chengui
    if (0 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "GSMPVT #65");
    } 
	else if(1 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "EGSMPVT #75");
    } 
	else if(2 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "DCSPVT #85");
    } 
	else if(3 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "PCSPVT #95");
    }
#endif
}


void CFunUIS8910::DisplayPFCode(char band)
{
    char temp_band;
	temp_band = band;

    Sleep(100);
#if 0  //chengui
    if(0 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "GSMFREQ #62");
    } 
	else if(1 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "EGSMFREQ #72");
    } 
	else if(2 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "DCSFREQ #82");
    } 
	else if(3 == band) 
	{
        sprintf_s(m_strcaliberrbuf , "PCSFREQ #92");
    }
#endif
}

int CFunUIS8910::pcl2dbm(char band, char pcl)
{
	if(band == 0) 
	{
		pcl -= 5;
		return(g_GsmPclPwr[pcl]);
	}
	else if(band == 1) 
	{
		pcl -= 5;
		return(g_EGsmPclPwr[pcl]);
	}
	else if(band == 2) 
	{
		return(g_DcsPclPwr[pcl]);
	}	
	else
	{
		return(g_PcsPclPwr[pcl]);
	}
}

float CFunUIS8910::ch2freq(unsigned char band, unsigned short arfcn, BOOL dw1up0)
{
    if (3 == band) 
	{ // PCS
        return ((18502 + 2 * ((float)arfcn-512) + (dw1up0? 800: 0)) / 10);
    } 
	else if (2 == band) 
	{ // DCS
        return ((17102 + 2 * ((float)arfcn-512) + (dw1up0? 950: 0)) / 10);
    } 
	else if (1 == band) 
	{ // EGSM
        if(arfcn > 124)
		{
            return (( 8900+ 2 *  (float)(arfcn - 1024)      + (dw1up0? 450: 0)) / 10);
		}
		else
		{
            return (( 8900+ 2 *  (float)arfcn      + (dw1up0? 450: 0)) / 10);
		}
    }
	else if (0 == band) 
	{ // GSM
        return (float)(( 8242.0 + 2.0 * (arfcn-128) + (dw1up0 ? 450: 0)) / 10.0);
    }
	else
	{
        return 0;
	}
}

#if 0//20191225 by jiangqing
void CFunUIS8910::BufferChangeFormat(void)
{
    int i;
    int z;
    z   = sizeof(gpib_InBuffer);

    for(i=0; i<sizeof(gpib_InBuffer); i++)
	{
        if(gpib_InBuffer[i] == ',')
		{
            gpib_InBuffer[i] = ' ';
		}
	}
    return;
}

void CFunUIS8910::GetTokenStrings(LPSTR lpszSouce, LPSTR *lppszDest, LPCSTR lpsczDelimit, int *lpnCount)
//void CFunUIS8910::GetTokenStrings(LPTSTR lpszSouce, LPTSTR *lppszDest, LPCTSTR lpsczDelimit, int *lpnCount)
{
	if((NULL == lpszSouce) || (NULL == lpsczDelimit))
	{
	    return;
	}

	//LPTSTR lpszTokenBuf = _tcstok(lpszSouce, lpsczDelimit);
	LPSTR    lpNext = NULL; 
	LPSTR lpszTokenBuf = strtok_s(lpszSouce, lpsczDelimit, &lpNext);
	int nCount = 0;
	while(NULL != lpszTokenBuf) 
	{
		if ( (_T(';') == *lpszTokenBuf) || _T('#') == *lpszTokenBuf ) 
		{
			break;
		} 
		else 
		{
			*(lppszDest++) = lpszTokenBuf;
			nCount++;

			//lpszTokenBuf = _tcstok(NULL, lpsczDelimit);
			lpszTokenBuf = strtok_s(NULL, lpsczDelimit, &lpNext);
		}
	}

	if (NULL != lpnCount)
	{
		*lpnCount = nCount;
	}
}
#endif

#if 0
LPSTR CFunUIS8910::_W2CA(LPCWSTR lpszInput)
{
    if (NULL != lpszInput)
    {
        size_t  nInputLen = wcslen(lpszInput);
        size_t nOutputLen = WideCharToMultiByte(CP_ACP, 0, lpszInput, nInputLen, NULL, 0, 0, 0) + 2;

        if (NULL != m_lpOutputStrA)
        {
            delete []m_lpOutputStrA;
            m_lpOutputStrA = NULL;
        }

        try
        {
            m_lpOutputStrA = new CHAR[nOutputLen];
        }
        catch (const std::bad_alloc& /*e*/)
        {
            m_lpOutputStrA = NULL;
        }
        
        if (NULL != m_lpOutputStrA)
        {
            memset((void* )m_lpOutputStrA, 0x0, sizeof(CHAR)*nOutputLen);
            WideCharToMultiByte(CP_ACP, 0, lpszInput, nInputLen, m_lpOutputStrA, nOutputLen, 0, 0);
            return m_lpOutputStrA;
        }
    }

    return NULL;
}

void CFunUIS8910::GetTokenStrings(LPTSTR lpszSouce, LPTSTR *lppszDest, LPCTSTR lpsczDelimit, int *lpnCount)
{
	*lpnCount = 0;
	
	if((NULL == lpszSouce) || (NULL == lpsczDelimit))
	{
	    return;
	}
	
#if defined (UNICODE) ||  defined (_UNICODE)
	LPWSTR    lpNext = NULL; 
	LPTSTR lpszTokenBuf = wcstok_s(lpszSouce, lpsczDelimit, &lpNext);//_tcstok(lpszSouce, lpsczDelimit);
#else
	LPTSTR lpszTokenBuf = strtok_s(lpszSouce, lpsczDelimit);//_tcstok(lpszSouce, lpsczDelimit);
#endif

	int nCount = 0;
	while(NULL != lpszTokenBuf) 
	{
		if ( (_T(';') == *lpszTokenBuf) || _T('#') == *lpszTokenBuf ) 
		{
			break;
		} 
		else 
		{
			*(lppszDest++) = lpszTokenBuf;
			nCount++;

			//lpszTokenBuf = _tcstok(NULL, lpsczDelimit);
#if defined (UNICODE) ||  defined (_UNICODE)
			LPTSTR lpszTokenBuf = wcstok_s(NULL, lpsczDelimit, &lpNext);//_tcstok(lpszSouce, lpsczDelimit);
#else
			LPTSTR lpszTokenBuf = strtok_s(NULL, lpsczDelimit);//_tcstok(lpszSouce, lpsczDelimit);
#endif

		}
	}

	if (NULL != lpnCount)
	{
		*lpnCount = nCount;
	}
}
#endif
//----------------------------------------------------------------------------公共函数 end



//----------------------------------------------------------------------------仪表控制函数

#if 0
SPRESULT CFunUIS8910::WriteGpibCmd(LPCSTR lpszCmd)
{
	CHKRESULT(m_pRFTester->GetGpib()->Send(lpszCmd));
	m_pImp->LogFmtStrA(SPLOGLV_INFO, "W-> %s", lpszCmd);

	return SP_OK;
}

SPRESULT CFunUIS8910::ReadGpibCmd()
{
	memset(gpib_InBuffer, 0, sizeof(gpib_InBuffer));

	CHKRESULT(m_pRFTester->GetGpib()->Recv(&gpib_InBuffer[0], sizeof(gpib_InBuffer)));

	m_pImp->LogFmtStrA(SPLOGLV_INFO, "R-> %s", gpib_InBuffer);
	return SP_OK;
}
#endif

SPRESULT CFunUIS8910::CalibInit()//(double GSMRF,double EGSMRF,double DCSRF, double PCSRF)
{
	m_pImp->LogFmtStrA(SPLOGLV_INFO, "Init the instrument tester...");
	//ibtmo(h_BASE, T3s);  //chengui

#if 0
	WriteGpibCmd("*CLS;*RST;*OPC?");
	ReadGpibCmd();
	// SYS setup
	WriteGpibCmd("CONF:SELF:REF:FREQ:SOUR INT");
	//ibtmo(h_BASE, T3s);  //chengui
	// RF port
#if 0
	WriteGpibCmd("ROUT:GPRF:GEN:RFS:CONN RF1C");
	WriteGpibCmd("ROUT:GPRF:MEAS:RFS:CONN RF1C");
	WriteGpibCmd("ROUT:GSM:MEAS:RFS:CONN RF1C");
#endif
	SetAnt(GSM_MAIN);

	WriteGpibCmd("CONF:GPRF:MEAS:POW:REP SING");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:TYPE GAUS");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:GAUS:BWID 1MHZ");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:SLEN 577.9230769E-6");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:MLEN 400E-6");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:SCO 5");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:LIST OFF");

	// Trigger
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:SOUR 'IF Power'");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:SLOP REDG");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:THR -30");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:OFFS 50E-6");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:TOUT 2");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:MGAP 0");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:MODE ALL");
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:SOUR 'Power'");
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:SLOP REDG");
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:THR -30");
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:TOUT 2");

	// GSM
	WriteGpibCmd("CONF:GSM:MEAS:MEV:REP SING");
	WriteGpibCmd("CONF:GSM:MEAS:MEV:SCON NONE");
	WriteGpibCmd("CONF:GSM:MEAS:MEV:SCO:MOD 5");
	WriteGpibCmd("CONF:GSM:MEAS:MEV:RES OFF,OFF,OFF,ON,OFF,OFF,OFF,OFF,OFF,OFF,OFF,OFF");
	WriteGpibCmd("CONF:GSM:MEAS:MEV:TOUT 2");

	//SetRfPathLoss(0);

	//WriteGpibCmd("CONFigure:BASE:FDCorrection:CTABle:DELete:ALL");
	WriteGpibCmd("SOUR:GPRF:GEN:STAT ON;*OPC?");

	ReadGpibCmd();
#else
	INIT_PARAM param;
	ZeroMemory(&param, sizeof(INIT_PARAM));
    CHKRESULT(m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_COMMON, &param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::SetBCHChannel(char band,int channel)
{
#if 0
	float freq;
	char cmdbuffer[512];

	freq = ch2freq(band, (unsigned short)channel, 1);
	sprintf_s(cmdbuffer , "SOUR:GPRF:GEN:RFS:FREQ %fMHZ",freq);
	WriteGpibCmd(cmdbuffer);
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dDlFreq = ch2freq(band, (unsigned short)channel, 1);
	CHKRESULT(m_pRFTester->SetParameter(PT_DOWNLINK_FREQ,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::SetTCHChannel(char band, unsigned short channel)
{
#if 0
	float freq;
	char cmdbuffer[512];

	freq = ch2freq(band, channel, 0);
	sprintf_s(cmdbuffer , "CONF:GPRF:MEAS:RFS:FREQ %fMHZ",freq);
	WriteGpibCmd(cmdbuffer);
	sprintf_s(cmdbuffer , "CONF:GSM:MEAS:RFS:FREQ %fMHZ",freq);
	WriteGpibCmd(cmdbuffer);
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dUlFreq = ch2freq(band, channel, 0);
	CHKRESULT(m_pRFTester->SetParameter(PT_UPLINK_FREQ,param));
#endif
	return SP_OK;

}

SPRESULT CFunUIS8910::SetTCHPcl(char band, char pcl)
{
#if 0
	char cmdbuffer[512];
	float pwr = (float)pcl2dbm(band, pcl);

	sprintf_s(cmdbuffer , "CONF:GPRF:MEAS:RFS:ENP %f",pwr);
	WriteGpibCmd(cmdbuffer);

	WriteGpibCmd("CONF:GPRF:MEAS:RFS:UMAR 8");

	sprintf_s(cmdbuffer , "CONF:GSM:MEAS:RFS:ENP %f",pwr);
	WriteGpibCmd(cmdbuffer);

	WriteGpibCmd("CONF:GSM:MEAS:RFS:UMAR 8");
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dExpPwr = (float)pcl2dbm(band, pcl) + 5;
	CHKRESULT(m_pRFTester->SetParameter(PT_EXP_PWR,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::SetTCHTsc(unsigned char tsc)
{
#if 0
	char cmdbuffer[512];

	sprintf_s(cmdbuffer , "CONF:GSM:MEAS:MEV:TSEQ TSC%d",tsc);
	WriteGpibCmd(cmdbuffer);
#else
	int iTsc;
	iTsc = tsc;
	//U_RF_PARAM param;
	//ZeroMemory(&param, sizeof(U_RF_PARAM));
	//param.nTSC = tsc;
	//CHKRESULT(m_pRFTester->SetParameter(PT_TSC,param));
#endif
    return SP_OK;
}

SPRESULT CFunUIS8910::SetTCHPower(float Power)
{
#if 0
	char cmdbuffer[512];

	sprintf_s(cmdbuffer , "CONF:GPRF:MEAS:RFS:ENP %f",Power);
	WriteGpibCmd(cmdbuffer);

	WriteGpibCmd("CONF:GPRF:MEAS:RFS:UMAR 8");

	sprintf_s(cmdbuffer , "CONF:GSM:MEAS:RFS:ENP %f",Power);
	WriteGpibCmd(cmdbuffer);

	WriteGpibCmd("CONF:GSM:MEAS:RFS:UMAR 8");
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dExpPwr = Power;
	CHKRESULT(m_pRFTester->SetParameter(PT_EXP_PWR,param));
#endif
    return SP_OK;	
}

SPRESULT CFunUIS8910::SetCellPower(char band, float Power)
{
#if 0
    char cmdbuffer[512];

    char temp_band;
	temp_band = band;

	sprintf_s(cmdbuffer , "SOURce:GPRF:GEN:RFSettings:LEVel -%d",Power);
	WriteGpibCmd(cmdbuffer);
#else
	U_RF_PARAM param;
    char temp_band;
	temp_band = band;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dOptPwr = Power;
	CHKRESULT(m_pRFTester->SetParameter(PT_OPT_PWR,param));

#endif
    return SP_OK;	
}

SPRESULT CFunUIS8910::SetMeasureRxILoss(unsigned char SelPow)
{
#if 0
	char cmdbuffer[512];

	//WriteGpibCmd("ROUTe:GPRF:GEN:SCENario:SALone RF1C, TX1");

	sprintf_s(cmdbuffer , "SOURce:GPRF:GEN:RFSettings:LEVel -%d", SelPow);
	WriteGpibCmd(cmdbuffer);
	WriteGpibCmd("SOURce:GPRF:GEN:BBMode CW");
	WriteGpibCmd("SOURce:GPRF:GEN:STATe ON");
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dOptPwr = SelPow;
	CHKRESULT(m_pRFTester->InitTest(TI_FDT_RX,&param));
#endif
	Sleep(100);
	
	return SP_OK;
}


SPRESULT CFunUIS8910::SetRfPathLoss(float iloss)
{
    float temp_iloss;
	temp_iloss = iloss;

#if 0
    char cmdbuffer[512];

	sprintf_s(cmdbuffer , "CONF:GPRF:MEAS:RFS:EATT %f", iloss);
	WriteGpibCmd(cmdbuffer);
	
	sprintf_s(cmdbuffer , "SOUR:GPRF:GEN:RFS:EATT %f", iloss);
	WriteGpibCmd(cmdbuffer);

	sprintf_s(cmdbuffer , "CONF:GSM:MEAS:RFS:EATT %f", iloss);
	WriteGpibCmd(cmdbuffer);
#endif
	SetAnt(GSM_MAIN);
	
	return SP_OK;

}

SPRESULT CFunUIS8910::SetRxChannel(char band, int Arfcn)
{
#if 0
	float freq;
	char cmdbuffer[512];

	freq = ch2freq(band, (unsigned short)Arfcn, 1);
	sprintf_s(cmdbuffer , "SOURce:GPRF:GEN:RFSettings:FREQuency %fE+006", (freq+0.02));
	WriteGpibCmd(cmdbuffer);
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dDlFreq = ch2freq(band, (unsigned short)Arfcn, 1) + 0.02;
	CHKRESULT(m_pRFTester->SetParameter(PT_DOWNLINK_FREQ,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::GetFreqErr(float *m_freqerr,char band,char pcl)
{
#if 0
	int     loop   = 0;
	int     Indic  = 0;
	LPSTR  Modulationvalue[64] = {NULL};
	int     nModuCount = 0;
	int i = 0;

	do 
	{
		Sleep(10);
		WriteGpibCmd("READ:GSM:MEAS:MEV:MOD:AVERage?");
		ReadGpibCmd();
		GetTokenStrings(&gpib_InBuffer[0], Modulationvalue, ",", &nModuCount);
		if (nModuCount < 13) 
		{
			return SP_E_RF_STATE_ERROR;
		}

		Indic = atoi(Modulationvalue[0]);

		switch(Indic) 
		{
		case 0:

			*m_freqerr  = (float)strtod(Modulationvalue[10], NULL);   //_tcstod(Modulationvalue[10], NULL);

			for(i = 0; i < 64; i++)
			{
				Modulationvalue[i] = NULL;
			}
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetFreqErr success loop=%d",loop);
			return SP_OK;
			break;

		case 3:
			// Overflow
			{
				float targetpow = (float)strtod(Modulationvalue[12], NULL);

				SetTCHPower(targetpow+1);
			}
			break;
		default:
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetFreqErr fail loop=%d",loop);
			//return SP_E_RF_STATE_ERROR;
			break;
		}
	} while(++loop < 3);

	return SP_E_RF_STATE_ERROR;
#else

	FER_RLT rlt; 
	ZeroMemory(&rlt, sizeof(FER_RLT));
	rlt.dRmsPer = pcl2dbm(band, (char)pcl);
	CHKRESULT(m_pRFTester->FetchResult(TI_ULAFC_UIS8910, (LPVOID)&rlt));
    *m_freqerr = (float)(rlt.dAvgFer);
	return SP_OK;
#endif
}

SPRESULT CFunUIS8910::GetTxPower(float exppow,float *measurepow)
{
    m_pImp->LogFmtStrA( SPLOGLV_INFO, "%s: GetTxPower: Start!",__FUNCTION__);
#if 0
	int     loop   = 0;
	int     Indic  = 0;
	LPSTR  Modulationvalue[16] = {NULL};
	int     nModuCount = 0;
	int i = 0;
	float targetpow = (float)exppow;

	do 
	{
		WriteGpibCmd("READ:GSM:MEAS:MEV:MOD:AVERage?");
		ReadGpibCmd();
		GetTokenStrings(&gpib_InBuffer[0], Modulationvalue, ",", &nModuCount);
		if (nModuCount < 13) 
		{
			return SP_E_RF_STATE_ERROR;
		}

		Indic = atoi(Modulationvalue[0]);

		switch(Indic) 
		{
		case 0:
			*measurepow  = (float)strtod(Modulationvalue[12], NULL);   //_tcstod(Modulationvalue[12], NULL);
			for(i = 0; i < 16; i++)
				Modulationvalue[i] = NULL;
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetTxPower success loop=%d",loop);
			return SP_OK;
			break;

		case 3:
			// Overflow
			{
				targetpow += 6;
				SetTCHPower(targetpow);
			}
			break;
		default:
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetTxPower fail loop=%d",loop);
			//return SP_E_RF_STATE_ERROR;
			break;
		}
	} while(++loop < 4);

	return SP_E_RF_STATE_ERROR;
#else
	PWR_RLT rlt; 
	ZeroMemory(&rlt, sizeof(PWR_RLT));
	rlt.dAvgPwr = exppow;
	CHKRESULT(m_pRFTester->FetchResult(TI_PWR, (LPVOID)&rlt));
    *measurepow = (float)(rlt.dAvgPwr);
    m_pImp->LogFmtStrA( SPLOGLV_INFO, "%s: GetTxPower: End!",__FUNCTION__ );
	return SP_OK;
#endif
}

SPRESULT CFunUIS8910::GetTxOffsetPower(char band,int pcl,double *measurepow)
{
    m_pImp->LogFmtStrA( SPLOGLV_INFO, "%s: GetTxOffsetPower: Start!",__FUNCTION__ );
#if 0
	int     loop   = 0;
	int     Indic  = 0;
	LPSTR  Modulationvalue[16] = {NULL};
	int     nModuCount = 0;
	int i =0;
	float targetpow = (float)pcl2dbm(band, (char)pcl);

	do 
	{
		WriteGpibCmd("READ:GSM:MEAS:MEV:MOD:AVERage?");
		ReadGpibCmd();
		if (strstr(gpib_InBuffer, "INV")) 
		{
			Sleep(100);
			continue;
		}
		GetTokenStrings(&gpib_InBuffer[0], Modulationvalue, ",", &nModuCount);
		if (nModuCount < 13) 
		{
			return SP_E_RF_STATE_ERROR;
		}

		Indic = atoi(Modulationvalue[0]);

		switch(Indic) 
		{
		case 0:
			*measurepow  = (double)strtod(Modulationvalue[12], NULL);   //_tcstod(Modulationvalue[12], NULL);
			for(i = 0; i < 16; i++)
				Modulationvalue[i] = NULL;
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetTxOffsetPower success loop=%d",loop);
			return SP_OK;
			break;

		case 3:
			// Overflow
			{
				targetpow += 6;
				SetTCHPower(targetpow+6);
			}
			break;
		default:
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"GetTxOffsetPower fail loop=%d",loop);
			//return SP_E_RF_STATE_ERROR;
			break;
		}
	} while(++loop < 4);

	return SP_E_RF_STATE_ERROR;
#else
	PWR_RLT rlt; 
	ZeroMemory(&rlt, sizeof(PWR_RLT));
	rlt.dAvgPwr = pcl2dbm(band, (char)pcl);;
	CHKRESULT(m_pRFTester->FetchResult(TI_PWR, (LPVOID)&rlt));
    *measurepow = rlt.dAvgPwr;
    m_pImp->LogFmtStrA( SPLOGLV_INFO, "%s: GetTxOffsetPower: End!",__FUNCTION__);
	return SP_OK;
#endif
}

SPRESULT CFunUIS8910::SetDacScanFreq(char band, int channel, float &fPower)
{
#if 0
    char m_cmdbuffer[512];
    float freq;

    //if(iGlobalFlagBs == 4) 
	{
#if 0
		if(band == 0) 
		{
            m_cmw500.CalibSetGsm850ILoss();
        } 
		else if(band == 1) 
		{
            m_cmw500.CalibSetGsm900ILoss();
        } 
		else if(band == 2) 
		{
            m_cmw500.CalibSetDcsILoss();
        } 
		else if(band == 3) 
		{
            m_cmw500.CalibSetPcsILoss();
        }
#endif
        sprintf_s(m_cmdbuffer, "SOURce:GPRF:GEN:RFSettings:LEVel -%d",50);
		WriteGpibCmd(m_cmdbuffer);

        freq = ch2freq(band, (unsigned short)channel, 1);
        sprintf_s(m_cmdbuffer, "SOUR:GPRF:GEN:RFS:FREQ %4.1fE+006",freq);
        WriteGpibCmd(m_cmdbuffer);

        freq = ch2freq(band, (unsigned short)channel, 0);
        sprintf_s(m_cmdbuffer, "CONFigure:GPRF:MEAS:RFSettings:FREQuency %4.1fE+6", freq);
        WriteGpibCmd(m_cmdbuffer);

        sprintf_s(m_cmdbuffer, "CONF:GSM:MEAS:RFS:FREQ %fMHZ",freq);
        WriteGpibCmd(m_cmdbuffer);

        //sprintf_s(m_cmdbuffer, "CONF:GSM:MEAS:RFS:ENP %d",(int)(m_DacandPower[band][m_dacValuenum].m_fPower));
        sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:RFS:ENP %f",fPower);
        WriteGpibCmd(m_cmdbuffer);

		WriteGpibCmd( "CONF:GPRF:MEAS:RFS:UMAR 8");

        sprintf_s(m_cmdbuffer , "CONF:GSM:MEAS:RFS:ENP %f",fPower);
        WriteGpibCmd(m_cmdbuffer);

		WriteGpibCmd( "CONF:GSM:MEAS:RFS:UMAR 8");
    }
#else
	double dPower;
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	dPower = fPower;
	param.dDlFreq = ch2freq(band, (unsigned short)channel, 1);
	CHKRESULT(m_pRFTester->SetParameter(PT_DOWNLINK_FREQ,param));
	param.dUlFreq = ch2freq(band, (unsigned short)channel, 0);
	CHKRESULT(m_pRFTester->SetParameter(PT_UPLINK_FREQ,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::SetDacScanPwr(float &expPower)
{
#if 0
    char m_cmdbuffer[512];

	sprintf_s(m_cmdbuffer , "CONF:GPRF:MEAS:RFS:ENP %f",expPower);
    WriteGpibCmd(m_cmdbuffer);
	WriteGpibCmd("CONF:GPRF:MEAS:RFS:UMAR 8");  //8

    sprintf_s(m_cmdbuffer , "CONF:GSM:MEAS:RFS:ENP %f",expPower);
    WriteGpibCmd(m_cmdbuffer);
	WriteGpibCmd("CONF:GSM:MEAS:RFS:UMAR 8");  //8
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
	param.dExpPwr = expPower;
	CHKRESULT(m_pRFTester->SetParameter(PT_EXP_PWR,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::ResetTester(char band)
{
    char temp_band;
	temp_band = band;
#if 0
	WriteGpibCmd("*RST");
#else
	U_RF_PARAM param;
	ZeroMemory(&param, sizeof(U_RF_PARAM));
    CHKRESULT(m_pRFTester->SetParameter(PT_RESET,param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::NonsigTotalInitialize()
{
#if 0
    //m_Idisplayflag = 0;
    //ibtmo (h_BASE, T3s);
	SetAnt(GSM_MAIN);

    WriteGpibCmd("CONF:SELF:REF:FREQ:SOUR INT");
    //WriteGpibCmd("ROUTe:GPRF:GEN:SCENario:SALone RF1C,TX1");
    WriteGpibCmd("SOUR:GPRF:GEN:BBMode ARB");
    WriteGpibCmd("SOUR:GPRF:GEN:LIST OFF");
    WriteGpibCmd("SOUR:GPRF:GEN:ARB:REP CONT");
    //WriteGpibCmd("SOUR:GPRF:GEN:ARB:AUT ON");
    WriteGpibCmd("SOUR:GPRF:GEN:ARB:FILE 'D:\\Rohde-Schwarz\\CMW\\Data\\waveform\\test_FRSp_1326_prbs9.wv'");
    WriteGpibCmd("SOUR:GPRF:GEN:STAT ON");
    WriteGpibCmd("*OPC?");
    WriteGpibCmd("SOUR:GPRF:GEN:STAT?");
    //WriteGpibCmd("ROUTe:GPRF:MEAS:SCENario:SALone RF1C,RX1");
    //WriteGpibCmd("ROUT:GPRF:MEAS:RFS:CONN RF1C");
    WriteGpibCmd("CONFigure:GPRF:MEAS:RFSettings:MLOFfset 2");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:REP SING");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:TYPE GAUS");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:GAUS:BWID 1MHZ");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:SLEN 577.9230769E-6");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:MLEN 400E-6");
    WriteGpibCmd("CONF:GPRF:MEAS:POW:LIST OFF");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:SOUR 'IF Power'");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:SLOP REDG");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:THR -30");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:OFFS 50E-6");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:MGAP 0");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:MODE ALL");
    //WriteGpibCmd("ROUTe:GSM:MEAS:SCENario:SALone RF1C,RX1");
    //WriteGpibCmd("ROUT:GSM:MEAS:RFS:CONN RF1C");
    WriteGpibCmd("TRIG:GSM:MEAS:MEV:SOUR 'Power'");
    WriteGpibCmd("TRIG:GSM:MEAS:MEV:SLOP REDG");
    WriteGpibCmd("TRIG:GSM:MEAS:MEV:THR -30");
    WriteGpibCmd("TRIG:GSM:MEAS:MEV:TOUT 2");
    WriteGpibCmd("CONF:GSM:MEAS:MEV:REP SING");
    WriteGpibCmd("CONF:GSM:MEAS:MEV:SCON NONE");
    WriteGpibCmd("CONF:GSM:MEAS:MEV:SCO:MOD 5");
    WriteGpibCmd("CONF:GSM:MEAS:MEV:RES ON,OFF,OFF,ON,OFF,ON,OFF,ON,OFF,OFF,OFF,OFF");
    WriteGpibCmd("CONF:GSM:MEAS:MEV:TOUT 2");
	WriteGpibCmd("CONF:GSM:MEAS:MEV:TSEQ TSC0");   //TSC0 需要和下发给UE的一致

	//0926 HXY
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:SOUR 'IF Power'");//"IF Power"功率测量触发类型？FREE RUN,必须选IF POWER
	WriteGpibCmd("TRIG:GSM:MEAS:MEV:SLOP REDG");  //REDGe | FEDGe
	WriteGpibCmd("TRIG:GSM:MEAS:POW:THR -30"); //测最小功率时  需要改

    WriteGpibCmd("*OPC?");
    //ibrd (h_BASE, gpib_InBuffer, sizeof(gpib_InBuffer));
	ReadGpibCmd();
#else
	INIT_PARAM param;
	ZeroMemory(&param, sizeof(INIT_PARAM));
    CHKRESULT(m_pRFTester->InitDev(TM_NON_SIGNAL_FINAL, SUB_INVALID, &param));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::NonsigInitialize(char band,int tch, int bch, int pwr,  float iLoss)
{
#if 0
    float m_freq = 0;
	char cmdbuffer[512];

	int temp_bch;
	char temp_pcl;
	temp_bch = bch;
	temp_pcl = pcl;

    if (0 == band) 
	{
		 WriteGpibCmd("CONF:GSM:MEAS:BAND G085");
       // m_pImp->LogFmtStrA(SPLOGLV_INFO,"---------->        GSM850 NonsigInitialize      ");
    } 
	else if(1 == band) 
	{
		WriteGpibCmd("CONF:GSM:MEAS:BAND G09");
       // m_pImp->LogFmtStrA(SPLOGLV_INFO,"---------->        GSM900 NonsigInitialize      ");
    } 
	else if(2 == band) 
	{
		WriteGpibCmd("CONF:GSM:MEAS:BAND G18");
       // m_pImp->LogFmtStrA(SPLOGLV_INFO,"---------->        DCS NonsigInitialize      ");
    } 
	else if(3 == band) 
	{
		WriteGpibCmd("CONF:GSM:MEAS:BAND G19");
       // m_pImp->LogFmtStrA(SPLOGLV_INFO,"---------->        PCS NonsigInitialize      ");
    }


	sprintf_s(cmdbuffer , "SOURce:GPRF:GEN:RFSettings:LEVel -%d",50);
	WriteGpibCmd(cmdbuffer);

    m_freq = ch2freq(band, (unsigned short)tch, 1);
	sprintf_s(cmdbuffer , "SOUR:GPRF:GEN:RFS:FREQ %4.1fE+006",m_freq);
	WriteGpibCmd(cmdbuffer);

    m_freq = ch2freq(band, (unsigned short)tch, 0);
	sprintf_s(cmdbuffer , "CONFigure:GPRF:MEAS:RFSettings:FREQuency %4.1fE+6", m_freq);
	WriteGpibCmd(cmdbuffer);

    //WriteGpibCmd("CONF:GSM:MEAS:RFS:FREQ %fMHZ",m_freq);
	sprintf_s(cmdbuffer ,"CONF:GSM:MEAS:CHAN %d",tch);
	WriteGpibCmd(cmdbuffer);
    if(band < 2) 
	{
        WriteGpibCmd("CONF:GSM:MEAS:RFS:ENP 33");
    } 
	else 
	{
        WriteGpibCmd("CONF:GSM:MEAS:RFS:ENP 30");
    }

    WriteGpibCmd("CONF:GSM:MEAS:RFS:UMAR 8");
    SetRfPathLoss(iLoss);
    WriteGpibCmd("*OPC?");
    //ibrd (h_BASE, gpib_InBuffer, sizeof(gpib_InBuffer));
	ReadGpibCmd();
#else
	int temp_bch;
	float temp_iLoss;
	temp_bch = bch;
	temp_iLoss = iLoss;

	INIT_PARAM param;
	ZeroMemory(&param, sizeof(INIT_PARAM));
    if (0 == band) 
	{
		param.eBand = BI_GSM_850;
    } 
	else if(1 == band) 
	{
		param.eBand = BI_EGSM;
    } 
	else if(2 == band) 
	{
	    param.eBand = BI_DCS;
    } 
	else if(3 == band) 
	{
	    param.eBand = BI_PCS;
    }
	param.dCellPower = 50;
	param.dTargetPower = pwr;
	param.dDlfreq = ch2freq(band, (unsigned short)tch, 1);
	param.dUlfreq = ch2freq(band, (unsigned short)tch, 0);
	param.nUlChan = tch;
    CHKRESULT(m_pRFTester->InitTest( TI_LMT, &param ));

#endif
	return SP_OK;

}

SPRESULT CFunUIS8910::NSFTSetup(char band, int channel)
{
    char temp_band;
    int temp_channel;
	temp_band = band;
	temp_channel = channel;

    return SP_OK;
}

SPRESULT CFunUIS8910::NsftMeasureTxPower(char band, int pcl, int power, float *pPwrUse)
{
#if 0
    int txpowerdbm;
	char cmdbuffer[512];

    txpowerdbm = pcl2dbm(band, (char)pcl);
    WriteGpibCmd("CONF:GPRF:MEAS:POW:SCO 10");
    WriteGpibCmd("TRIG:GPRF:MEAS:POW:TOUT 2");
    sprintf_s(cmdbuffer ,"CONFigure:GPRF:MEAS:RFSettings:ENPower %d", txpowerdbm);
	WriteGpibCmd(cmdbuffer);
    //WriteGpibCmd("CONFigure:GPRF:MEAS:RFSettings:ENPower %d", txpowerdbm);
    WriteGpibCmd("CONFigure:GPRF:MEAS:RFSettings:UMARgin 5");
    WriteGpibCmd("INIT:GPRF:MEAS:POW");
    WriteGpibCmd("READ:GPRF:MEAS:POWer:RMS:AVERage?");
    ReadGpibCmd();
    //if(gpib_InBuffer[0] == 'N' && gpib_InBuffer[1] == 'A' && gpib_InBuffer[2] == 'N') {
    if (strstr(gpib_InBuffer, "INV")) 
    {
//       DisplayTxPowerCode(band);
        return SP_E_RF_STATE_ERROR;
    }
    BufferChangeFormat();

	sscanf_s(gpib_InBuffer, "%*e %e", pPwrUse);
    //sscanf_s(gpib_InBuffer, "%*e %e", &m_Pow);
    //  if(PrintTxPower(band, pcl))
    //      return -1;
    //power = m_Pow;
	power = *pPwrUse;
#else
	char temp_band;
	int temp_pcl;
	temp_band = band;
	temp_pcl = pcl;
	PWR_RLT rlt; 
	ZeroMemory(&rlt, sizeof(PWR_RLT));
	rlt.dAvgPwr = power;
	CHKRESULT(m_pRFTester->FetchResult(TI_TXP, (LPVOID)&rlt));
	*pPwrUse = (float)(rlt.dAvgPwr);
#endif
    return SP_OK;
}

SPRESULT CFunUIS8910::TestModulation(unsigned char band, unsigned int tch)
{
    float mwflag = 1.0f;
#if 0
    char m_tempbuf[128];

    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:SMODulation 3");
    WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
    WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:SMODulation?");
    ReadGpibCmd();
    if (strstr(gpib_InBuffer, "INV")) 
	{
        Sleep(100);
        WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:SMODulation 3");
        WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
        WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:SMODulation?");
        ReadGpibCmd();
        if (strstr(gpib_InBuffer, "INV")) 
		{
            DisplayORFSMCode(band);
            return SP_E_RF_STATE_ERROR;
        }
    }
    BufferChangeFormat();
	sscanf_s(gpib_InBuffer, "%*e %e %*e", &mwflag);

    if((int)mwflag == 0)
	{
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             PASS" , "ORFSM");
	}
    else 
	{
        //AfxMessageBox("stop");
        Sleep(100);
        WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:SMODulation 3");
        WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
        WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:SMODulation?");
        ReadGpibCmd();
        BufferChangeFormat();
        sscanf_s(gpib_InBuffer, "%*e %e %*e", &mwflag);
        if((int)mwflag == 0)
		{
            m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             PASS" , "ORFSM");
		}
        else 
		{
            sprintf_s(m_tempbuf , "%-20s:             FAIL" , "ORFSM");
            DisplayORFSMCode(band);

            m_pImp->LogFmtStrA(SPLOGLV_INFO,m_tempbuf);

			m_pImp->NOTIFY("GSM NST(Modulation Spectrum)", 
				LEVEL_ITEM, //LEVEL_UI,  
				0, 
				(int)mwflag, 
				0, 
				CgsmUtility::GSM_BAND_NAME[band], 
				tch, //nChannel, 
				NULL
				);

            return SP_E_RF_STATE_ERROR;
        }
    }
#else
	ORFS_RLT rlt; 
	ZeroMemory(&rlt, sizeof(ORFS_RLT));
	rlt.nIndicator = 0xa;
	CHKRESULT(m_pRFTester->FetchResult(TI_ORFS, (LPVOID)&rlt));
    mwflag = (float)(rlt.nModLimit);
    if((int)mwflag == 0)
	{
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             PASS" , "ORFSM");
	}
    else 
	{
        DisplayORFSMCode(band);
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             FAIL" , "ORFSM");

		m_pImp->NOTIFY("GSM NST(Modulation Spectrum)", 
			LEVEL_ITEM, //LEVEL_UI,  
			0, 
			(int)mwflag, 
			0, 
			CgsmUtility::GSM_BAND_NAME[band], 
			tch, //nChannel, 
			NULL
			);

            return SP_E_RF_STATE_ERROR;
        }
#endif
	m_pImp->NOTIFY("GSM NST(Modulation Spectrum)", 
		LEVEL_ITEM, //LEVEL_UI,  
		0, 
		(int)mwflag, 
		0, 
		CgsmUtility::GSM_BAND_NAME[band], 
		tch, //nChannel, 
		NULL
		);

    return SP_OK;
}

SPRESULT CFunUIS8910::TestSwSpec(unsigned char band, unsigned int tch)
{
    float swflag = 1.0f;
#if 0
    //char m_tempbuf[128];
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:SSW 3");
    WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
    WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:SSW?");
    ReadGpibCmd();
    if (strstr(gpib_InBuffer, "INV")) 
	{
        DisplayORFSSCode(band);
        return SP_E_RF_STATE_ERROR;
    }
    BufferChangeFormat();
    sscanf_s(gpib_InBuffer, "%*e %e %*e",&swflag);
#else
	ORFS_RLT rlt; 
	ZeroMemory(&rlt, sizeof(ORFS_RLT));
	rlt.nIndicator = 0xb;
	CHKRESULT(m_pRFTester->FetchResult(TI_ORFS, (LPVOID)&rlt));
    swflag = (float)(rlt.nSwtLimit);
#endif

	m_pImp->NOTIFY("GSM NST(SwitchSpectrum)", 
		LEVEL_ITEM, //LEVEL_UI, 
		0, 
		(int)swflag, 
		0, 
		CgsmUtility::GSM_BAND_NAME[band], 
		tch, //nChannel, 
		NULL
		);

    if((int)swflag == 0)
	{
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             PASS" , "ORFSS");
	}
    else 
	{
		m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             FAIL" , "ORFSS");
        DisplayORFSSCode(band);
        return SP_E_RF_STATE_ERROR;
    }

    return SP_OK;
}

SPRESULT CFunUIS8910::NsftMeasurePVT(char band, unsigned int tch)
{
    float flag;
	float m_pvtmask;
#if 0
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:TOUT 1.5");
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:PVTime 100");
    WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
    WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:PVTime?");
    ReadGpibCmd();
    if (strstr(gpib_InBuffer, "INV")) 
	{
        DisplayPVTCode(band);
        return SP_E_RF_STATE_ERROR;
    }
    BufferChangeFormat();

    sscanf_s(gpib_InBuffer, "%e %e", &flag, &m_pvtmask);
//  m_pImp->LogFmtStrA(SPLOGLV_INFO,"pvt flag = %f m_pvtmask = %f",flag,m_pvtmask);
#else
	PVT_RLT rlt; 
	ZeroMemory(&rlt, sizeof(PVT_RLT));
	CHKRESULT(m_pRFTester->FetchResult(TI_PVT, (LPVOID)&rlt));
    flag = (float)(rlt.nIndicator);
    m_pvtmask = (float)(rlt.nPass);
#endif

	m_pImp->NOTIFY("GSM NST(PVT)", 
		LEVEL_ITEM, //LEVEL_UI, 
		0, 
		(int)m_pvtmask, 
		0, 
		CgsmUtility::GSM_BAND_NAME[band], 
		tch, //nChannel, 
		NULL
		);

	if(((int)m_pvtmask == 0)&&((int)flag == 0))
	{
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             PASS" , "PVT");
	}
    else 
	{
        m_pImp->LogFmtStrA(SPLOGLV_INFO,"%-20s:             FAIL" , "PVT");
		DisplayPVTCode(band);

		return SP_E_RF_STATE_ERROR;
    }

    return SP_OK;
}

SPRESULT CFunUIS8910::NsftMeasurePhaseNFrequency(char band, float *pFreq)
{
    float flag;
    char temp_band;
    temp_band = band;
#if 0
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:MODulation:DECode GTBits");
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:TOUT 1.5");
    WriteGpibCmd("CONFigure:GSM:MEAS:MEValuation:SCOunt:MODulation 10");
    WriteGpibCmd("INIT:GSM:MEAS:MEValuation");
    WriteGpibCmd("FETCh:GSM:MEAS:MEValuation:MODulation:AVERage?");
    ReadGpibCmd();

    if (strstr(gpib_InBuffer, "INV")) 
	{
        DisplayPFCode(band);
        return SP_E_RF_STATE_ERROR;
    }
    BufferChangeFormat();
    //sscanf_s(gpib_InBuffer, "%e %*e %*s %*s %*s %*s %e %e %*e %*e %e", &flag, &m_frequency[2],&m_frequency[1],&m_frequency[0]);
	sscanf_s(gpib_InBuffer, "%e %*e %*s %*s %*s %*s %e %e %*e %*e %e", &flag, &pFreq[2],&pFreq[1],&pFreq[0]);
#else
	FER_RLT rlt; 
	ZeroMemory(&rlt, sizeof(FER_RLT));
	CHKRESULT(m_pRFTester->FetchResult(TI_FER, (LPVOID)&rlt));
    flag = (float)(rlt.nIndicator);
	pFreq[0] = (float)(rlt.dAvgFer);  //Freq Error
	pFreq[1] = (float)(rlt.dPeakPer); //Phase Error
	pFreq[2] = (float)(rlt.dRmsPer);  //RMS Error
#endif

#if 0 //chengui  //放到外面做判断
    if(PrintPFerror(band) == -1)  
	{
        return SP_E_RF_STATE_ERROR;
	}
#endif

    return SP_OK;
}


SPRESULT CFunUIS8910::SetGSMFastCaliUTSParam(unsigned char band,unsigned short channel, int frame_num)
{
#if 0
	int i = 0;
	float pcldbm = 0.0f;
	char m_cmdbuffer[4096];
    int temp_frame_num;
    temp_frame_num = frame_num;

	if(band == 0) 
	{
		//      CalibSetGsm850ILoss();
		pcldbm = 36.0f;
	} 
	else if(band == 1) 
	{
		//      CalibSetGsm900ILoss();
		pcldbm = 36.0f;
	}
	else if(band == 2) 
	{
		//      CalibSetDcsILoss();
		pcldbm = 34.0f;
	} 
	else if(band == 3) 
	{
		//      CalibSetPcsILoss();
		pcldbm = 34.0f;
	}

	int nTestSlotCount = frame_num*8 - 1;

	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:STOP %d", nTestSlotCount);
	WriteGpibCmd(m_cmdbuffer);

	int nFramesIndex;
	float dMHz;
	char tembuff[100];
	//char m_strTxFreqSet[1024] = "";
	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:FREQ:ALL ");
	for (nFramesIndex=0; nFramesIndex < frame_num; nFramesIndex++) 
	{
		dMHz = ch2freq(band, channel, 0);
		for(int j=0; j<8; j++) 
		{
			memset(tembuff,0,100);
			sprintf_s(tembuff, "%.1f MHz,", dMHz);
			//m_strTxFreqSet += tembuff;
			strcat_s(m_cmdbuffer, tembuff);
		}
	}


	//m_strTxFreqSet.SetAt((m_strTxFreqSet.GetLength()-1), '\0');  // remove the last ',' character
	m_cmdbuffer[strlen(m_cmdbuffer)-1] = 0;
	//sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:FREQ:ALL %s",m_strTxFreqSet);
	WriteGpibCmd(m_cmdbuffer);

	//set ENP
	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:ENP:ALL ");
	//CString strExpPwrBuff;
	for (i=0; i<frame_num; i++) 
	{
		if(band == 0)
		{
			//      CalibSetGsm850ILoss();
			pcldbm = 36.0f;
		} 
		else if(band == 1) 
		{
			//      CalibSetGsm900ILoss();
			pcldbm = 36.0f;
		} 
		else if(band == 2) 
		{
			//      CalibSetDcsILoss();
			pcldbm = 34.0f;
		} 
		else if(band == 3) 
		{
			//      CalibSetPcsILoss();
			pcldbm = 34.0f;
		}

		double dExpPwr = (band < 2) ? 38.0 : 35.0;
		double dDelta[8] = {0,5,10,10,20,20,20,20};

		for(int k=0;k<8;k++)
		{
			dDelta[k] -= (i/2)*5;
			//if((((0 == i)||(1 == i)))&&(3 == k))
			//{
			    //dDelta[k] += 15;
			//}
			if(dDelta[k] < 0)
			{
				dDelta[k] = 0;
			}
		}

		for (int j=0; j<8; j++) 
		{
			if (pcldbm < 20.0 && pcldbm >= 10.0) 
			{
				dExpPwr = 25.0 - dDelta[j];
			} 
			else if (pcldbm < 10.0 && pcldbm >= 0.00) 
			{
				dExpPwr = 10.0 - dDelta[j];
			}
			else if (pcldbm < 0.0) 
			{
				dExpPwr = 5.0 - dDelta[j];
			}

			memset(tembuff,0,100);
            sprintf_s(tembuff, "%.0f,", dExpPwr);
			//m_strTxFreqSet += tembuff;
			strcat_s(m_cmdbuffer, tembuff);

			//strExpPwrBuff.Format("%.0f,", dExpPwr);
			//strExpPwrList += strExpPwrBuff;
			pcldbm -= 10;
		}
	}
	//strExpPwrList.SetAt((strExpPwrList.GetLength()-1), '\0');  // remove the last ',' character
	//sprintf_s(m_cmdbuffer, "%s",strExpPwrList);
	m_cmdbuffer[strlen(m_cmdbuffer)-1] = 0;
	WriteGpibCmd(m_cmdbuffer);

	// Start
	WriteGpibCmd("INIT:GPRF:MEAS:POW;*OPC?");
	//
	//  Sleep(200);

	DWORD dwTick = GetTickCount();
	bool  bRun   = false;
	do 
	{
		WriteGpibCmd("FETC:GPRF:MEAS:POW:STAT?");
		ReadGpibCmd();
		if (NULL != strstr(gpib_InBuffer, "RUN")) 
		{
			bRun = true;

			Sleep(100);
			break;
		} 
		else 
		{
			Sleep(10);
		}

	} while(GetTickCount() - dwTick < 2000);
	if (!bRun) 
	{
		return SP_E_FAIL;
	}

#else

	float pcldbm = 0.0f;
	int slot_num = 8;
	uint32 slotmask = 0xF;

    // Setup FDT parameters of RF Instrument
    INSTR_GSM_FDT_TX_PARAM InstrCfg;
    ZeroMemory((void *)&InstrCfg, sizeof(InstrCfg));
    InstrCfg.frame_count = frame_num;

	for (int nFrameIndex=0; nFrameIndex < frame_num; nFrameIndex++) 
	{
        InstrCfg.frame[nFrameIndex].eBand     = (SP_BAND_INFO)band;
        InstrCfg.frame[nFrameIndex].arfcn     = channel;
        InstrCfg.frame[nFrameIndex].slotmask  = slotmask;
	}

	//set ENP
	for (int nFrameIndex=0; nFrameIndex < frame_num; nFrameIndex++) 
	{
		if(band == 0)
		{
			pcldbm = 36.0f;
		} 
		else if(band == 1) 
		{
			pcldbm = 36.0f;
		} 
		else if(band == 2) 
		{
			pcldbm = 34.0f;
		} 
		else if(band == 3) 
		{
			pcldbm = 34.0f;
		}

		double dExpPwr = (band < 2) ? 38.0 : 35.0;
		double dDelta[8] = {0,5,10,10,20,20,20,20};

		for (int nSlotIndex=0; nSlotIndex < slot_num; nSlotIndex++) 
		{
			dDelta[nSlotIndex] -= (nFrameIndex/2)*5;
			if(dDelta[nSlotIndex] < 0)
			{
				dDelta[nSlotIndex] = 0;
			}

			if (pcldbm < 20.0 && pcldbm >= 10.0) 
			{
				dExpPwr = 25.0 - dDelta[nSlotIndex];
			} 
			else if (pcldbm < 10.0 && pcldbm >= 0.00) 
			{
				dExpPwr = 15.0 - dDelta[nSlotIndex];
			}
			else if (pcldbm < 0.0) 
			{
				dExpPwr = 10.0 - dDelta[nSlotIndex];
			}

            InstrCfg.frame[nFrameIndex].dExpPwr[nSlotIndex] =  ((float)dExpPwr);

			pcldbm -= 10;
		}
	}

    InstrCfg.trigger = (float)InstrCfg.frame[0].dExpPwr[0];
	CHKRESULT(m_pRFTester->InitTest(TI_FDT,(LPCVOID)&InstrCfg));
#endif
	return SP_OK;
}


SPRESULT CFunUIS8910::GetGSMFastCaliULResult(unsigned char band,volatile float *powbuffer)
{
#if 0
	DWORD dwStartTick = GetTickCount();
	bool  bSweepReady = false;
	//  char  m_cmdbuffer[1024];
	// Do with the results
	int     i = 0;
	float  m_TxPwr[8*2*5] = {0.0f};  // 7 slot, 2 same frame, 5 sque
	unsigned char temp_band;
	temp_band = band;

	//LPTSTR *m_pTxPowResult  = (LPTSTR *)new TCHAR[sizeof(LPTSTR)*1000];
	LPSTR  m_pTxPowResult[1000] = {NULL};
	int     nPowerCount = 0;

	do 
	{
		WriteGpibCmd("FETC:GPRF:MEAS:POW:STAT?");

		ReadGpibCmd();
		if (NULL != strstr(gpib_InBuffer, "RDY")) 
		{
			bSweepReady = true;
			break;
		} 
		else 
		{
			Sleep(185);
		}

	} while(GetTickCount() - dwStartTick < 3000);

	if (!bSweepReady) 
	{
		return SP_E_FAIL;
	}

	Sleep(185);
	WriteGpibCmd("FETC:GPRF:MEAS:POW:AVER?");
	ReadGpibCmd();

	GetTokenStrings(gpib_InBuffer, m_pTxPowResult, ",", &nPowerCount);

	for (i=0; i<8*2*5; i++) 
	{ // 8 data, 2frame, 5 sque
		//m_TxPwr[i] = (float)_tcstod(m_pTxPowResult[i], NULL);
		m_TxPwr[i]  = ((float)(atof(m_pTxPowResult[i])));

	}
#else

	float  m_TxPwr[8*2*5] = {0.0f};  // 7 slot, 2 same frame, 5 sque
	unsigned char temp_band;
	temp_band = band;

    INSTR_GSM_FDT_TX_RLT rlt;
    ZeroMemory(&rlt, sizeof(rlt));

	CHKRESULT(m_pRFTester->FetchResult(TI_FDT, (LPVOID)&rlt));
	for (int i=0; i<8*2*5; i++) 
	{ // 8 data, 2frame, 5 sque
		//m_TxPwr[i] = (float)_tcstod(m_pTxPowResult[i], NULL);
		m_TxPwr[i]  = ((float)(rlt.dPwr[i]));
	}
#endif

	for(int i=0, kk = 0; kk < 40;) 
	{
		powbuffer[15]   = m_TxPwr[kk];
		powbuffer[10+i] = m_TxPwr[kk+1];
		powbuffer[5+i]  = m_TxPwr[kk+2];
		powbuffer[i]    = m_TxPwr[kk+3];
		kk = kk + 8;
		i++;
	}

	return SP_OK;
}

SPRESULT CFunUIS8910::SetGSMFastoffsetCaliUTSParam(unsigned char band)
{
#if 0
	int i = 0;
	float pcldbm = 0.0f;
	char m_cmdbuffer[4096];
	short arfcnMax = 0, arfcnMin = 0;

	if(band == 0) 
	{
		arfcnMin = 128;
		arfcnMax = 251;
		pcldbm = 36.0f;
	} 
	else if(band == 1) 
	{
		arfcnMin = 1;
		arfcnMax = 124;
		pcldbm = 36.0f;
	} 
	else if(band == 2) 
	{
		arfcnMin = 512;
		arfcnMax = 885;
		pcldbm = 33.0f;
	} 
	else if(band == 3) 
	{
		arfcnMin = 512;
		arfcnMax = 810;
		pcldbm = 33.0f;
	}

	int nTestSlotCount = 10*8 - 1;

	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:STOP %d", nTestSlotCount);
	WriteGpibCmd(m_cmdbuffer);

	int nFramesIndex;
	float dMHz;
	char tembuff[100];
	//CString m_strTxFreqSet = "";
	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:FREQ:ALL ");
	for (nFramesIndex=0; nFramesIndex < 8; nFramesIndex++) 
	{
		dMHz = ch2freq(band, arfcnMin, 0);
		for(int j=0; j<8; j++) 
		{
			memset(tembuff,0,100);
			sprintf_s(tembuff, "%.1f MHz,", dMHz);
			//m_strTxFreqSet += tembuff;
			strcat_s(m_cmdbuffer, tembuff);
		}
	}
	for (nFramesIndex=8; nFramesIndex < 10; nFramesIndex++) 
	{
		dMHz = ch2freq(band, arfcnMax, 0);
		for(int j=0; j<8; j++) 
		{
			memset(tembuff,0,100);
			sprintf_s(tembuff, "%.1f MHz,", dMHz);
			//m_strTxFreqSet += tembuff;
			strcat_s(m_cmdbuffer, tembuff);
		}
	}

	//m_strTxFreqSet.SetAt((m_strTxFreqSet.GetLength()-1), '\0');  // remove the last ',' character

	//sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:FREQ:ALL %s",m_strTxFreqSet);
	m_cmdbuffer[strlen(m_cmdbuffer)-1] = 0;
	WriteGpibCmd(m_cmdbuffer);

	//set ENP
	//CString strExpPwrList = "CONF:GPRF:MEAS:POW:LIST:ENP:ALL ";
	//CString strExpPwrBuff;
	memset(m_cmdbuffer,0,4096);
	sprintf_s(m_cmdbuffer, "CONF:GPRF:MEAS:POW:LIST:ENP:ALL ");
	for (i=0; i<10; i++) 
	{
		if(band == 0) 
		{
			pcldbm = 36.0f;
		} 
		else if(band == 1) 
		{
			pcldbm = 36.0f;
		} 
		else if(band == 2) 
		{
			pcldbm = 34.0f;
		} 
		else if(band == 3) 
		{
			pcldbm = 34.0f;
		}

		double dExpPwr = (band < 2) ? 38.0 : 35.0;

		for (int j=0; j<8; j++) 
		{
			if (pcldbm < 20.0 && pcldbm >= 10.0) 
			{
				dExpPwr = 25.0;
			} 
			else if (pcldbm < 10.0 && pcldbm >= 0.00) 
			{
				dExpPwr = 15.0;
			}
			else if (pcldbm < 0.0) 
			{
				dExpPwr = 5.0;
			}

			//strExpPwrBuff.Format("%.0f,", dExpPwr);
			//strExpPwrList += strExpPwrBuff;
			memset(tembuff,0,100);
            sprintf_s(tembuff, "%.0f,", dExpPwr);
			strcat_s(m_cmdbuffer, tembuff);
			pcldbm -= 10;
		}
	}
	//strExpPwrList.SetAt((strExpPwrList.GetLength()-1), '\0');  // remove the last ',' character
	//sprintf_s(m_cmdbuffer, "%s",strExpPwrList);
	m_cmdbuffer[strlen(m_cmdbuffer)-1] = 0;
	WriteGpibCmd(m_cmdbuffer);

	// Start

	WriteGpibCmd("INIT:GPRF:MEAS:POW;*OPC?");
	//
	//  Sleep(200);

	DWORD dwTick = GetTickCount();
	bool  bRun   = false;
	do 
	{
		WriteGpibCmd("FETC:GPRF:MEAS:POW:STAT?");
		ReadGpibCmd();
		if (NULL != strstr(gpib_InBuffer, "RUN")) 
		{
			bRun = true;

			Sleep(100);
			break;
		} 
		else 
		{
			Sleep(10);
		}

	} while(GetTickCount() - dwTick < 2000);
	if (!bRun) 
	{

		return SP_E_FAIL;
	}
#else

    char gsm_pcl[10][8] = {{5,9, 13,17,17,17,17,17},
						  {5,9, 13,17,17,17,17,17},
						  {6,10,14,18,18,18,18,18},
						  {6,10,14,18,18,18,18,18},
						  {7,11,15,19,19,19,19,19},
						  {7,11,15,19,19,19,19,19},
						  {8,12,16,20,20,20,20,20},
						  {8,12,16,20,20,20,20,20},
						  {5,6, 13,20,20,20,20,20},
	                      {5,6, 13,20,20,20,20,20}};
    char dcs_pcl[10][8] = {{0,4, 8, 12,12,12,12,12},
						  {0,4, 8, 12,12,12,12,12},
						  {1,5, 9, 13,13,13,13,13},
						  {1,5, 9, 13,13,13,13,13},
						  {2,6, 10,14,14,14,14,14},
						  {2,6, 10,14,14,14,14,14},
						  {3,7, 11,15,15,15,15,15},
						  {3,7, 11,15,15,15,15,15},
						  {0,1, 8, 15,15,15,15,15},
	                      {0,1, 8, 15,15,15,15,15}};
	short arfcnMax = 0;
	short arfcnMin = 0;
	int frame_num = 10;
	float dExpPwr = 0.0;
	float dmargin = 5.0;
	int slot_num = 8;
	uint32 slotmask = 0xF;

    // Setup FDT parameters of RF Instrument
    INSTR_GSM_FDT_TX_PARAM InstrCfg;
    ZeroMemory((void *)&InstrCfg, sizeof(InstrCfg));
    InstrCfg.frame_count = frame_num;

	if(band == 0) 
	{
		arfcnMin = 128;
		arfcnMax = 251;
	} 
	else if(band == 1) 
	{
		arfcnMin = 1;
		arfcnMax = 124;
	} 
	else if(band == 2) 
	{
		arfcnMin = 512;
		arfcnMax = 885;
	} 
	else if(band == 3) 
	{
		arfcnMin = 512;
		arfcnMax = 810;
	}

	for (int nFrameIndex=0; nFrameIndex < (frame_num - 2); nFrameIndex++) 
	{
        InstrCfg.frame[nFrameIndex].eBand     = (SP_BAND_INFO)band;
        InstrCfg.frame[nFrameIndex].arfcn     = arfcnMin;
        InstrCfg.frame[nFrameIndex].slotmask  = slotmask;
	}

	for (int nFrameIndex=8; nFrameIndex < frame_num; nFrameIndex++) 
	{
        InstrCfg.frame[nFrameIndex].eBand     = (SP_BAND_INFO)band;
        InstrCfg.frame[nFrameIndex].arfcn     = arfcnMax;
        InstrCfg.frame[nFrameIndex].slotmask  = slotmask;
	}

	//set ENP
	for (int nFrameIndex=0; nFrameIndex < frame_num; nFrameIndex++) 
	{
		for (int nSlotIndex=0; nSlotIndex < slot_num; nSlotIndex++) 
		{
			if(band < 2) 
	        {
				dExpPwr = (float)pcl2dbm(band, gsm_pcl[nFrameIndex][nSlotIndex]) + dmargin;
			}
			else
			{
                dExpPwr = (float)pcl2dbm(band, dcs_pcl[nFrameIndex][nSlotIndex]) + dmargin;
			}
            InstrCfg.frame[nFrameIndex].dExpPwr[nSlotIndex] =  ((float)dExpPwr);
		}
	}

    InstrCfg.trigger = (float)InstrCfg.frame[0].dExpPwr[0];
	CHKRESULT(m_pRFTester->InitTest(TI_FDT,(LPCVOID)&InstrCfg));

#endif
	return SP_OK;

}

SPRESULT CFunUIS8910::GetFastCaliboffsetUTSResult(unsigned char band,volatile float* measured_pow)
{
#if 0
	int pclNum;
	int pclToSkip;
	bool  bSweepReady = false;
	DWORD dwStartTick = 0;
	int     i = 0;
	float  m_TxPwr[8*2*5] = {0};  // 7 slot, 2 same frame, 5 sque
	//LPTSTR *m_pTxPowResult  = (LPTSTR *)new TCHAR[sizeof(LPTSTR)*1000];
	LPSTR  m_pTxPowResult[1000] = {NULL};
	int     nPowerCount = 0;

	if (3 == band) 
	{
		pclNum = 18;
		pclToSkip = 2;
	} 
	else if (2 == band) 
	{
		pclNum = 17;
		pclToSkip = 1;
	} 
	else if (1 == band) 
	{
		pclNum = 15;
		pclToSkip = 0;
	} 
	else 
	{
		pclNum = 15;
		pclToSkip = 0;
	}
	dwStartTick = GetTickCount();

	do 
	{
		WriteGpibCmd("FETC:GPRF:MEAS:POW:STAT?");

		ReadGpibCmd();
		if (NULL != strstr(gpib_InBuffer, "RDY")) 
		{
			bSweepReady = true;
			break;
		} 
		else 
		{
			Sleep(185);
		}
	} while(GetTickCount() - dwStartTick < 3000);

	if (!bSweepReady) {
		return 0;
	}


	Sleep(185);
	WriteGpibCmd("FETC:GPRF:MEAS:POW:AVER?");
	ReadGpibCmd();

	// Do with result

	GetTokenStrings(gpib_InBuffer, m_pTxPowResult, ",", &nPowerCount);
	for (i=0; i<8*2*5; i++) 
	{ // 7 data, 2frame, 5 sque
		//m_TxPwr[i] = (float)_tcstod(m_pTxPowResult[i], NULL);
		m_TxPwr[i] = ((float)(atof(m_pTxPowResult[i])));
	}
#else

	int pclNum;
	int pclToSkip;
	float  m_TxPwr[8*2*5] = {0.0f};  // 8 slot, 2 same frame, 5 sque

	INSTR_GSM_FDT_TX_RLT rlt;
    ZeroMemory(&rlt, sizeof(rlt));

	CHKRESULT(m_pRFTester->FetchResult(TI_FDT, (LPVOID)&rlt));
	for (int i=0; i<8*2*5; i++) 
	{ // 8 data, 2frame, 5 sque
		m_TxPwr[i]  = ((float)(rlt.dPwr[i]));
	}
	if (3 == band) 
	{
		pclNum = 18;
		pclToSkip = 2;
	} 
	else if (2 == band) 
	{
		pclNum = 17;
		pclToSkip = 1;
	} 
	else if (1 == band) 
	{
		pclNum = 15;
		pclToSkip = 0;
	} 
	else 
	{
		pclNum = 15;
		pclToSkip = 0;
	}
#endif

	for(int	i = 0, kk = 0; kk < 32;) 
	{
		measured_pow[i] = m_TxPwr[kk];
		measured_pow[4+i] = m_TxPwr[kk+1];
		measured_pow[8+i] = m_TxPwr[kk+2];
		measured_pow[12+i] = m_TxPwr[kk+3];
		kk = kk + 8;
		i++;
	}
	measured_pow[pclNum] = m_TxPwr[32];
	measured_pow[pclNum+1] = m_TxPwr[33];
	measured_pow[pclNum*2-2-pclToSkip] = m_TxPwr[34];
	measured_pow[pclNum*2-1-pclToSkip] = m_TxPwr[35];

	return SP_OK;
}

void CFunUIS8910::SetFastCalibMode()
{
#if 0
	// TX
	WriteGpibCmd("CONF:GPRF:MEAS:POW:REP SING");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:THR -30");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:TYPE GAUS");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:FILT:GAUS:BWID 1MHz");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:MODE ONCE");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:SCO 1");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:SLEN 576.9230769E-6");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:MLEN 200E-6");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:LIST ON");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:LIST:STAR 0");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:SOUR 'IF Power'");
	WriteGpibCmd("CONF:GPRF:MEAS:POW:TRIG:OFFS 50E-6");
	WriteGpibCmd("TRIG:GPRF:MEAS:POW:TOUT 2");
#else
	SPRESULT ret;
	INIT_PARAM param;
	ZeroMemory(&param, sizeof(INIT_PARAM));
    ret = m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TX_FDT, &param);
	if(SP_OK != ret)
	{
       m_pImp->LogFmtStrA(SPLOGLV_ERROR,"%s: Fast Calib Init Fail!" , __FUNCTION__);
	}
#endif
}

void CFunUIS8910::SetFastCalibStopMode()
{
#if 0
	WriteGpibCmd("CONF:GPRF:MEAS:POW:LIST OFF");
#else
	//SPRESULT ret;
	//INIT_PARAM param;
	//ZeroMemory(&param, sizeof(INIT_PARAM));
    //ret = m_pRFTester->InitDev(TM_NON_SIGNAL, SUB_CAL_TX_FDT, &param);
	//if(SP_OK != ret)
	//{
    //   m_pImp->LogFmtStrA(SPLOGLV_ERROR,"%s: Fast Calib Stop Fail!" , __FUNCTION__);
	//}
#endif
	Sleep(300);
}

//----------------------------------------------------------------------------仪表控制函数 end

//----------------------------------------------------------------------------终端控制函数
SPRESULT CFunUIS8910::closeUeTrace()
{
	//CHKRESULT(SP_EnableArmLog(m_hDUT, FALSE));
	//CHKRESULT(SP_EnableArmLog(m_hDUT, TRUE));

	return SP_OK;
}


SPRESULT CFunUIS8910::send_cmd(uint32 cmd)
{
	TOOL_MPH_GSM_SEND_COMMAND_REQ_T *pMsg = (TOOL_MPH_GSM_SEND_COMMAND_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SEND_COMMAND_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SEND_COMMAND_REQ_T);
	pMsg->command = (CALIB_COMMAND_T)cmd;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	return SP_OK;
}

SPRESULT CFunUIS8910::send_mode(uint32 mode)
{
	TOOL_MPH_GSM_SEND_MODE_REQ_T *pMsg = (TOOL_MPH_GSM_SEND_MODE_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SEND_MODE_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SEND_MODE_REQ_T);
	pMsg->firstFint = 1;
    pMsg->state = (CALIB_STUB_CMDS_T)mode;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	return SP_OK;
}

SPRESULT CFunUIS8910::mode_rx(uint8 band, uint16 arfcn, uint8 expPow, uint32 tag)
{
	TOOL_MPH_GSM_MODE_RX_REQ_T *pMsg = (TOOL_MPH_GSM_MODE_RX_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_MODE_RX_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_MODE_RX_REQ_T);
	pMsg->arfcn = arfcn;
	pMsg->band = band;
	pMsg->expPow = expPow;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	if (tag == 0)
	{
        tag = CALIB_STUB_MONIT_STATE;
	}

	CHKRESULT(send_mode(tag));
	
	return SP_OK;
}

SPRESULT CFunUIS8910::mode_tx(uint8 band, uint16 arfcn, uint8 pcl, uint16 dacIdVal, uint8 tsc, uint32 tag)
{
	TOOL_MPH_GSM_MODE_TX_REQ_T *pMsg = (TOOL_MPH_GSM_MODE_TX_REQ_T *)&g_gsmTrans[0];
    
	if (tag == 0)
	{
        tag = CALIB_STUB_MONIT_STATE;
	}
	pMsg->SignalCode = TOOL_MPH_GSM_MODE_TX_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_MODE_TX_REQ_T);
	pMsg->tag = (CALIB_STUB_CMDS_T)tag;
	pMsg->arfcn = arfcn;
	pMsg->band = band;
	pMsg->pcl = pcl;
	pMsg->dacIdVal = dacIdVal;
	pMsg->tsc = tsc;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	CHKRESULT(send_mode(tag));
	
	return SP_OK;

}

SPRESULT CFunUIS8910::mode_tx_pcl(uint8 band, uint16 arfcn, uint8 pcl, uint8 tsc)
{
    CHKRESULT(mode_tx(band, arfcn, pcl, 0, tsc, CALIB_STUB_TX_STATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::mode_tx_pa_val(uint8 band, uint16 arfcn, uint16 dacIdVal, uint8 tsc)
{
    CHKRESULT(mode_tx(band, arfcn, 0, dacIdVal | (1<<15), tsc, CALIB_STUB_PA_STATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::mode_rx_power(uint8 band, uint16 arfcn, uint8 expPow)
{
    CHKRESULT(mode_rx(band, arfcn, expPow, CALIB_STUB_MONIT_STATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::update()
{
    CHKRESULT(send_cmd((uint32)CALIB_CMD_UPDATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::set_xtal_freq_offset(int32 xtalFreqOffset, bool& stop)
{
	TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_REQ_T *pMsg = (TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_REQ_T);
	pMsg->xtalCalibDone = CALIB_PROCESS_PENDING;
	pMsg->xtalFreqOffset = xtalFreqOffset;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_CNF_T *pMsgCnf = (TOOL_MPH_GSM_AFC_XTAL_COMPENSATE_CNF_T *)&g_gsmTrans[0];
	if(pMsgCnf->xtalCalibDone == CALIB_PROCESS_STOP)
	{
	    stop = true;
	}
	else if(pMsgCnf->xtalCalibDone == CALIB_PROCESS_CONTINUE)
	{
	    stop = false;
	}
	else
	{
	    return SP_E_FAIL;
	}

    return SP_OK;
}

SPRESULT CFunUIS8910::get_xcv_param(int id, int32 &param)
{
	TOOL_MPH_GSM_GET_XCV_PARAM_REQ_T *pMsg = (TOOL_MPH_GSM_GET_XCV_PARAM_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_GET_XCV_PARAM_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_GET_XCV_PARAM_REQ_T);
	pMsg->id = (uint8)id;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_GET_XCV_PARAM_CNF_T *pMsgCnf = (TOOL_MPH_GSM_GET_XCV_PARAM_CNF_T *)&g_gsmTrans[0];
	param = pMsgCnf->param;

	return SP_OK;
}

SPRESULT CFunUIS8910::get_rx_nb_power(uint8 &val)
{
	TOOL_MPH_GSM_GET_RX_NB_POWER_REQ_T *pMsg = (TOOL_MPH_GSM_GET_RX_NB_POWER_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_GET_RX_NB_POWER_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_GET_RX_NB_POWER_REQ_T);

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    TOOL_MPH_GSM_GET_RX_NB_POWER_CNF_T *pMsgCnf = (TOOL_MPH_GSM_GET_RX_NB_POWER_CNF_T *)&g_gsmTrans[0];
	val = pMsgCnf->nbPower;

    return SP_OK;
}

SPRESULT CFunUIS8910::set_sw_param(int id, int32 param)
{
	TOOL_MPH_GSM_SET_SW_PARAM_REQ_T *pMsg = (TOOL_MPH_GSM_SET_SW_PARAM_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SET_SW_PARAM_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SET_SW_PARAM_REQ_T);
	pMsg->id = (uint8)id;
	pMsg->param = param;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    return SP_OK;
}

SPRESULT CFunUIS8910::get_calibflag(uint32 &flag)
{
	TOOL_MPH_GSM_GET_CALIB_INFO_REQ_T *pMsg = (TOOL_MPH_GSM_GET_CALIB_INFO_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_GET_CALIB_INFO_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_GET_CALIB_INFO_REQ_T);

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_GET_CALIB_INFO_CNF_T *pMsgCnf = (TOOL_MPH_GSM_GET_CALIB_INFO_CNF_T *)&g_gsmTrans[0];
	flag = pMsgCnf->date;

	return SP_OK;
}

SPRESULT CFunUIS8910::set_default_power_per_pcl_per_arfcn(int band, T_CALIB_DATA *pData)
{
	TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T *pMsg = (TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T);
	pMsg->band = (uint8)band;

	switch (band) 
	{
	case CALIB_STUB_BAND_PCS1900:
		for (int n = 0; n < 18; n++) 
		{
			pMsg->power[n][0] = pData->DefaultTargetPclPowP[n];
			pMsg->power[n][1] = pData->DefaultTargetPclPowP[n] + 1;
		}
		break;
	case CALIB_STUB_BAND_DCS1800:
		for (int n = 0; n < 17; n++) 
		{
			pMsg->power[n][0] = pData->DefaultTargetPclPowD[n];
			pMsg->power[n][1] = pData->DefaultTargetPclPowD[n] + 1;
		}
		break;
	case CALIB_STUB_BAND_GSM900:
		for (int n = 0; n < 15; n++) 
		{
			pMsg->power[n][0] = pData->DefaultTargetPclPowG[n];
			pMsg->power[n][1] = pData->DefaultTargetPclPowG[n] + 1;
		}
		break;
	case CALIB_STUB_BAND_GSM850:
		for (int n = 0; n < 15; n++) 
		{

			pMsg->power[n][0] = pData->DefaultTargetPclPowG850[n];
			pMsg->power[n][1] = pData->DefaultTargetPclPowG850[n] + 1;
		}
		break;
	}

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	CHKRESULT(update());

    return SP_OK;
}

SPRESULT CFunUIS8910::set_pa_profile_dac_req(int *pData0, int *pData1, int *pData2, int *pData3)
{
	TOOL_MPH_GSM_PA_PROFILE_DAC_REQ_T *pMsg = (TOOL_MPH_GSM_PA_PROFILE_DAC_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_PA_PROFILE_DAC_SET_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_PA_PROFILE_DAC_REQ_T);

	for (int n = 0; n < 16; n++) 
	{
		pMsg->profile_interp[0][n] = (uint16)pData0[n];
		pMsg->profile_interp[1][n] = (uint16)pData1[n];
		pMsg->profile_interp[2][n] = (uint16)pData2[n];
		pMsg->profile_interp[3][n] = (uint16)pData3[n];
	}

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	//CHKRESULT(update());

    return SP_OK;
}

SPRESULT CFunUIS8910::set_pa_profile_meas(uint8 band, int32 measPower, uint16 &paProfNextDacVal, uint8 &paProfCalibDone)
{
	TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_REQ_T *pMsg = (TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_REQ_T *)&g_gsmTrans[0];
	
	paProfCalibDone = CALIB_PROCESS_PENDING;
	pMsg->SignalCode = TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_REQ_T);
	pMsg->paProfCalibDone = paProfCalibDone;
	pMsg->txBand = band;
	pMsg->setPAProfMeas = measPower;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_APC_PA_PROFILE_MEAS_CNF_T *)&g_gsmTrans[0];
	paProfNextDacVal = pMsgCnf->paProfNextDacVal;
	paProfCalibDone = pMsgCnf->paProfCalibDone;

	if (paProfCalibDone != CALIB_PROCESS_STOP &&
		paProfCalibDone != CALIB_PROCESS_CONTINUE &&
		paProfCalibDone != CALIB_PROCESS_NEED_CALM)
	{
		return SP_E_FAIL;
	}

	return SP_OK;
}

SPRESULT CFunUIS8910::set_pa_profile_meass(uint8 band, int32 measPower[], uint16 paDacVal[])
{
    TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_REQ_T *pMsg = (TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_REQ_T *)&g_gsmTrans[0];
	
	pMsg->SignalCode = TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_REQ_T);
	pMsg->txBand = band;
	memcpy(&pMsg->setfastPaProfMeas[0],&measPower[0],16*sizeof(int32));

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_APC_PA_FAST_PROFILE_MEAS_CNF_T *)&g_gsmTrans[0];
	memcpy(&paDacVal[0], &pMsgCnf->fastPaProfDacVal[0], 16*sizeof(uint16));

    return SP_OK;
}

//SPRESULT CFunUIS8910::set_power_per_pcl_per_arfcn(int band, int pclId, int arfcn, int16 measuredPower, T_CALIB_DATA *pData)
SPRESULT CFunUIS8910::set_power_per_pcl_per_arfcn(int band, int16 *pMeasPow)
{
    TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T *pMsg = (TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SET_POW_PER_PCL_ARFCN_REQ_T);
	pMsg->band = (uint8)band;

	memcpy(&pMsg->power[0][0], pMeasPow, 18*2*sizeof(int16));

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg)) 

    return SP_OK;
}

SPRESULT CFunUIS8910::set_method(bool automaticTool)
{
	TOOL_MPH_GSM_SET_CALIB_INFO_REQ_T *pMsg = (TOOL_MPH_GSM_SET_CALIB_INFO_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SET_CALIB_INFO_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SET_CALIB_INFO_REQ_T);

	uint32 date = 0;//uint32(QDateTime::currentMSecsSinceEpoch() / 1000);  //chengui
	pMsg->date = date;
	if(automaticTool)
	{
	    pMsg->method = CALIB_METH_AUTOMATIC;
	}
	else
	{
	    pMsg->method = CALIB_METH_MANUAL;
	}

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    CHKRESULT(send_cmd((uint32)CALIB_CMD_UPDATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::set_iloss_offset(int8 ilossOffset, uint16 &iLossNextArfcn, bool &stop)
{
    TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_REQ_T *pMsg = (TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_REQ_T *)&g_gsmTrans[0];
	uint8 iLossCalibDone = CALIB_PROCESS_PENDING;

	pMsg->SignalCode = TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_REQ_T);
	pMsg->iLossCalibDone = iLossCalibDone;
	pMsg->ilossOffset = ilossOffset;

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_CNF_T *pMsgCnf = (TOOL_MPH_GSM_AGC_ILOSS_COMPENSATE_CNF_T *)&g_gsmTrans[0];
	iLossNextArfcn = pMsgCnf->iLossNextArfcn;
	if(pMsgCnf->iLossCalibDone == CALIB_PROCESS_STOP)
    {
		stop = true;
	}
	else if(pMsgCnf->iLossCalibDone == CALIB_PROCESS_CONTINUE)
    {
		stop = false;
	}
	else
	{
	    return SP_E_FAIL;
	}

    return SP_OK;
}

SPRESULT CFunUIS8910::set_connectNs(int rfset, uint8 band, uint16 tch, uint16 bch, uint8 pcl, uint8 &rspid, uint8 &status)
{
    uint8 temp_rspid;
    uint8 temp_status;
    temp_rspid  = rspid;
    temp_status  = status;

    TOOL_MPH_GSM_NST_SET_CONNECT_REQ_T *pMsg = (TOOL_MPH_GSM_NST_SET_CONNECT_REQ_T *)&g_gsmTrans[0];
	//uint8 iLossCalibDone = CALIB_PROCESS_PENDING;

	pMsg->SignalCode = TOOL_MPH_GSM_NST_SET_CONNECT_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_SET_CONNECT_REQ_T);
	pMsg->txArfcn = tch;
	pMsg->txPcl = pcl;
	pMsg->txBand = band;
	pMsg->monArfcn = bch;
	pMsg->monBand = band;
	pMsg->txTsc = NO_SIG_SETUP[rfset][0];
	pMsg->monExpPow = 0xff;
	pMsg->nbPower = 0xff;
	pMsg->tn = NO_SIG_SETUP[rfset][1];
	pMsg->power[0] = 120;  //CALIB_NB_CELLS
	pMsg->nstreqstate = 0xc0;
	pMsg->nstreqcurrstate = 1;
	pMsg->loopback = NO_SIG_SETUP[rfset][3];
	pMsg->syncPhase = NO_SIG_SETUP[rfset][11];
	pMsg->codec = NO_SIG_SETUP[rfset][2];
	pMsg->pn_type = NO_SIG_SETUP[rfset][4];
	pMsg->ber_mode = NO_SIG_SETUP[rfset][5];
	pMsg->invert = NO_SIG_SETUP[rfset][6];
	pMsg->acq_threshold = NO_SIG_SETUP[rfset][8];
	pMsg->acq_lost_threshold = NO_SIG_SETUP[rfset][9];

	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

    return SP_OK;
}

SPRESULT CFunUIS8910::mode_stop()
{
    CHKRESULT(send_mode(CALIB_STUB_NO_STATE));

	return SP_OK;
}

SPRESULT CFunUIS8910::burn_flash(bool automaticTool)
{
    CHKRESULT(mode_stop());
    CHKRESULT(set_method(automaticTool));
    CHKRESULT(send_cmd((uint32)CALIB_CMD_FLASH_BURN));
    
	return SP_OK;
}

SPRESULT CFunUIS8910::reset_rfparameters()
{
    CHKRESULT(mode_stop());
    CHKRESULT(send_cmd((uint32)CALIB_CMD_RF_RESET));

    return SP_OK;
}

SPRESULT CFunUIS8910::CalibPrepare(void)
{
    CHKRESULT(reset_rfparameters());
    Sleep(100);

    return SP_OK;
}

SPRESULT CFunUIS8910::CalibBurnFlash(void)
{
    CHKRESULT(burn_flash(true)); 

    return SP_OK;
}

SPRESULT CFunUIS8910::dump_GsmRftable(uint32 caliFlag)
{
	TOOL_MPH_GSM_SET_CALIB_FLAG_REQ_T *pMsg = (TOOL_MPH_GSM_SET_CALIB_FLAG_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_SET_CALIB_FLAG_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_SET_CALIB_FLAG_REQ_T);
	pMsg->caliFlag = caliFlag;
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_DUMP_CALIB_INFO_REQ_T *pMsg2 = (TOOL_MPH_GSM_DUMP_CALIB_INFO_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_DUMP_CALIB_INFO_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_DUMP_CALIB_INFO_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	TOOL_MPH_GSM_DUMP_CALIB_INFO_CNF_T *pRsp = (TOOL_MPH_GSM_DUMP_CALIB_INFO_CNF_T *)&g_gsmTrans[0];

	m_log.Open(m_pImp, _T("GSM\\GSM_CALIB.bin"), OPEN_ALWAYS);
	m_log.Write(&pRsp->versionTag, sizeof(TOOL_MPH_GSM_DUMP_CALIB_INFO_CNF_T)-sizeof(TOOL_EMPTY_CNF_T));
	m_log.Close();

	return SP_OK;
}


SPRESULT CFunUIS8910::set_gsmNstFlag(uint32 caliFlag)
{
	TOOL_MPH_GSM_NST_SET_FLAG_REQ_T *pMsg2 = (TOOL_MPH_GSM_NST_SET_FLAG_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_NST_SET_FLAG_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_NST_SET_FLAG_REQ_T);
	pMsg2->nstFlag = caliFlag;
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	return SP_OK;
}

SPRESULT CFunUIS8910::set_gsmAntTestFlag(uint32 caliFlag)
{
	TOOL_MPH_GSM_NST_SET_FLAG_REQ_T *pMsg2 = (TOOL_MPH_GSM_NST_SET_FLAG_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_ANTTEST_SET_FLAG_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_NST_SET_FLAG_REQ_T);
	pMsg2->nstFlag = caliFlag;
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	return SP_OK;
}

SPRESULT CFunUIS8910::get_NsftComstatus(uint8 &rspid, uint8 &status)
{
    TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *pMsg = (TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_NST_GET_STATUS_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_GET_STATUS_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *)&g_gsmTrans[0];
	
	rspid = pMsgCnf->rspcId;
	status = pMsgCnf->status;
	
    return SP_OK;
}

SPRESULT CFunUIS8910::get_versions(int &major, int &minor)
{
    TOOL_MPH_GSM_CODE_VERSION_REQ_T *pMsg = (TOOL_MPH_GSM_CODE_VERSION_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_CODE_VERSION_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_CODE_VERSION_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_CODE_VERSION_CNF_T *pMsgCnf = (TOOL_MPH_GSM_CODE_VERSION_CNF_T *)&g_gsmTrans[0];

    major = (pMsgCnf->version >> 8) & 0xFF;
    minor = (pMsgCnf->version >> 0) & 0xFF;

    return SP_OK;
}

SPRESULT CFunUIS8910::gsm_set_mode(uint32 cmd)
{
   uint32 temp_cmd;
   temp_cmd = cmd;

#if 0  //chengui  //不需要
	TOOL_MPH_GSM_CODE_VERSION_REQ_T *pMsg = (TOOL_MPH_GSM_CODE_VERSION_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_CODE_VERSION_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_CODE_VERSION_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	//RETURN_IF_FAIL(writeTarget(&mCtrlTarget->CtrlCalib->mode.gsm, cmd));
#endif
	return SP_OK;
}

SPRESULT CFunUIS8910::set_Nsft_configure_req(uint8 &rspid, uint8 &status)
{
	TOOL_MPH_GSM_NST_CONFIG_REQ_T *pMsg = (TOOL_MPH_GSM_NST_CONFIG_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_NST_CONFIG_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_CONFIG_REQ_T);
	pMsg->nstreqstate = 0xc2;
	pMsg->nstreqcurrstate = 1;
	pMsg->status = 1;
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *pMsg2 = (TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_NST_GET_STATUS_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_NST_GET_STATUS_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *)&g_gsmTrans[0];

	status = pMsgCnf->status;
	rspid = pMsgCnf->rspcId;

    return SP_OK;
}

SPRESULT CFunUIS8910::set_Nsft_berloop_ab_orderreq(int rfset, uint8 &rspid, uint8 &status)
{
	TOOL_MPH_GSM_NST_BER_LOOP_AB_ORDER_REQ_T *pMsg = (TOOL_MPH_GSM_NST_BER_LOOP_AB_ORDER_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_NST_BER_LOOP_AB_ORDER_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_BER_LOOP_AB_ORDER_REQ_T);
	pMsg->nstreqstate = 0xc8;
	pMsg->nstreqcurrstate = 1;
	pMsg->status = 1;
	pMsg->order = NO_SIG_SETUP[rfset][7];
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *pMsg2 = (TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_NST_GET_STATUS_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_NST_GET_STATUS_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *)&g_gsmTrans[0];

	status = pMsgCnf->status;
	rspid = pMsgCnf->rspcId;

    return SP_OK;
}

SPRESULT CFunUIS8910::set_Nsft_ber_start_log_req(int rfset, uint8 &rspid, uint8 &status)
{
	TOOL_MPH_GSM_NST_BER_START_LOG_REQ_T *pMsg = (TOOL_MPH_GSM_NST_BER_START_LOG_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_NST_BER_START_LOG_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_BER_START_LOG_REQ_T);
	pMsg->nstreqstate = 0xc4;
	pMsg->nstreqcurrstate = 1;
	pMsg->status = 1;
	pMsg->logEnable = 1;
	pMsg->num_samples = NO_SIG_SETUP[rfset][10];
	pMsg->measuretimes = 0;
	pMsg->num_bits = 0;
	pMsg->num_errors = 0;
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *pMsg2 = (TOOL_MPH_GSM_NST_GET_STATUS_REQ_T *)&g_gsmTrans[0];
	pMsg2->SignalCode = TOOL_MPH_GSM_NST_GET_STATUS_REQ;
	pMsg2->SignalSize = sizeof(TOOL_MPH_GSM_NST_GET_STATUS_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg2));

	TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *pMsgCnf = (TOOL_MPH_GSM_NST_GET_STATUS_CNF_T *)&g_gsmTrans[0];

	status = pMsgCnf->status;
	rspid = pMsgCnf->rspcId;

    return SP_OK;
}

SPRESULT CFunUIS8910::get_NsftBitNumber(uint32 &nbBits, uint32 &nbErr, uint32 &logindex)
{
	TOOL_MPH_GSM_NST_BER_GET_REQ_T *pMsg = (TOOL_MPH_GSM_NST_BER_GET_REQ_T *)&g_gsmTrans[0];
	pMsg->SignalCode = TOOL_MPH_GSM_NST_BER_GET_REQ;
	pMsg->SignalSize = sizeof(TOOL_MPH_GSM_NST_BER_GET_REQ_T);
	CHKRESULT(SP_gsmUIS8910(m_hDUT, (void *)pMsg));

	TOOL_MPH_GSM_NST_BER_GET_CNF_T *pMsgCnf = (TOOL_MPH_GSM_NST_BER_GET_CNF_T *)&g_gsmTrans[0];
	nbBits = pMsgCnf->num_bits;
	nbErr = pMsgCnf->num_errors;
	logindex = pMsgCnf->measuretimes;
	
    return SP_OK;
}

//----------------------------------------------------------------------------终端控制函数 end


UINT8 CFunUIS8910::NO_SIG_SETUP[7][13] =
{
	{5, 3, 0, 1, 1, 3, 0, 1, 250, 100, 32, 0, 0},
	/*{5, 3, 0, 1, 3, 3, 0, 0, 250,  50, 32, 1, 0},*/
	{5, 3, 0, 1, 1, 3, 0, 1, 250, 100, 32, 0, 0},
	{0, 3, 0, 1, 1, 3, 0, 0, 250, 100, 32, 0, 1},
	{0, 3, 0, 1, 1, 3, 0, 1, 250, 100, 32, 1, 1},
	/*{0, 3, 0, 1, 1, 3, 0, 1, 250, 100, 32, 1, 1},*/
	{0, 3, 0, 1, 1, 3, 0, 1, 250, 100, 32, 0, 1},
	{0, 3, 0, 1, 1, 3, 0, 0, 250, 100, 32, 0, 1},
	{5, 3, 0, 1, 1, 3, 0, 0, 250, 100, 32, 0, 0}
};


