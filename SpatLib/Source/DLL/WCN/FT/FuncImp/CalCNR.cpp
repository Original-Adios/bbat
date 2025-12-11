#include "StdAfx.h"
#include "CalCNR.h"

LPCSTR GPS_MODULE_CNR_HEADER = "DATA_CAPTURE_BEGIN";
LPCSTR GPS_MODULE_CNR_END = "DATA_CAPTURE_END";

#define A0 (0.21557895)
#define A1 (0.41663158)
#define A2 (0.277263158)
#define A3 (0.083578947)
#define A4 (0.006947368)


#define FS (4092000)
//#define nDataCount (22528/*16384*/)  //4096
#define FIN (200000)
#define CW_RAW_DATA_LEN (1024)
#define MAX_DBG_BUF_LEN (1024)
#define MAX_BYTE_COUNT_IN_ROW (32)

// The following line must be defined before including math.h to correctly define M_PI
#define PI M_PI /* pi to machine precision, defined in math.h */
#define TWOPI (PI*2.0)



char data_capture_flag = 0;
static int dataCount = 0;
static int lineCount = 0;
double arrIQData[2*0xFFFF] = { 0.0 };//3 * nDataCount + 1
double arrWinAdd[0xFFFF] = { 0.0 };// nDataCount
double fMaxMag = 0.0;
int g_MaxIndex = 0;
double g_MaxIndex_first = 0;
double g_MaxIndex_second = 0;
int g_CN0 = 0;

typedef enum _ECW_State
{
	CW_START_UNKWON = 0,
	CW_START_BEGIN,
	CW_START_END
}_ECW_State_e;

CCalCNR::CCalCNR(void)
	:m_nDataCount(MAX_L1_DATA_COUNT)
	, m_pLog(NULL)
{
}


CCalCNR::~CCalCNR(void)
{
}

void CCalCNR::Init(CImpBase* lpTr, UINT32 nDataCount)
{
    m_pLog = (CImpBase*)lpTr;
	m_nDataCount = nDataCount;
}

int CCalCNR::eng_ascii2hex(const char* inputdata, unsigned char* outputdata, int inputdatasize)
{
	int i, j, tmp_len;
	unsigned char tmpData;
	std::string strTmp = inputdata;
	for (i = 0; i < inputdatasize; i++)
	{
		if ((strTmp[i] >= '0') && (strTmp[i] <= '9'))
		{
			tmpData = strTmp[i] - '0';
		}
		else if ((strTmp[i] >= 'A') && (strTmp[i] <= 'F'))
		{ //A....F
			tmpData = strTmp[i] - 'A' + 10;
		}
		else if ((strTmp[i] >= 'a') && (strTmp[i] <= 'f'))
		{ //a....f
			tmpData = strTmp[i] - 'a' + 10;
		}
		else
		{
			break;
		}
		strTmp[i] = tmpData;
	}

	for (tmp_len = 0, j = 0; j < i; j += 2)
	{
		outputdata[tmp_len++] = (strTmp[j] << 4) | strTmp[j + 1];
	}

	return tmp_len;
}

/*
   FFT/IFFT routine. (see pages 507-508 of Numerical Recipes in C)
Inputs:
data[] : array of complex* data points of size 2*NFFT+1.
data[0] is unused,
 * the n'th complex number x(n), for 0 <= n <= length(x)-1, is stored as:
 data[2*n+1] = real(x(n))
 data[2*n+2] = imag(x(n))
 if length(Nx) < NFFT, the remainder of the array must be padded with zeros
nn : FFT order NFFT. This MUST be a power of 2 and >= length(x).
isign: if set to 1,
computes the forward FFT
if set to -1,
computes Inverse FFT - in this case the output values have
to be manually normalized by multiplying with 1/NFFT.
Outputs:
data[] : The FFT or IFFT results are stored in data, overwriting the input.
 */

