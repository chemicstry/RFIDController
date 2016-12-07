#include "Desfire.h"

#include "PCH.h"
#include "ByteBuffer.h"
#include "Utils.h"

#include <crypto++/aes.h>
#include <crypto++/modes.h>

#include <iostream>
#include <iomanip>

BinaryData Decrypt(const BinaryData& data, const BinaryData& key, const BinaryData& iv)
{
    CryptoPP::AES::Decryption aesDecryption(key.data(), key.size());
    
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv.data());

    BinaryData dec;
    
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::VectorSink(dec), CryptoPP::StreamTransformationFilter::NO_PADDING);
    stfDecryptor.Put(data.data(), data.size());
    stfDecryptor.MessageEnd();
    
    return dec;
}

BinaryData Encrypt(const BinaryData& data, const BinaryData& key, const BinaryData& iv)
{
    CryptoPP::AES::Encryption aesEncryption(key.data(), key.size());
    
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv.data());

    BinaryData enc;
    
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::VectorSink(enc), CryptoPP::StreamTransformationFilter::NO_PADDING);
    stfEncryptor.Put(data.data(), data.size());
    stfEncryptor.MessageEnd();
    
    return enc;
}

struct freefare_tag {
    nfc_device *device;
    nfc_target info;
    int type;
    int active;
    void (*free_tag) (FreefareTag tag);
};

Desfire::Desfire(FreefareTag& tag): tag(tag)
{
    freefare_tag_type type = freefare_get_tag_type(tag);
    if (type != MIFARE_DESFIRE)
		throw Exception("Wrong card type: %u", type);

	int res = mifare_desfire_connect(tag);
    if (res < 0)
        throw Exception("mifare_desfire_connect failed: %d", res);
}

Desfire::~Desfire()
{
    mifare_desfire_disconnect(tag);
    freefare_free_tag(tag);
}

BinaryData Desfire::Transceive(uint8_t ins, const BinaryData& args)
{
    // Code from freefare lib (ugly)
    uint8_t __msg[MAX_CAPDU_SIZE + sizeof(ISO7816_4_CAPDU)];
    uint8_t __res[MAX_RAPDU_SIZE + 1];
    
    ISO7816_4_CAPDU* iso_capdu = (ISO7816_4_CAPDU*)__msg;
    
    iso_capdu->CLA = 0x90; // Command class
    iso_capdu->INS = ins; // Instruction
    iso_capdu->P1 = 0x00;
    iso_capdu->P2 = 0x00;
    iso_capdu->Lc = 0x00;
    
    if (args.size())
    {
        memcpy(__msg + sizeof(ISO7816_4_CAPDU), args.data(), args.size());
        iso_capdu->Lc = args.size();
    }
    
    // Set Le (Maximum number of bytes expected in the data field of the response to the command. 0 - no limit)
    __msg[sizeof(ISO7816_4_CAPDU) + args.size()] = 0x00;
    
    int len = sizeof(ISO7816_4_CAPDU) + args.size() + 1;
    
    int _res; // Bytes transferred if positive, error if negative
    if ((_res = nfc_initiator_transceive_bytes(tag->device, __msg, len, __res, sizeof(__res), 0)) < 0) {
        throw Exception("nfc_initiator_transceive_bytes: %d", _res);
    }
    
    if (_res < 2)
        throw Exception("Malformed ISO7816_4_RAPDU packet");
    
    ISO7816_4_RAPDU* iso_rapdu = (ISO7816_4_RAPDU*)(__res + _res - sizeof(ISO7816_4_RAPDU));
    
    if (iso_rapdu->SW2 != OPERATION_OK && iso_rapdu->SW2 != ADDITIONAL_FRAME)
    {
        if (iso_rapdu->SW2 == AUTHENTICATION_ERROR)
            throw Exception("Desfire::Transceive(): Authentication error");
        
        throw Exception("Desfire::Transceive(): Error SW1: %#02x, SW2: %#02x", iso_rapdu->SW1, iso_rapdu->SW2);
    }
    
    // Extract data
    BinaryData res;
    res.assign(__res, __res + _res - sizeof(ISO7816_4_RAPDU));
    
    return res;
}

