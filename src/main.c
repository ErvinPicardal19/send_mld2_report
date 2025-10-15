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
    fprintf(stdout,
            "Usage: %s <ifname> <BLOCK/ALLOW/INCLUDE/EXCLUDE> <GROUP> <SOURCES>\n\n"
            "    ex.\n"
            "        %s eth0 ALLOW ff3e::1:1 882::1:1:1,882::2:2:2...\n"
            ,exec_name, exec_name);
    exit(1);
}


int main(int argc, char *argv[])
{
    int i = 0, num_of_srcs = 0;
    struct in6_addr group = {0};
    char *ifname = NULL;
    char *source = NULL;
    struct in6_addr sources[MAX_SOURCES] = {0};
    enum MLD2_TYPES type;

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
        goto err_exit;
    }

    for((source = strtok(argv[4], ","), i = 0); (source != NULL && i < MAX_SOURCES);
            (source = strtok(NULL, ","), i++))
    {
        if(inet_pton(AF_INET6, source, &sources[i]) <= 0)
        {
            LOG(ERROR, "%s is not a valid AF_INET6 address", source);
            goto err_exit;
        }
    }

    num_of_srcs = (i+1);

    if(strncmp(argv[2], "BLOCK", sizeof("BLOCK")) == 0)
    {
        LOG(INFO, "Sending MLDv2 Report type BLOCK group %s for sources %s", argv[3], argv[4]);
        type =  BLOCK;
        //send_mldv2_report(ifname, 1, &group, &source, 1);
    }
    else if(strncmp(argv[2], "ALLOW", sizeof("ALLOW")) == 0)
    {
        LOG(INFO, "Sending MLDv2 Report type ALLOW group %s for sources %s", argv[3], argv[4]);
        type =  ALLOW;
    }
    else if(strncmp(argv[2], "INCLUDE", sizeof("INCLUDE")) == 0)
    {
        LOG(INFO, "Sending MLDv2 Report type INCLUDE group %s for sources %s", argv[3], argv[4]);
        type =  INCLUDE;
    }
    else if(strncmp(argv[2], "EXCLUDE", sizeof("EXCLUDE")) == 0)
    {
        LOG(INFO, "Sending MLDv2 Report type EXCLUDE group %s for sources %s", argv[3], argv[4]);
        type =  EXCLUDE;
    }
    else
    {
        LOG(ERROR, "%s is not a valid MLDv2 Record Type", argv[2]);
        goto err_exit;
    }

    send_mldv2_report(ifname, type, &group, sources, num_of_srcs);

exit:
    return 0;
err_exit:
    return 1;
}
