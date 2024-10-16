static u32 nft_objname_hash_obj(const void *data, u32 len, u32 seed)
{
	const struct nft_object *obj = data;

	return nft_objname_hash(&obj->key, 0, seed);
}