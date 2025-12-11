#pragma once

typedef int  (__stdcall*pStart_New)(char parSFC[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parRemark[], char *retMessage);
typedef int  (__stdcall*pComplete_New)(char parSFC[], char parBoardCount[], char parQualityBatchNum[], char parWorkStation[], char parRemark[], char *retMessage);
typedef int  (__stdcall*pNcComplete_New)(char parSFC[], char parNcType[], char parNcCode[], char parNcContext[], char parFailItem[], char parFailValue[], char parBoardCount[], char parWorkStation[], char parLogOperation[], char parLogResource[], char parNcPlace[], char parCreateUser[], char parOldStationName[], char parRemark[], char *retMessage);
typedef int  (__stdcall*pSfcKeyCollect_New)(char parSFC[], char parData[], char *retMessage);
typedef int  (__stdcall*pGetNumberBySfc_New)(char parSFC[], char parNumberStore[], char parModel[], char parModuleID[], char parCustomStatus[], char parRemark[], char *retNumber, char *retMessage);
typedef int  (__stdcall*pGetCustomDatabyShoporder_New)(char *retCustomData, char *retMessage);

typedef struct _tagBYD_DRIVER_T
{
	pStart_New Start_New;
	pComplete_New Complete_New;
	pNcComplete_New NcComplete_New;
	pSfcKeyCollect_New SfcKeyCollect_New;
	pGetNumberBySfc_New GetNumberBySfc_New;
	pGetCustomDatabyShoporder_New GetCustomDatabyShoporder_New;

    _tagBYD_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
		Start_New = NULL;
		Complete_New = NULL;
		NcComplete_New = NULL;
		SfcKeyCollect_New = NULL;
		GetNumberBySfc_New = NULL;
		GetCustomDatabyShoporder_New = NULL;
    };

    BOOL IsValid(void) {
        return ( (NULL != Start_New) && (NULL != Complete_New) && (NULL != NcComplete_New) && (NULL != SfcKeyCollect_New) && (NULL != GetNumberBySfc_New) && (NULL != GetCustomDatabyShoporder_New));
    };

} BYD_DRIVER_T;

