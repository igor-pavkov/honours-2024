static int lan9118_filter(lan9118_state *s, const uint8_t *addr)
{
    int multicast;
    uint32_t hash;

    if (s->mac_cr & MAC_CR_PRMS) {
        return 1;
    }
    if (addr[0] == 0xff && addr[1] == 0xff && addr[2] == 0xff &&
        addr[3] == 0xff && addr[4] == 0xff && addr[5] == 0xff) {
        return (s->mac_cr & MAC_CR_BCAST) == 0;
    }

    multicast = addr[0] & 1;
    if (multicast &&s->mac_cr & MAC_CR_MCPAS) {
        return 1;
    }
    if (multicast ? (s->mac_cr & MAC_CR_HPFILT) == 0
                  : (s->mac_cr & MAC_CR_HO) == 0) {
        /* Exact matching.  */
        hash = memcmp(addr, s->conf.macaddr.a, 6);
        if (s->mac_cr & MAC_CR_INVFILT) {
            return hash != 0;
        } else {
            return hash == 0;
        }
    } else {
        /* Hash matching  */
        hash = net_crc32(addr, ETH_ALEN) >> 26;
        if (hash & 0x20) {
            return (s->mac_hashh >> (hash & 0x1f)) & 1;
        } else {
            return (s->mac_hashl >> (hash & 0x1f)) & 1;
        }
    }
}