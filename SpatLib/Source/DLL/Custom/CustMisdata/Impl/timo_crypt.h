#pragma once
class timo_crypt
{
public:
	static std::string EncryptSSNToS1(std::string sn);
	static std::string EncryptSSN(std::string sn);
	static std::string EncryptSSNToJoy2S(std::string sn);
	//static std::string DecryptSSN(std::string ssn);
	static std::string EncryptSSNToJoy3(std::string sn);
	static std::string EncryptSSNToM4(std::string sn);
	static std::string EncryptSSNToT10(std::string sn);
	static std::string EncryptSSNToQ1(std::string sn);
	static std::string EncryptSSNToK1PF(std::string sn);
	static std::string EncryptSSNToTeemo(const char *in_sn, const char *pro_name,  const char* const *teemo_key);
};