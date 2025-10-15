#include <stdint.h>

#include "packet.h"

char output[1024] = {0};

char *hexdump(void *buf, size_t size)
{
    char *hexits = "0123456789ABCDEF";
    uint8_t *byte = NULL;
    size_t len = 0;

    if(!buf)
    {
        return NULL;
    }

    byte = buf;

    for(size_t i = 0; i < size; i++)
    {
        len += snprintf(output+len, sizeof(output)-len, "%c", hexits[((*(byte + i)) >> 4)]);
        len += snprintf(output+len, sizeof(output)-len, "%c", hexits[(*(byte + i)) & 0x0f]);
        //fprintf(stdout, "%c", hexits[((*(byte + i)) >> 4)]);
        //fprintf(stdout, "%c", hexits[(*(byte + i)) & 0x0f]);

        if(((i+1) % 16) == 0)
            len += snprintf(output+len, sizeof(output)-len, "\n");
        else
            len += snprintf(output+len, sizeof(output)-len, " ");
    }

    return output;
}
