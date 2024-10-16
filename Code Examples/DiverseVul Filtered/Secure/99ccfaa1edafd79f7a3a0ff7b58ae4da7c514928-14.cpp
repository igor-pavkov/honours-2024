static inline int ladr_match(PCNetState *s, const uint8_t *buf, int size)
{
    struct qemu_ether_header *hdr = (void *)buf;
    if ((*(hdr->ether_dhost)&0x01) &&
        ((uint64_t *)&s->csr[8])[0] != 0LL) {
        uint8_t ladr[8] = {
            s->csr[8] & 0xff, s->csr[8] >> 8,
            s->csr[9] & 0xff, s->csr[9] >> 8,
            s->csr[10] & 0xff, s->csr[10] >> 8,
            s->csr[11] & 0xff, s->csr[11] >> 8
        };
        int index = net_crc32_le(hdr->ether_dhost, ETH_ALEN) >> 26;
        return !!(ladr[index >> 3] & (1 << (index & 7)));
    }
    return 0;
}