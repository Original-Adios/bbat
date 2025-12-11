#pragma once
#include "global_def.h"

#define ShareMemory_My_UserInputBarcode                L"My_6A02D9C7_67AF_4E33_99FA_D5A743FF7D60_InputBarcode"
#define ShareMemory_My_UserInputSN                     L"My_7B4A9EB1-86F0-4868-853A-6EBED9D78A93_InputSN"
#define ShareMemory_My_UserInputIMEI                   L"My_59123B21-FD18-4E82-A535-6D5ECC29A094_InputIMEI"
#define ShareMemory_My_UserInputWIFI                   L"My_8197BFB3-BE83-4226-A7AF-AEB1457FCD31_InputWIFI"
#define ShareMemory_My_UserInputBT                     L"My_8A4C5000-500C-42A3-8500-7C452EF84EEF_InputBT"
#define ShareMemory_My_UpdateSN                        L"My_2505373C-9D47-44A0-BC58-E7DD0860F792_UpdateSN"
#define ShareMemory_My_UserInputCustSN                 L"My_7B4A9EB1-86F0-4868-853A-6EBED9D78A93_InputCustSN"
/*InputSN define*/
#define MAX_SN_COUNT    2
#define MAX_IMEI_COUNT    4
#define BARCODEMAXLENGTH (64)

typedef enum 
{
	E_GENCODE_MAN,
	E_GENCODE_AUTO,
	E_GENCODE_SECTION,
	E_GENCODE_SAMEASIMEI1,
	E_GENCODE_FROMIMEI1,
	E_GENCODE_SAMEASSN1,
	E_GENCODE_MAX
}E_GEN_CODE_TYPE;

typedef struct _tagINPUT_CODES_T
{
	UINT8 bEnable;
	UINT8 nMaxLength;
	CHAR szCodeName[30];
	E_GEN_CODE_TYPE eGenCodeType;
	CHAR szPrefix[BARCODEMAXLENGTH + 1];
	CHAR szValidation[BARCODEMAXLENGTH * 2 + 1];
	CHAR szCode[BARCODEMAXLENGTH + 1];
	CHAR szInitCode[BARCODEMAXLENGTH +1];
	unsigned __int64 nSectionStart;
	unsigned __int64 nSectionEnd;
	unsigned __int64 nSectionID;

	STRUCT_INITIALIZE(_tagINPUT_CODES_T);
} INPUT_CODES_T;

/////////////////////////

/*InputBarcode define*/
#define MAX_BARCODE_COUNT   10
#define MAX_BARCODE_LENGTH  64

typedef struct _tagBARCODE_T
{
    CHAR szBarcode[MAX_BARCODE_COUNT][MAX_BARCODE_LENGTH];

    STRUCT_INITIALIZE(_tagBARCODE_T);

} BARCODE_T;

static LPCWSTR g_BARCODE_SOURCE[ ] = {
    L"NONE",
    L"Barcode1",
    L"Barcode2",
    L"Barcode3",
    L"Barcode4",
    L"Barcode5",
    L"Barcode6",
    L"Barcode7",
    L"Barcode8",
    L"Barcode9",
    L"Barcode10"
};

typedef enum _EM_BARCODE_SOURCE
{
    BARCODE_NONE,
    BARCODE_1,
    BARCODE_2,
    BARCODE_3,
    BARCODE_4,
    BARCODE_5,
    BARCODE_6,
    BARCODE_7,
    BARCODE_8,
    BARCODE_9,
    BARCODE_10,

    MAX_BARCODE_SOURCE
}EM_BARCODE_SOURCE;
////////////////////////////////
