#include "Desfire.h"

#include <iostream>
#include <freefare.h>

#include <err.h>
#include <errno.h>
#include <stdlib.h>

using namespace std;


uint8_t key_data_null[8]  = { 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00 };
uint8_t key_data_new[16]  = { 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x03, 0x10, 0x00, 0x00 };


int main()
{
    int error = EXIT_SUCCESS;
    nfc_device *device = NULL;
    FreefareTag *tags = NULL;

    nfc_connstring devices[8];
    size_t device_count;

    nfc_context *context;
    nfc_init(&context);
    
    if (context == NULL)
        errx(EXIT_FAILURE, "Unable to init libnfc (malloc)");

    device_count = nfc_list_devices(context, devices, 8);
    
    if (device_count <= 0)
        errx(EXIT_FAILURE, "No NFC device found.");

    for (size_t d = 0; d < device_count; d++) {
        device = nfc_open(context, devices[d]);
        
        if (!device) {
            warnx("nfc_open() failed.");
            error = EXIT_FAILURE;
            continue;
        }

        tags = freefare_get_tags (device);
        
        if (!tags) {
            nfc_close (device);
            errx (EXIT_FAILURE, "Error listing tags.");
        }
        
        if (!tags[0])
            warnx("No tags detected");

        for (int i = 0; (!error) && tags[i]; i++)
        {
            try {
                Desfire desfire(tags[i]);
                
                cout << "UID: " << desfire.GetUID() << endl;
                
                DesfireKeyDES key(key_data_null);
                DesfireKeyAES keynew(key_data_new);
                
                desfire.Authenticate(0, key);
                desfire.ChangeKey(0, keynew, key);
            } catch (exception& e) {
                cout << "Exception: " << e.what() << endl;
            }
        }
        
        freefare_free_tags(tags);
        nfc_close(device);
    }
    
    nfc_exit(context);
    
    return 0;
}
