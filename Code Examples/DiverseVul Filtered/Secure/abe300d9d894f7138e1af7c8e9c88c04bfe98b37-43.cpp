static int receive_filter(VirtIONet *n, const uint8_t *buf, int size)
{
    static const uint8_t bcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    static const uint8_t vlan[] = {0x81, 0x00};
    uint8_t *ptr = (uint8_t *)buf;
    int i;

    if (n->promisc)
        return 1;

    ptr += n->host_hdr_len;

    if (!memcmp(&ptr[12], vlan, sizeof(vlan))) {
        int vid = lduw_be_p(ptr + 14) & 0xfff;
        if (!(n->vlans[vid >> 5] & (1U << (vid & 0x1f))))
            return 0;
    }

    if (ptr[0] & 1) { // multicast
        if (!memcmp(ptr, bcast, sizeof(bcast))) {
            return !n->nobcast;
        } else if (n->nomulti) {
            return 0;
        } else if (n->allmulti || n->mac_table.multi_overflow) {
            return 1;
        }

        for (i = n->mac_table.first_multi; i < n->mac_table.in_use; i++) {
            if (!memcmp(ptr, &n->mac_table.macs[i * ETH_ALEN], ETH_ALEN)) {
                return 1;
            }
        }
    } else { // unicast
        if (n->nouni) {
            return 0;
        } else if (n->alluni || n->mac_table.uni_overflow) {
            return 1;
        } else if (!memcmp(ptr, n->mac, ETH_ALEN)) {
            return 1;
        }

        for (i = 0; i < n->mac_table.first_multi; i++) {
            if (!memcmp(ptr, &n->mac_table.macs[i * ETH_ALEN], ETH_ALEN)) {
                return 1;
            }
        }
    }

    return 0;
}