void Desfire::Authenticate(const uint8_t keyno, const DesfireKey& key)
{
    int res = mifare_desfire_authenticate(tag, keyno, key.key);
	if (res < 0)
        throw Exception("mifare_desfire_authenticate failed: %d", res);
}

BinaryData Desfire::InitiateAuth(const uint8_t keyno, const DesfireKeyType type)
{
    uint8_t cmd = GetAuthCmd(type);

    ByteBuffer args;
    args << keyno;
    
    return Transceive(cmd, args.Binary());
}

BinaryData Desfire::ContinueAuth(const BinaryData& token)
{
    return Transceive(DF_INS_ADDITIONAL_FRAME, token);
}

uint8_t Desfire::GetAuthCmd(const DesfireKeyType& type)
{
    switch (type) {
        case KEY_DES:
        case KEY_3DES:
            return DF_INS_AUTHENTICATE_LEGACY;
        case KEY_3K3DES:
            return DFEV1_INS_AUTHENTICATE_ISO;
        case KEY_AES:
            return DFEV1_INS_AUTHENTICATE_AES;
    }

    return 0xFF;
}

/*void Desfire::CustomAuth(const uint8_t keyno, const DesfireKey& key)
{
    uint8_t cmd = GetAuthCmd(key);

    ByteBuffer args;
    args << keyno;
    
    BinaryData res = Transceive(cmd, args.Binary());
    
    PrintBin(res);
    
    uint8_t keyy[16]  = { 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00 };
    BinaryData bkey;
    bkey.assign(keyy, keyy+sizeof(keyy));
    
    BinaryData zero_iv(16, 0x00);
    
    BinaryData RndB = Decrypt(res, bkey, zero_iv);
    
    std::cout << "RndB: ";
    PrintBin(RndB);
    
    BinaryData RndBRot;
    RndBRot.assign(RndB.begin()+1, RndB.end());
    RndBRot.push_back(RndB[0]);
    
    std::cout << "RndBRot: ";
    PrintBin(RndBRot);
    
    BinaryData RndA;
    for (int i = 0; i < 16; ++i)
        RndA.push_back(rand() % 0xFF);
    
    std::cout << "RndA: ";
    PrintBin(RndA);
    
    ByteBuffer token;
    token << RndA;
    token << RndBRot;
    
    std::cout << "Token: ";
    PrintBin(token.Binary());
    
    BinaryData tokenEnc = Encrypt(token.Binary(), bkey, res);
    
    std::cout << "TokenEnc: ";
    PrintBin(tokenEnc);
    
    BinaryData res2 = Transceive(DF_INS_ADDITIONAL_FRAME, tokenEnc);
    
    std::cout << "res2: ";
    PrintBin(res2);
    
    BinaryData RndARot;
    RndARot.assign(RndA.begin()+1, RndA.end());
    RndARot.push_back(RndA[0]);
    
    std::cout << "RndARot: ";
    PrintBin(RndARot);
    
    BinaryData iv2;
    iv2.assign(tokenEnc.begin()+16, tokenEnc.end());
    
    BinaryData res2Dec = Decrypt(res2, bkey, iv2);
    
    std::cout << "res2Dec: ";
    PrintBin(res2Dec); 
    
}*/

void Desfire::ChangeKey(uint8_t keyno, DesfireKey& newkey, DesfireKey& oldkey)
{
    int res = mifare_desfire_change_key(tag, keyno, newkey.key, oldkey.key);
    if (res < 0)
        throw Exception("mifare_desfire_change_key failed: %d", res);
}

std::string Desfire::GetUID()
{
    return freefare_get_tag_uid(tag);
}
