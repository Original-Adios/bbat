#pragma once

/// Calibration flag
#define ShareMemory_TG_CalibrationFlag      L"Global_adc_reserved[7]"
#define ShareMemory_W_CalibrationFlag       L"Global_WCDMA Test Flag"
#define ShareMemory_TD_CalibrationFlag      L"Global_TDS Test Flag for V3"

/// SN input
#define ShareMemory_SN1                     L"Global_SN1"
#define ShareMemory_SN2                     L"Global_SN2"
#define ShareMemory_SN_SIZE                 64

/// SecureBoot Version
#define ShareMemory_UBootSecureBoot         L"Global_UBootSecureBoot"

/// DeviceID
#define ShareMemory_DeviceID                L"Global_Device_ID"


#define ShareMemory_GSM_SaveToFlash_PreSet	L"ShareMemory_GSM_SaveToFlash_PreSet"		//flag集合
#define ShareMemory_GSM_SaveToFlash_Done	L"ShareMemory_GSM_SaveToFlash_Done"		//已经完成
#define GSM_AFC_DONE_FLAG			(0X01)
#define GSM_APC_DONE_FLAG			(0X02)
#define GSM_AGC_DONE_FLAG			(0X04)
#define EDGE_APC_DONE_FLAG			(0X08)

/// VBUS PowerSupply
#define ShareMemory_RemoteVBusPowerSupply   L"Global_RemoteVBusPowerSupply"
#define ShareMemory_VBusPowerSupplyObject   L"Global_VBusPowerSupplyObject"

///
#define ShareMemory_ICCID                   L"Global_ICCID"
#define ShareMemory_ICCID_SIZE              32
#define ShareMemory_IMSI                    L"Global_IMSI"
#define ShareMemory_IMSI_SIZE               32
#define ShareMemory_EID                     L"Global_EID"
#define ShareMemory_EID_SIZE                32
#define ShareMemory_UID                     L"Global_UID"
#define ShareMemory_UID_SIZE                256
#define ShareMemory_IMEI1                   L"Global_IMEI1"
#define ShareMemory_IMEI2                   L"Global_IMEI2"
#define ShareMemory_IMEI_SIZE               16
#define ShareMemory_MEID1                   L"Global_MEID1"
#define ShareMemory_MEID2                   L"Global_MEID2"
#define ShareMemory_MEID_SIZE               16


//MES
#define ShareMemory_MES_Handle_Unisoc					L"Global_MES_Handle_Unisoc"	
#define ShareMemory_MES_Result_Guid						L"Global_MES_Result_Guid"
#define ShareMemory_MES_Start_Time						L"Global_MES_Start_Time"
#define ShareMemory_MES_Assigned_Codes					L"Global_MES_Assigned_Codes"
#define ShareMemory_MES_Device_Codes					L"Global_MES_Device_Codes"
#define ShareMemory_MES_V1_Batch_info					L"Global_MES_V1_Batch_info"
#define ShareMemory_MES_New_Log_Path					L"Global_MES_New_Log_Path"
#define ShareMemory_MES_Test_Env					    L"Global_MES_Test_Environment"
#define ShareMemory_MES_Test_Tool						L"Global_MES_Test_Tool"
#define ShareMemory_MES_Check_Flow_SN				    L"Global_MES_Check_Flow_SN	"
#define ShareMemory_SW_AP_VERSION						L"Global_Sw_Ap_Version"
#define ShareMemory_SW_CP_VERSION				        L"Global_Sw_Cp_Version"
#define ShareMemory_MCU_VERSION				            L"Global_Mcu_Version"

// SSID: 32bytes PSK: 64bytes
#define ShareMemory_SSID                                L"ShareMemory_SSID"
#define ShareMemory_PSK                                 L"ShareMemory_SSID_PSK"

#define ShareMemory_DeviceFilePath                      L"ShareMemory_DeviceFilePath"

#define ShareMemory_BackupNVFilePath                    L"ShareMemory_BackupNVFilePath"

#define ShareMemory_CsvStartTime                        L"ShareMemory_CsvStartTime"

// IP & Port
typedef struct _IP_T 
{
    CHAR  szIP[16];
    DWORD dwPort;
    _IP_T(void) {
        memset(this, 0, sizeof(*this));
    }
} IP_T;
#define ShareMemory_IP                            L"ShareMemory_IP"