void CCalCNR::FFT(double data[], int nn, int isign)
{
	int n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;
	n = nn << 1;
	j = 1;

	for (i = 1; i < n; i += 2)
	{
		if (j > i)
		{
			tempr = data[j]; data[j] = data[i]; data[i] = tempr;
			tempr = data[j + 1]; data[j + 1] = data[i + 1]; data[i + 1] = tempr;
		}
		m = n >> 1;
		while (m >= 2 && j > m)
		{
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax)
	{
		istep = 2 * mmax;
		theta = TWOPI / (isign * mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2)
		{
			for (i = m; i <= n; i += istep)
			{
				j = i + mmax;
				tempr = wr * data[j] - wi * data[j + 1];
				tempi = wr * data[j + 1] + wi * data[j];
				data[j] = data[i] - tempr;
				data[j + 1] = data[i + 1] - tempi;
				data[i] += tempr;
				data[i + 1] += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		mmax = istep;
	}
}

void CCalCNR::CalMag(double data[], double arryFFTMagData[])
{
	double fMax = 0.0;
	int nIndex = 0;
	double fCur = 0.0;
	int i;

	for (i = 0; i < m_nDataCount; i++)
	{
		fCur = sqrt(data[2 * i + 1] * data[2 * i + 1] + data[2 * i + 2] * data[2 * i + 2]) / m_nDataCount / 1024.0;
		arryFFTMagData[i] = fCur;
		if (fCur > fMax)
		{
			fMax = fCur;
			nIndex = i;
		}
	}

	fMaxMag = fMax;
	g_MaxIndex = nIndex;
}

void CCalCNR::DataSwap(double data[], int count)
{
	double temp = 0.0;
	int i;

	for (i = 0; i < count / 2; i++)
	{
		temp = data[i];
		data[i] = data[i + count / 2];
		data[i + count / 2] = temp;
	}
}

double CCalCNR::FFT_Interpolate(int max_indx, double* fft_pow, int fft_length)
{
	float/*float*/ y1 = 0.0;
	float y2 = 0.0;
	float y3 = 0.0;
	float /*float*/ delta_x = 0.0;

	if (max_indx == 0)
	{
		y1 = (float)fft_pow[fft_length - 1];
		y2 = (float)fft_pow[max_indx];
		y3 = (float)fft_pow[max_indx + 1];
	}
	else if (max_indx == (fft_length - 1))
	{
		y1 = (float)fft_pow[max_indx - 1];
		y2 = (float)fft_pow[max_indx];
		y3 = (float)fft_pow[0];
	}
	else
	{
		y1 = (float)fft_pow[max_indx - 1];
		y2 = (float)fft_pow[max_indx];
		y3 = (float)fft_pow[max_indx + 1];
	}

	y1 = (float)pow(y1, 0.25);
	y2 = (float)pow(y2, 0.25);
	y3 = (float)pow(y3, 0.25);

	if ((2 * y2) == (y1 + y3)) //lint !e777
	{
		delta_x = 0.0;
	}
	else
	{
		delta_x = (y3 - y1) / (2 * (2 * y2 - (y1 + y3)));
	}

	return delta_x;

}

void CCalCNR::FindMax(double data[], int count)
{
	double maxValue = data[0];
	double maxIndex_frac = 0.0;
	int maxIndex = 0;
	int i;

	for (i = 1; i < count; i++)
	{
		if (data[i] > maxValue)
		{
			maxValue = data[i];
			maxIndex = i;
		}
	}

	fMaxMag = maxValue;
	maxIndex_frac = FFT_Interpolate(maxIndex, data, count);
	g_MaxIndex = maxIndex + (INT)maxIndex_frac;
	g_MaxIndex_first = g_MaxIndex_second;
	g_MaxIndex_second = g_MaxIndex;
    m_pLog->LogFmtStrA(SPLOGLV_INFO, "%s fMaxMag=%lf, maxIndex=%ld, maxIndex_frac=%lf", __FUNCTION__, fMaxMag, maxIndex, maxIndex_frac);
}

double CCalCNR::CalSigPower(double arryFFTMagData[], int nSigBins)
{
	double fPowerSum = 0.0;
	double fSigPower = 0.0;
	int arryIndex[5] = { 0 };
	int i = 0;
	int count = 0;
	int k;

	switch (nSigBins)
	{
	case 0:
		fPowerSum = arryFFTMagData[m_nDataCount - 2] * arryFFTMagData[m_nDataCount - 2]
			+ arryFFTMagData[m_nDataCount - 1] * arryFFTMagData[m_nDataCount - 1]
			+ arryFFTMagData[0] * arryFFTMagData[0]
			+ arryFFTMagData[1] * arryFFTMagData[1]
			+ arryFFTMagData[2] * arryFFTMagData[2];

		arryIndex[0] = m_nDataCount - 2;
		arryIndex[1] = m_nDataCount - 1;
		arryIndex[2] = 0;
		arryIndex[3] = 1;
		arryIndex[4] = 2;
		break;

	case 1:
		fPowerSum = arryFFTMagData[m_nDataCount - 1] * arryFFTMagData[m_nDataCount - 1]
			+ arryFFTMagData[0] * arryFFTMagData[0]
			+ arryFFTMagData[1] * arryFFTMagData[1]
			+ arryFFTMagData[2] * arryFFTMagData[2]
			+ arryFFTMagData[3] * arryFFTMagData[3];
		arryIndex[0] = m_nDataCount - 1;
		arryIndex[1] = 0;
		arryIndex[2] = 1;
		arryIndex[3] = 2;
		arryIndex[4] = 3;
		break;

	case 16383:
		fPowerSum = arryFFTMagData[m_nDataCount - 3] * arryFFTMagData[m_nDataCount - 3]
			+ arryFFTMagData[m_nDataCount - 2] * arryFFTMagData[m_nDataCount - 2]
			+ arryFFTMagData[m_nDataCount - 1] * arryFFTMagData[m_nDataCount - 1]
			+ arryFFTMagData[0] * arryFFTMagData[0]
			+ arryFFTMagData[1] * arryFFTMagData[1];
		arryIndex[0] = m_nDataCount - 3;
		arryIndex[1] = m_nDataCount - 2;
		arryIndex[2] = m_nDataCount - 1;
		arryIndex[3] = 0;
		arryIndex[4] = 1;
		break;

	case 16382:
		fPowerSum = arryFFTMagData[m_nDataCount - 4] * arryFFTMagData[m_nDataCount - 4]
			+ arryFFTMagData[m_nDataCount - 3] * arryFFTMagData[m_nDataCount - 3]
			+ arryFFTMagData[m_nDataCount - 2] * arryFFTMagData[m_nDataCount - 2]
			+ arryFFTMagData[m_nDataCount - 1] * arryFFTMagData[m_nDataCount - 1]
			+ arryFFTMagData[0] * arryFFTMagData[0];
		arryIndex[0] = m_nDataCount - 4;
		arryIndex[1] = m_nDataCount - 3;
		arryIndex[2] = m_nDataCount - 2;
		arryIndex[3] = m_nDataCount - 1;
		arryIndex[4] = 0;
		break;

	default:
		k = nSigBins - 2;

		for (i = 0; k <= nSigBins + 2; k++, count++, i++)
		{
			fPowerSum += arryFFTMagData[k] * arryFFTMagData[k];
			arryIndex[count] = k;
		}
	}

	for (i = 0; i < 5; i++)
	{
		arryFFTMagData[arryIndex[i]] = 0.0;
	}
	fSigPower = sqrt((double)fPowerSum);

	return fSigPower;
}

double CCalCNR::CalNoisePower(double arryFFTMagData[])
{
	int k = 0;
	double fNoisePower = 0.0;
	double fSum = 0.0;
	double fNoiseBinLow = 0.0, fNoiseBinHigh = 0.0;
	double nFrqBin = FS / (double)m_nDataCount;

	fNoiseBinLow = m_nDataCount / 2 - floor(1000000.0 / nFrqBin) - 1;
	fNoiseBinHigh = m_nDataCount / 2 + floor(1000000.0 / nFrqBin) - 1;

	for (k = (int)fNoiseBinLow; k <= (int)fNoiseBinHigh; k++)
	{
		fSum += arryFFTMagData[k] * arryFFTMagData[k];
	}

	fNoisePower = sqrt((double)fSum);
	return fNoisePower;
}

double CCalCNR::CalSNR(double arryFFTMagData[], double fMaxValue, int nMaxIndex)
{
	double power = 0.0, noise = 0.0, SNR = 0.0;

	double nFrqBin = FS / (double)m_nDataCount;
	int nSigBins1 = (int)floor(m_nDataCount / 2.0 + FIN / nFrqBin + 1);

	int nSigBins = (fMaxValue > arryFFTMagData[nSigBins1]) ? nMaxIndex : nSigBins1;

	power = CalSigPower(arryFFTMagData, nSigBins);
	noise = CalNoisePower(arryFFTMagData);

	SNR = 20 * log10(power / noise);

	return SNR;
}


#define GNSS_MARLIN3 1
//parse the cw raw data
int CCalCNR::Calculate_Cw_Data_Capture(const char* nmea)
{
	unsigned char data1 = 0, data2 = 0, data3 = 0, data4 = 0;
	long IHexVal = 0;
	long QHexVal = 0;
	double snr = 0.0;
	double arrFFTMagData[0xFFFF] = {0.0};
	int n = 0;
	static int totalLen = 0;

	if (NULL == nmea)
	{
		return -999999;
	}
	char* pBeginPos = (char*)strstr(nmea, GPS_MODULE_CNR_HEADER);
	char* pTailer = (char*)strstr(nmea, GPS_MODULE_CNR_END);

	if (NULL == pBeginPos)
	{
        m_pLog->LogFmtStrA(SPLOGLV_ERROR, "DataHead %s Not Found.", GPS_MODULE_CNR_HEADER);
		return -999999;
	}

	if (NULL == pTailer)
	{
		m_pLog->LogFmtStrA(SPLOGLV_ERROR, "DataTailer %s Not Found.", GPS_MODULE_CNR_END);
		return -999999;
	}

	m_pLog->LogFmtStrA(SPLOGLV_INFO, "####data calculate begin.");
	data_capture_flag = CW_START_BEGIN;

	fMaxMag = 0;
	g_MaxIndex = 0;

	memset(arrIQData, 0, sizeof(arrIQData));
	memset(arrWinAdd, 0, sizeof(arrWinAdd));

	int nCnrHeadLength = strlen(GPS_MODULE_CNR_HEADER);
	int nCnrDataLength = pTailer - pBeginPos - nCnrHeadLength;//180224
	char szCnrData[MAX_DIAG_GPS_MODULE_CNR_BUFF_SIZE] = { 0 };
	strncpy_s(szCnrData, MAX_DIAG_GPS_MODULE_CNR_BUFF_SIZE, pBeginPos + nCnrHeadLength, nCnrDataLength);
	szCnrData[nCnrDataLength] = '\0';

	uint8 u8CnrData[MAX_CNR_DATA_COUNT] = { 0 };
	int hexCnrDataLength = eng_ascii2hex(szCnrData, u8CnrData, nCnrDataLength);//180224/2 = 90112

    if (0 != nCnrDataLength % 1024 || 0 != m_nDataCount %4/* || (hexCnrDataLength / 4) < m_nDataCount*/)//90112/4 =22528
	{
		m_pLog->LogFmtStrA(SPLOGLV_ERROR, "Fail Of CnrData Format #nCnrDataLength = %d# #nDataCount = %d#.", hexCnrDataLength, m_nDataCount);
        return -999999;
	}

	totalLen = 0;
	dataCount = 0;
	const char* pData = (char*)u8CnrData;

	while (dataCount < (m_nDataCount))//90112/4 =22528
	{
		//"af002100"
		data1 = *pData++;//0xaf
		data2 = *pData++;//0x00
		data3 = *pData++;//0x21
		data4 = *pData++;//0x00
		dataCount++;

		IHexVal = (data2 << 8) + data1;
		QHexVal = (data4 << 8) + data3;
#ifdef GNSS_MARLIN3
		IHexVal = IHexVal > 256 ? IHexVal - 512 : IHexVal;
		QHexVal = QHexVal > 256 ? QHexVal - 512 : QHexVal;
#else
		IHexVal = IHexVal > 32768 ? IHexVal - 65535 : IHexVal;
		QHexVal = QHexVal > 32768 ? QHexVal - 65535 : QHexVal;
#endif           
		arrIQData[2 * (dataCount - 1) + 1] = (double)IHexVal;
		arrIQData[2 * (dataCount - 1) + 2] = (double)QHexVal * -1;

		arrWinAdd[dataCount - 1] = A0 - A1 * cos(2 * PI * (dataCount - 1) / (m_nDataCount - 1))
			+ A2 * cos(4 * PI * (dataCount - 1) / (m_nDataCount - 1))
			- A3 * cos(6 * PI * (dataCount - 1) / (m_nDataCount - 1))
			+ A4 * cos(8 * PI * (dataCount - 1) / (m_nDataCount - 1));

		arrIQData[2 * (dataCount - 1) + 1] *= arrWinAdd[dataCount - 1];
		arrIQData[2 * (dataCount - 1) + 2] *= arrWinAdd[dataCount - 1];

	}


	m_pLog->LogFmtStrA(SPLOGLV_INFO, "####data calculate end.#nCnrDataLength(*4):%d#totalLen:%d", hexCnrDataLength, dataCount);
	data_capture_flag = CW_START_END;
	memset(arrFFTMagData, 0, sizeof(arrFFTMagData));
	n = (int)(log((double)m_nDataCount) / log(2.0));
	FFT(arrIQData, m_nDataCount, -1);

	CalMag(arrIQData, arrFFTMagData);
	DataSwap(arrFFTMagData, m_nDataCount);
	FindMax(arrFFTMagData, m_nDataCount);

	snr = CalSNR(arrFFTMagData, fMaxMag, (int)g_MaxIndex);

	g_CN0 = (int)(snr + 10 * log10(2e6));
	m_pLog->LogFmtStrA(SPLOGLV_INFO, "########CNR:%d########\n", g_CN0);
	return g_CN0;
}

//parse the cw raw data
void CCalCNR::cw_data_capture(const char* nmea, int length)
{
	unsigned char data1 = 0, data2 = 0, data3 = 0, data4 = 0;
	long IHexVal = 0;
	long QHexVal = 0;
	double snr = 0.0;
	double arrFFTMagData[0xFFFF] = {0};
	int n = 0;
	static int totalLen = 0;

	if (NULL == nmea)
	{
		return;
	}

	//parse the data 
	if (CW_RAW_DATA_LEN == length)
	{
		const char* pData = nmea;
		int   tmpLen = length;


		if (CW_START_BEGIN == data_capture_flag)
		{
			totalLen += length;
			while ((tmpLen > 0) && (dataCount < (m_nDataCount - CW_RAW_DATA_LEN / 4)))
			{
				//"af002100"
				data1 = *pData++;//0xaf
				data2 = *pData++;//0x00
				data3 = *pData++;//0x21
				data4 = *pData++;//0x00
				dataCount++;
				IHexVal = (data2 << 8) + data1;
				QHexVal = (data4 << 8) + data3;
#ifdef GNSS_MARLIN3
				IHexVal = IHexVal > 256 ? IHexVal - 512 : IHexVal;
				QHexVal = QHexVal > 256 ? QHexVal - 512 : QHexVal;
#else
				IHexVal = IHexVal > 32768 ? IHexVal - 65535 : IHexVal;
				QHexVal = QHexVal > 32768 ? QHexVal - 65535 : QHexVal;
#endif           
				arrIQData[2 * (dataCount - 1) + 1] = (double)IHexVal;
				arrIQData[2 * (dataCount - 1) + 2] = (double)QHexVal * -1;

				arrWinAdd[dataCount - 1] = A0 - A1 * cos(2 * PI * (dataCount - 1) / (m_nDataCount - 1))
					+ A2 * cos(4 * PI * (dataCount - 1) / (m_nDataCount - 1))
					- A3 * cos(6 * PI * (dataCount - 1) / (m_nDataCount - 1))
					+ A4 * cos(8 * PI * (dataCount - 1) / (m_nDataCount - 1));

				arrIQData[2 * (dataCount - 1) + 1] *= arrWinAdd[dataCount - 1];
				arrIQData[2 * (dataCount - 1) + 2] *= arrWinAdd[dataCount - 1];
				tmpLen -= 4;
			}
		}
		else
		{
			//E("########There are some cw raw  len=%d, data_capture_flag%d########\n", length, data_capture_flag);
		}

	}
	else //others ,start or end or error 
	{
		if (NULL != strstr(nmea, "DATA_CAPTURE_BEGIN"))
		{
			//D("####data capture begin.");
			data_capture_flag = CW_START_BEGIN;
			dataCount = 0;
			fMaxMag = 0;
			//g_MaxIndex = 0;
			totalLen = 0;
			memset(arrIQData, 0, sizeof(arrIQData));
			memset(arrWinAdd, 0, sizeof(arrWinAdd));
		}
		else if (NULL != strstr(nmea, "DATA_CAPTURE_END"))
		{
			//D("####data capture end.totalLen:%d", totalLen);

			data_capture_flag = CW_START_END;
			memset(arrFFTMagData, 0, sizeof(arrFFTMagData));
			n = (int)(log((double)m_nDataCount) / log(2.0));
			FFT(arrIQData, m_nDataCount, -1);

			CalMag(arrIQData, arrFFTMagData);
			DataSwap(arrFFTMagData, m_nDataCount);
			FindMax(arrFFTMagData, m_nDataCount);

			snr = CalSNR(arrFFTMagData, fMaxMag, (int)g_MaxIndex);

			g_CN0 = (int)(snr + 10 * log10(2e6));
			//D("########CN0:%d########\n", g_CN0);

		}
		else
		{
			data_capture_flag = CW_START_UNKWON;
			//E("########len=%d, data:%s########\n", length, nmea);
		}
	}
}