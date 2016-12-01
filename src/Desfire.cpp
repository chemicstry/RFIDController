#include "Desfire.h"

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
}

void Desfire::Authenticate(uint8_t keyno, DesfireKey& key)
{
    int res = mifare_desfire_authenticate(tag, keyno, key.key);
	if (res < 0)
        throw Exception("mifare_desfire_authenticate failed: %d", res);
}

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
