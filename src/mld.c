#include <stdio.h>
#include <linux/icmpv6.h>
#include <linux/ipv6.h>
#include <netinet/ip6.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "mld.h"
#include "log/logger.h"
#include "utils/packet.h"

#define IS_VALID_LINKLOCAL(ip6) (((ip6).s6_addr[0] == 0xfe) && (((ip6).s6_addr[1] & 0xc0) == 0x80))

int compute_checksum(const uint8_t *data, size_t len)
{
    int sum = 0;

    while(len >= 2)
    {
        sum += (data[0] << 8) | data[1];
        data += 2;
        len -= 2;
    }

    if(len == 1)
    {
        sum += (data[0] << 8);
    }

    while(sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return (uint16_t)~sum;
}

int icmpv6_checksum(void *packet, const size_t len)
{
    struct __attribute__((packed))
    {
        struct in6_addr src;
        struct in6_addr dst;
        uint32_t upper_pkt_len;
        uint8_t zero[3];
        uint8_t next_hdr;
    } *ip6_pseudo_hdr = NULL;

    size_t icmpv6_len = (len - (sizeof(struct ip6_hdr) + sizeof(struct ip6_mld_hopopt_hdr)));
    char buf[512] = {0};

    //LOG(DEBUG, "ICMP len %ld", icmpv6_len);

    ip6_pseudo_hdr = (void *)buf;
    struct ip6_hdr *ip6_hdr = (struct ip6_hdr *)packet;
    struct icmp6_mldv2_hdr *icmp6_hdr = (struct icmp6_mldv2_hdr *)((char *)packet + (len - icmpv6_len));

    if(!ip6_hdr)
        return -1;

    memcpy(&ip6_pseudo_hdr->src, &ip6_hdr->ip6_src, sizeof(struct in6_addr));
    memcpy(&ip6_pseudo_hdr->dst, &ip6_hdr->ip6_dst, sizeof(struct in6_addr));
    ip6_pseudo_hdr->upper_pkt_len = htonl(icmpv6_len);
    ip6_pseudo_hdr->next_hdr = IPPROTO_ICMPV6;

    memcpy(((char *)buf + sizeof(*ip6_pseudo_hdr)), icmp6_hdr, icmpv6_len);

    //hexdump(buf, (sizeof(*ip6_pseudo_hdr) + icmpv6_len));
    return compute_checksum((void *)buf, (sizeof(*ip6_pseudo_hdr) + icmpv6_len));
}

int get_ip6addr_by_name(char *ifname, struct in6_addr *ip6)
{
    struct ifaddrs *ifap, *tmp = NULL;
    struct sockaddr *sa = NULL;
    struct sockaddr_in6 *sa6 = NULL;

    if(getifaddrs(&ifap) < 0)
    {
        return -1;
    }

    for(tmp = ifap; tmp != NULL; tmp = tmp->ifa_next)
    {
        if(strncmp(tmp->ifa_name, ifname, IFNAMSIZ) != 0)
            continue;

        sa = tmp->ifa_addr;
        if(sa->sa_family == AF_INET6 &&
                ((sa6 = (struct sockaddr_in6 *)sa) && (IS_VALID_LINKLOCAL(sa6->sin6_addr))))
        {
            memcpy(ip6, &sa6->sin6_addr, sizeof(struct in6_addr));
            return 0;
        }

    }

    return -1;
}

void send_mldv2_report(char *ifname, enum MLD2_TYPES type, struct in6_addr *group, struct in6_addr *srcs, uint16_t num_of_src)
{
    struct
    {
        struct ip6_hdr ip6_hdr;
        struct ip6_mld_hopopt_hdr hopopt_hdr;
        struct icmp6_mldv2_hdr mldv2_hdr;
        char mld_records[];
    } *hdr = NULL;
    struct mldv2_record *mld_record;
    struct in6_addr *src;
    char buf[1024] = {0};
    char addr6[INET6_ADDRSTRLEN] = {0};
    int payload_len = 0;
    int fd = -1;
    struct sockaddr_in6 dst = {0};

    hdr = (void *)buf;
    hdr->ip6_hdr.ip6_vfc = 0x60;
    payload_len = (sizeof(struct ip6_mld_hopopt_hdr) + sizeof(struct icmp6_mldv2_hdr));

    hdr->ip6_hdr.ip6_nxt = IPPROTO_HOPOPTS;
    hdr->ip6_hdr.ip6_hlim = 1;
    get_ip6addr_by_name(ifname, &hdr->ip6_hdr.ip6_src);
    inet_pton(AF_INET6, "ff02::16", &hdr->ip6_hdr.ip6_dst);

    hdr->hopopt_hdr.next_hdr = IPPROTO_ICMPV6;
    hdr->hopopt_hdr.rtr_alert_type = IPV6_TLV_ROUTERALERT;
    hdr->hopopt_hdr.rtr_alert_len = 0x02;
    hdr->hopopt_hdr.pad_type = 0x01;

    hdr->mldv2_hdr.type = ICMPV6_MLD2_REPORT;
    hdr->mldv2_hdr.num_of_rec = htons(0x0001); // Only 1 record for now

    mld_record = (struct mldv2_record *)hdr->mld_records;
    for(int i = 0; i < 1; i++)
    {

        size_t mld_len = (sizeof(struct mldv2_record) + (sizeof(struct in6_addr) * num_of_src));
        if((payload_len + mld_len) >= sizeof(buf))
        {
            break;
        }

        payload_len += mld_len;

        mld_record += i;
        switch(type)
        {
            case INCLUDE:
                mld_record->rec_type = MLD2_CHANGE_TO_INCLUDE;
                break;
            case EXCLUDE:
                mld_record->rec_type = MLD2_CHANGE_TO_EXCLUDE;
                break;
            case ALLOW:
                mld_record->rec_type = MLD2_ALLOW_NEW_SOURCES;
                break;
            case BLOCK:
                mld_record->rec_type = MLD2_BLOCK_OLD_SOURCES;
                break;
        }
        mld_record->aux_data_len = 0;
        mld_record->num_of_src = htons(num_of_src);
        memcpy(&mld_record->group, group, sizeof(struct in6_addr));

        src = (struct in6_addr *)mld_record->srcs;
        for(int j = 0; j < num_of_src; j++)
        {
            src += j;
            memcpy(src, &srcs[j], sizeof(struct in6_addr));
        }
    }

    hdr->ip6_hdr.ip6_plen = htons(payload_len);
    hdr->mldv2_hdr.checksum = htons(icmpv6_checksum(buf, (sizeof(struct ip6_hdr) + payload_len)));

    LOG(DEBUG, "Packet: ");
    LOG(DEBUG, "%s", hexdump(buf, (sizeof(struct ip6_hdr) + payload_len)));

    fd = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW);
    if(fd < 0)
    {
        return;
    }

    dst.sin6_family = AF_INET6;
    memcpy(&dst.sin6_addr, &hdr->ip6_hdr.ip6_dst, sizeof(struct in6_addr));
    inet_ntop(AF_INET6, &dst.sin6_addr, addr6, INET6_ADDRSTRLEN);
    dst.sin6_scope_id = if_nametoindex(ifname);

    LOG(DEBUG, "Sending MLDv2 Report to (%s%%%d)", addr6, dst.sin6_scope_id);
    if(sendto(fd, buf, (sizeof(struct ip6_hdr) + payload_len), 0, (struct sockaddr *)&dst, sizeof(dst)) < 0)
    {
        LOG(ERROR, "%s", strerror(errno));
    }

    close(fd);
}
