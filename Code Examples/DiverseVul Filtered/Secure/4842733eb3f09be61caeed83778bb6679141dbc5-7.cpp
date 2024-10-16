static void hm_sort(struct cli_sz_hash *szh, size_t l, size_t r, unsigned int keylen) {
    uint8_t piv[32], tmph[32];
    size_t l1, r1;

    const char *tmpv;

    if(l + 1 >= r)
	return;

    l1 = l+1, r1 = r;

    memcpy(piv, &szh->hash_array[keylen * l], keylen);
    while(l1 < r1) {
	if(hm_cmp(&szh->hash_array[keylen * l1], piv, keylen) > 0) {
	    r1--;
	    if(l1 == r1) break;
	    memcpy(tmph, &szh->hash_array[keylen * l1], keylen);
	    tmpv = szh->virusnames[l1];
	    memcpy(&szh->hash_array[keylen * l1], &szh->hash_array[keylen * r1], keylen);
	    szh->virusnames[l1] = szh->virusnames[r1];
	    memcpy(&szh->hash_array[keylen * r1], tmph, keylen);
	    szh->virusnames[r1] = tmpv;
	} else
	    l1++;
    }

    l1--;
    if(l1!=l) {
	memcpy(tmph, &szh->hash_array[keylen * l1], keylen);
	tmpv = szh->virusnames[l1];
	memcpy(&szh->hash_array[keylen * l1], &szh->hash_array[keylen * l], keylen);
	szh->virusnames[l1] = szh->virusnames[l];
	memcpy(&szh->hash_array[keylen * l], tmph, keylen);
	szh->virusnames[l] = tmpv;
    }

    hm_sort(szh, l, l1, keylen);
    hm_sort(szh, r1, r, keylen);
}