void hm_free(struct cli_matcher *root) {
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

	    mpool_free(root->mempool, szh->hash_array);
	    while(szh->items)
		mpool_free(root->mempool, (void *)szh->virusnames[--szh->items]);
	    mpool_free(root->mempool, szh->virusnames);
	    mpool_free(root->mempool, szh);
	}
	cli_htu32_free(ht, root->mempool);
    }
}