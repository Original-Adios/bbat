#pragma once
class timo_crypt
{
public:
	static std::string EncryptSSNToS1(std::string sn);
	static std::string EncryptSSN(std::string sn);
	//static std::string DecryptSSN(std::string ssn);
};