#pragma once
#include "..//ImpBase.h"

///////////////////////////////////////
class CAMU_Keypad :public CImpBase
{
	DECLARE_RUNTIME_CLASS(CAMU_Keypad)

public:
	CAMU_Keypad(void);
	virtual ~CAMU_Keypad(void);

protected:
	virtual SPRESULT __PollAction(void);
	virtual BOOL LoadXMLConfig(void);

	struct keyValue
	{
		int8 iOperate; // BBAT_KEYPAD
	};

	struct keyValueRecv
	{
		int8 KeyCount;
		int8 iKeyValue[10];
	};

	enum BBAT_KEYPAD
	{
		OPEN = 1,
		READ = 2,
		CLOSE = 3,
		RESERVED = 4,
		MULTIREAD = 5
	};

	enum KeyValueCount
	{

	};

	
private:
    int m_iKeyNumber = 0;
	std::string m_strKeyValue;

	SPRESULT RunOpen();
	SPRESULT RunMultiRead();
	SPRESULT CheckKeyVal(keyValueRecv DataRecv);
	SPRESULT RunClose();
};