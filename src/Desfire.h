#ifndef _DESFIRE_H_
#define _DESFIRE_H_

#include "PCH.h"

#include "Exception.h"
#include "DesfireKey.h"
#include "ByteBuffer.h"

#include <string>

/* Max APDU sizes to be ISO encapsulated by DESFIRE_TRANSCEIVE()
   From MIFARE DESFire Functional specification:
   MAX_CAPDU_SIZE:   "The length of the total wrapped DESFire
                      command is not longer than 55 byte long."
   MAX_RAPDU_SIZE:   1 status byte + 59 bytes
 */
#define MAX_CAPDU_SIZE 55
#define MAX_RAPDU_SIZE 60

struct ISO7816_4_CAPDU
{
    uint8_t CLA;
    uint8_t INS;
    uint8_t P1;
    uint8_t P2;
    uint8_t Lc;
};

struct ISO7816_4_RAPDU
{
    uint8_t SW1;
    uint8_t SW2;
};

// ------- Desfire legacy instructions --------

#define DF_INS_AUTHENTICATE_LEGACY        0x0A
#define DF_INS_CHANGE_KEY_SETTINGS        0x54
#define DF_INS_GET_KEY_SETTINGS           0x45
#define DF_INS_CHANGE_KEY                 0xC4
#define DF_INS_GET_KEY_VERSION            0x64

#define DF_INS_CREATE_APPLICATION         0xCA
#define DF_INS_DELETE_APPLICATION         0xDA
#define DF_INS_GET_APPLICATION_IDS        0x6A
#define DF_INS_SELECT_APPLICATION         0x5A

#define DF_INS_FORMAT_PICC                0xFC
#define DF_INS_GET_VERSION                0x60

#define DF_INS_GET_FILE_IDS               0x6F
#define DF_INS_GET_FILE_SETTINGS          0xF5
#define DF_INS_CHANGE_FILE_SETTINGS       0x5F
#define DF_INS_CREATE_STD_DATA_FILE       0xCD
#define DF_INS_CREATE_BACKUP_DATA_FILE    0xCB
#define DF_INS_CREATE_VALUE_FILE          0xCC
#define DF_INS_CREATE_LINEAR_RECORD_FILE  0xC1
#define DF_INS_CREATE_CYCLIC_RECORD_FILE  0xC0
#define DF_INS_DELETE_FILE                0xDF

#define DF_INS_READ_DATA                  0xBD
#define DF_INS_WRITE_DATA                 0x3D
#define DF_INS_GET_VALUE                  0x6C
#define DF_INS_CREDIT                     0x0C
#define DF_INS_DEBIT                      0xDC
#define DF_INS_LIMITED_CREDIT             0x1C
#define DF_INS_WRITE_RECORD               0x3B
#define DF_INS_READ_RECORDS               0xBB
#define DF_INS_CLEAR_RECORD_FILE          0xEB
#define DF_COMMIT_TRANSACTION             0xC7
#define DF_INS_ABORT_TRANSACTION          0xA7

#define DF_INS_ADDITIONAL_FRAME           0xAF // data did not fit into a frame, another frame will follow

// -------- Desfire EV1 instructions ----------

#define DFEV1_INS_AUTHENTICATE_ISO        0x1A
#define DFEV1_INS_AUTHENTICATE_AES        0xAA
#define DFEV1_INS_FREE_MEM                0x6E
#define DFEV1_INS_GET_DF_NAMES            0x6D
#define DFEV1_INS_GET_CARD_UID            0x51
#define DFEV1_INS_GET_ISO_FILE_IDS        0x61
#define DFEV1_INS_SET_CONFIGURATION       0x5C

// ---------- ISO7816 instructions ------------

#define ISO7816_INS_EXTERNAL_AUTHENTICATE 0x82
#define ISO7816_INS_INTERNAL_AUTHENTICATE 0x88
#define ISO7816_INS_APPEND_RECORD         0xE2
#define ISO7816_INS_GET_CHALLENGE         0x84
#define ISO7816_INS_READ_RECORDS          0xB2
#define ISO7816_INS_SELECT_FILE           0xA4
#define ISO7816_INS_READ_BINARY           0xB0
#define ISO7816_INS_UPDATE_BINARY         0xD6

class Desfire
{
public:
    Desfire(FreefareTag& tag);
    ~Desfire();
    
    BinaryData Transceive(const uint8_t cmd, const BinaryData& args);
    
    void Authenticate(const uint8_t keyno, const DesfireKey& key);
    
    BinaryData InitiateAuth(const uint8_t keyno, const DesfireKeyType type);
    BinaryData ContinueAuth(const BinaryData& token);
    
    uint8_t GetAuthCmd(const DesfireKeyType& type);
    //void CustomAuth(const uint8_t keyno, const DesfireKey& key);
    
    void ChangeKey(uint8_t keyno, DesfireKey& newkey, DesfireKey& oldkey);
    
    std::string GetUID();

private:
    FreefareTag& tag;
};

#endif
