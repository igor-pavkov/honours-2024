int hm_addhash(struct cli_matcher *root, const char *hash, uint32_t size, const char *virusname) {
    const struct cli_htu32_element *item;
    struct cli_sz_hash *szh;
    struct cli_htu32 *ht;
    enum CLI_HASH_TYPE type;
    uint8_t binhash[32];
    int hashlen, i;

    if(!root || !hash) {
	cli_errmsg("hm_addhash: NULL root or hash\n");
	return CL_ENULLARG;
    }

    if(!size || size == (uint32_t)-1) {
	cli_errmsg("hm_addhash: null or invalid size (%u)\n", size);
	return CL_EARG;
    }

    hashlen = strlen(hash);
    switch(hashlen) {
    case 32:
	type = CLI_HASH_MD5;
	break;
    case 40:
	type = CLI_HASH_SHA1;
	break;
    case 64:
	type = CLI_HASH_SHA256;
	break;
    default:
	cli_errmsg("hm_addhash: invalid hash %s -- FIXME!\n", hash);
	return CL_EARG;
    }
    if(cli_hex2str_to(hash, (char *)binhash, hashlen)) {
	cli_errmsg("hm_addhash: invalid hash %s\n", hash);
	return CL_EARG;
    }

    hashlen /= 2;
    ht = &root->hm.sizehashes[type];
    if(!root->hm.sizehashes[type].capacity) {
	i = cli_htu32_init(ht, 64, root->mempool);
	if(i) return i;
    }

    item = cli_htu32_find(ht, size);
    if(!item) {
	struct cli_htu32_element htitem;
	szh = mpool_calloc(root->mempool, 1, sizeof(*szh));
	if(!szh) {
	    cli_errmsg("hm_addhash: failed to allocate size hash\n");
	    return CL_EMEM;
	}

	htitem.key = size;
	htitem.data.as_ptr = szh;
	i = cli_htu32_insert(ht, &htitem, root->mempool);
	if(i) {
	    cli_errmsg("ht_addhash: failed to add item to hashtab");
	    mpool_free(root->mempool, szh);
	    return i;
	}
    } else
	szh = (struct cli_sz_hash *)item->data.as_ptr;

    szh->items++;

    szh->hash_array = mpool_realloc2(root->mempool, szh->hash_array, hashlen * szh->items);
    if(!szh->hash_array) {
	cli_errmsg("ht_add: failed to grow hash array to %u entries\n", szh->items);
	szh->items=0;
	mpool_free(root->mempool, szh->virusnames);
	szh->virusnames = NULL;
	return CL_EMEM;
    }

    szh->virusnames = mpool_realloc2(root->mempool, szh->virusnames, sizeof(*szh->virusnames) * szh->items);
    if(!szh->virusnames) {
	cli_errmsg("ht_add: failed to grow virusname array to %u entries\n", szh->items);
	szh->items=0;
	mpool_free(root->mempool, szh->hash_array);
	szh->hash_array = NULL;
	return CL_EMEM;
    }

    memcpy(&szh->hash_array[(szh->items-1) * hashlen], binhash, hashlen);
    szh->virusnames[(szh->items-1)] = virusname;
    
    return 0;
}