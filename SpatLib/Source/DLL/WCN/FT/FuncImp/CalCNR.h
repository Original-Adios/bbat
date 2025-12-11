#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ImpBase.h"

#define MAX_DIAG_GPS_MODULE_CNR_BUFF_SIZE      (20/*DATA_CAPTURE_BEGIN+\n\r*/+8192/*5632*/*(16*3+2)/*第2~第5633包数据：每包16=4个unsigned int数据,3个空格，2个\n\r，总共281600*/+18/*DATA_CAPTURE_END+\n\r*/)//281638//409638
#define MAX_CNR_DATA_COUNT (32*1024)//32768
#define MAX_L1_DATA_COUNT (16*1024)
#define MAX_L5_DATA_COUNT (32*1024)

class CCalCNR
{
public:
	CCalCNR(void);
	virtual ~CCalCNR(void);

public:
    void Init(CImpBase* lpTr, UINT32 nDataCount);
	void FFT(double data[], int nn, int isign);
	void CalMag(double data[], double arryFFTMagData[]);
	void DataSwap(double data[], int count);
	double FFT_Interpolate(int max_indx, double* fft_pow, int fft_length);
	double CalSigPower(double arryFFTMagData[], int nSigBins);
	int Calculate_Cw_Data_Capture(const char* nmea);
	void FindMax(double data[], int count);
	double CalNoisePower(double arryFFTMagData[]);
	double CalSNR(double arryFFTMagData[], double fMaxValue, int nMaxIndex);

	void cw_data_capture(const char* nmea, int length);
	int eng_ascii2hex(const char* inputdata, unsigned char* outputdata, int inputdatasize);
public:
    CImpBase *m_pLog;
	UINT32 m_nDataCount;
};
