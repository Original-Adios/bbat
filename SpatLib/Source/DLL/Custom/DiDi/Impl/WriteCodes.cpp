#include "StdAfx.h"
#include "WriteCodes.h"
#include "InputBarCodes.h"
#include "UeHelper.h"

IMPLEMENT_RUNTIME_CLASS(CWriteCodes)
//////////////////////////////////////////////////////////////////////////
CWriteCodes::CWriteCodes(void)
{

}

CWriteCodes::~CWriteCodes(void)
{
}

SPRESULT CWriteCodes::__PollAction(void)
{
    BarCode_T barCodes[MAX_BARCODE_NUM];
    SPRESULT res = GetShareMemory(ShareMemory_DiDiInputCodes, (void* )&barCodes[0], sizeof(BarCode_T)*MAX_BARCODE_NUM);
    if (SP_OK != res)
    {
        NOTIFY("No BarCode", LEVEL_ITEM, 1, 1, 1, NULL, -1, NULL);
        return res;
    }

    CUeHelper ue(this);
    for (INT i=0; i<MAX_BARCODE_NUM; i++)
    {
        if (!barCodes[i].bActived)
        {
            continue;
        }

        if (0 == _stricmp(barCodes[i].szLabel, "SN1"))
        {
            CHKRESULT(ue.WriteSN(SN1, barCodes[i].szCode));
        }
        else if (0 == _stricmp(barCodes[i].szLabel, "IMEI1"))
        {
            PC_PRODUCT_DATA data;
            data.u32OperMask = FNAMASK_RW_IMEI1;
            strncpy_s((CHAR* )&data.szImei1[0], MAX_IMEI_STR_LENGTH, barCodes[i].szCode, CopySize(data.szImei1));
            CHKRESULT(ue.WriteProductData(data));
        }
        else if (0 == _stricmp(barCodes[i].szLabel, "WIFI"))
        {
            PC_PRODUCT_DATA data;
            data.u32OperMask = FNAMASK_RW_WIFIADDR;
            strncpy_s((CHAR* )&data.szWIFIAddr[0], MAX_WIFI_ADDR_STR_LENGTH, barCodes[i].szCode, CopySize(data.szWIFIAddr));
            CHKRESULT(ue.WriteProductData(data));
        }
        else if (0 == _stricmp(barCodes[i].szLabel, "BT"))
        {
            PC_PRODUCT_DATA data;
            data.u32OperMask = FNAMASK_RW_BTADDR;
            strncpy_s((CHAR* )&data.szBTAddr[0], MAX_BT_ADDR_STR_LENGTH, barCodes[i].szCode, CopySize(data.szBTAddr));
            CHKRESULT(ue.WriteProductData(data));
        }
    }

    return SP_OK;
}