#ifndef __MLD_H__
#define __MLD_H__

#include <stdint.h>
#include <linux/in6.h>

#define MAX_SOURCES 10

enum MLD2_TYPES {
    INCLUDE = 1,
    EXCLUDE,
    ALLOW,
    BLOCK
};

struct ip6_mld_hopopt_hdr
{
    uint8_t next_hdr;           // 0x3a (ICMPv6 Header)
    uint8_t ext_hdr_len;        // 0x00
    uint8_t rtr_alert_type;     // 0x05 (Rotuer Alert)
    uint8_t rtr_alert_len;       // 0x02 (2 bytes)
    uint16_t rtr_alert_val;      // 0x00 (MLD)
    uint8_t pad_type;           // 0x01
    uint8_t pad_len;            // 0x00
} __attribute__((packed));

struct mldv2_record
{
    uint8_t rec_type;
    uint8_t aux_data_len;
    uint16_t num_of_src;
    struct in6_addr group;
    char srcs[];
} __attribute__((packed));

struct icmp6_mldv2_hdr
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t reserved;
    uint16_t num_of_rec;
} __attribute__((packed));

void send_mldv2_report(char *ifname, enum MLD2_TYPES type, struct in6_addr *group, struct in6_addr *srcs, uint16_t num_of_src);

#endif//__MLD_H__
