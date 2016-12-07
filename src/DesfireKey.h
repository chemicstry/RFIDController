#ifndef _DESFIRE_KEY_H_
#define _DESFIRE_KEY_H_

#include "PCH.h"


// These values must be OR-ed with the key number when executing command DF_INS_CHANGE_KEY
/*enum DesFireKeyType
{
    DF_KEY_2K3DES  = 0x00, // for DFEV1_INS_AUTHENTICATE_ISO + DF_INS_AUTHENTICATE_LEGACY
    DF_KEY_3K3DES  = 0x40, // for DFEV1_INS_AUTHENTICATE_ISO
    DF_KEY_AES     = 0x80, // for DFEV1_INS_AUTHENTICATE_AES
    DF_KEY_INVALID = 0xFF    
};*/

enum DesfireKeyType
{
    KEY_DES,
	KEY_3DES,
	KEY_3K3DES,
	KEY_AES
};

class DesfireKey
{
public:
    ~DesfireKey()
    {
        mifare_desfire_key_free(key);
    }
    
    virtual DesfireKeyType Type() const = 0;
    
    MifareDESFireKey key;
};

class DesfireKeyDES: public DesfireKey
{
public:
    DesfireKeyDES(const uint8_t value[8])
    {
        key = mifare_desfire_des_key_new_with_version(value);
    }
    
    DesfireKeyType Type() const
    {
        return KEY_DES;
    }
};

class DesfireKeyAES: public DesfireKey
{
public:
    DesfireKeyAES(const uint8_t value[16])
    {
        key = mifare_desfire_aes_key_new(value);
    }
    
    DesfireKeyType Type() const
    {
        return KEY_AES;
    }
};

#endif
