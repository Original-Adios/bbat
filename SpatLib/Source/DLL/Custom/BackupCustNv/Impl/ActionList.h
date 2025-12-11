
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,            Version,       ClassName,          ActionName,        rcID,            rcName,                Description     
    ----------------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      0,               NULL,                   L"Action without RC"),

 */
	ACTION(L"BackNV", L"ReadBackupNV",  L"CReadBackupNV",  L"ReadBackupNV", IDR_XML_NV_FILE_NAME,     TOWSTR(IDR_XML_NV_FILE_NAME),    L"Read Backup NV [包括Fixnv,Prodnv,Miscdata]"),
	ACTION(L"BackNV", L"ReadBackupNvV2",  L"CReadBackupNvV2",  L"ReadBackupNvV2", IDR_XML_NV_FILEV2_NAME,     TOWSTR(IDR_XML_NV_FILEV2_NAME),    L"Read Backup NVV2 8520机器之后的项目[包括Fixnv,Prodnv,Miscdata]"),
    ACTION(L"BackNV", L"ReadBackupNvV3", L"CReadBackupNvV2", L"ReadBackupNvV3", IDR_XML_NV_FILEV3_NAME, TOWSTR(IDR_XML_NV_FILEV3_NAME), L"Read Backup NVV3 [包括Fixnv,Prodnv,Miscdata]"),

    ACTION(L"BackNV", L"WriteBackupNV", L"CWriteBackupNV", L"WriteBackupNV", IDR_XML_NV_SAVE_2PHONE, TOWSTR(IDR_XML_NV_SAVE_2PHONE), L"Write Backup NV [包括Fixnv,Prodnv,Miscdata]"),
    ACTION(L"BackNV", L"WriteBackupNvV2", L"CWriteBackupNvV2", L"WriteBackupNvV2", IDR_XML_NV_SAVEV2_2PHONE, TOWSTR(IDR_XML_NV_SAVEV2_2PHONE), L"Write Backup NVV2 8520机器之后的项目[包括Fixnv,Prodnv,Miscdata]"),
    ACTION(L"BackNV", L"WriteBackupNvV3", L"CWriteBackupNvV3", L"WriteBackupNvV3", IDR_XML_NV_SAVEV3_3PHONE, TOWSTR(IDR_XML_NV_SAVEV3_3PHONE), L"Write Backup NVV3 [包括Fixnv,Prodnv,Miscdata]")

#endif 
