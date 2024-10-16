static inline int hm_cmp(const uint8_t *itm, const uint8_t *ref, unsigned int keylen) {
    uint32_t i = *(uint32_t *)itm, r = *(uint32_t *)ref;
    if(i!=r)
	return (i<r) * 2 -1;
    return memcmp(&itm[4], &ref[4], keylen - 4);
}