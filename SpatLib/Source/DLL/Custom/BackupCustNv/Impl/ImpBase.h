#pragma once
#include "SpatBase.h"

extern "C"
{
#include "crc16.h"
}


//CONST CHAR *PNV_FILE_NAME[] = {"calinv", "miscnv", "prodnv"};

#define MAX_MISCDATA_ALL_SIZE (1024*1024) // 最大不能超过 1M
#define MISCDATA_ALL_OFFSET (0)
#define MAX_PATITION_COUNT (3)
//////////////////////////////////////////////////////////////////////////

#define MAX_DIAG_BUFF_SIZE      (0xFFFF)  
typedef enum
{
	FILE_NV_CAL,
	FILE_NV_PROD,
	FILE_NV_MISC
}BACKUP_FILE_TYPE;

#pragma pack(push, 1)
/*Nv FILE Backup struct*/
typedef struct _tagSEGMENT_HEAD_STRUCT
{
	char szFileName[8];
	UINT32 size;
	_tagSEGMENT_HEAD_STRUCT()
	{
		memset(this, 0, sizeof(*this));
	}

} SEGMENT_HEAD_STRUCT;

/*Nv FILE Backup struct*/
typedef struct _tagSEGMENT_STRUCT
{
	SEGMENT_HEAD_STRUCT szSegHead;
	uint8 *Nvcontent;
	_tagSEGMENT_STRUCT()
	{
		memset(this, 0, sizeof(*this));
	}

} SEGMENT_STRUCT;

typedef struct _tagBACKUP_FILE_HEAD
{
	uint32 magic;
	uint16 u16CalCrc;
	uint16 u16FileCount;
	_tagBACKUP_FILE_HEAD()
	{
		memset(this, 0, sizeof(*this));
	}
}BACKUP_FILE_HEAD;
#pragma pack(pop)

class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);
	
protected:
	SPRESULT QueryPartitionSize(uint32 *u32FixnvSize, uint32 *u32MiscnvSize, uint32 *u32ProdnvSize, uint16 *u32nvFileCount);
	SPRESULT QueryPartitionSizeV2(uint32* u32FixnvSize, uint32* u32MiscnvSize, uint32* u32ProdnvSize, uint16* u32nvFileCount);
	LPCSTR GetPartionName(uint8 i)
	{
		switch (i)
		{
		case 0:
			return "fixnv";
			break;
		case 1:
			return "miscdata";
			break;
		case 2:
			return "prodnv";
			break;
		case 3:
			return "fixnvV2";
			break;
		default:
			return "";
			break;
		}
	}
};
