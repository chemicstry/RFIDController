#ifndef _DESFIRE_H_
#define _DESFIRE_H_

#include "Exception.h"
#include "DesfireKey.h"

#include <freefare.h>
#include <string>
#include <vector>

class Desfire
{
public:
    Desfire(FreefareTag& tag);
    ~Desfire();
    
    void Authenticate(uint8_t keyno, DesfireKey& key);
    
    void ChangeKey(uint8_t keyno, DesfireKey& newkey, DesfireKey& oldkey);
    
    std::string GetUID();

private:
    FreefareTag& tag;
};

#endif
