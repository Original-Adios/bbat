#pragma once
#include "ImpBase.h"
//////////////////////////////////////////////////////////////////////////
/* 
   SPCSS00476367

The format of the ICCID is: MMCC IINN NNNN NNNN NN C x 
MM = Constant (ISO 7812 Major Industry Identifier)
CC = Country Code
II = Issuer Identifier
N{12} = Account ID ("SIM number")
C = Checksum calculated from the other 19 digits using the Luhn algorithm.
x = An extra 20th digit is returned by the 'AT!ICCID?' command, but it is not officially part of the ICCID.
*/
class CLoadICCID : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CLoadICCID)
public:
    CLoadICCID(void);
    virtual ~CLoadICCID(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strCode;
};
