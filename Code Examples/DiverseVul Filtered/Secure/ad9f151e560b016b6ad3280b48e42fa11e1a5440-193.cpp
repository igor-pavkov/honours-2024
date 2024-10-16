static int nft_chain_hash_cmp(struct rhashtable_compare_arg *arg,
			      const void *ptr)
{
	const struct nft_chain *chain = ptr;
	const char *name = arg->key;

	return strcmp(chain->name, name);
}