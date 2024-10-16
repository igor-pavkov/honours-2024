static int nft_objname_hash_cmp(struct rhashtable_compare_arg *arg,
				const void *ptr)
{
	const struct nft_object_hash_key *k = arg->key;
	const struct nft_object *obj = ptr;

	if (obj->key.table != k->table)
		return -1;

	return strcmp(obj->key.name, k->name);
}