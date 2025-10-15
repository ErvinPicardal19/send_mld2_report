#include <stdio.h>
#include <stdint.h>

void hexdump(void *buf, size_t len)
{
    char *hexits = "0123456789ABCDEF";
    uint8_t *byte = NULL;

    if(!buf)
    {
        return;
    }

    byte = buf;

    for(size_t i = 0; i < len; i++)
    {
        fprintf(stdout, "%c", hexits[((*(byte + i)) >> 4)]);
        fprintf(stdout, "%c", hexits[(*(byte + i)) & 0x0f]);

        if(((i+1) % 16) == 0)
            fprintf(stdout, "\n");
        else
            fprintf(stdout, " ");
    }

    fprintf(stdout, "\n");
}
