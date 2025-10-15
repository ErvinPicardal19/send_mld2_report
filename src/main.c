#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "log/logger.h"
#include "mld.h"

static char *exec_name = NULL;

static void usage(void)
{
    fprintf(stdout, "Usage: %s <ifname> <block/allow> <group> <src>\n", exec_name);
    exit(1);
}


int main(int argc, char *argv[])
{
    struct in6_addr group = {0}, src = {0};
    char *ifname = NULL;
    int fd = -1;

    set_log_level(ERROR);

    exec_name = argv[0];

    if(argc < 2)
        usage();

    if(geteuid() > 0)
    {
        fprintf(stdout, "Please run as root or with sudo...\n");
        goto exit;
    }

    ifname = argv[1];

    if(inet_pton(AF_INET6, argv[3], &group) <= 0)
    {
        LOG(ERROR, "%s is not a valid AF_INET6 address", argv[3]);
    }

    if(inet_pton(AF_INET6, argv[4], &src) <= 0)
    {
        LOG(ERROR, "%s is not a valid AF_INET6 address", argv[4]);
    }

    if(strncmp(argv[2], "BLOCK", 5) == 0)
    {
        LOG(DEBUG, "Sending MLDv2 Report type BLOCK to %s from %s", argv[3], argv[4]);
        send_mldv2_report(ifname, 1, &group, &src, 1);
    }
    else
    {
        LOG(DEBUG, "Sending MLDv2 Report type ALLOW to %s from %s", argv[3], argv[4]);
        send_mldv2_report(ifname, 0, &group, &src, 1);
    }

exit:
    if(fd > 0)
        close(fd);
    return 0;
}
