void hm_flush(struct cli_matcher *root) {
    enum CLI_HASH_TYPE type;

    if(!root)
	return;

    for(type = CLI_HASH_MD5; type < CLI_HASH_AVAIL_TYPES; type++) {
	struct cli_htu32 *ht = &root->hm.sizehashes[type];
	const struct cli_htu32_element *item = NULL;

	if(!root->hm.sizehashes[type].capacity)
	    continue;

	while((item = cli_htu32_next(ht, item))) {
	    struct cli_sz_hash *szh = (struct cli_sz_hash *)item->data.as_ptr;
	    unsigned int keylen = hashlen[type];

	    if(szh->items > 1)
		hm_sort(szh, 0, szh->items, keylen);
	}
    }
}