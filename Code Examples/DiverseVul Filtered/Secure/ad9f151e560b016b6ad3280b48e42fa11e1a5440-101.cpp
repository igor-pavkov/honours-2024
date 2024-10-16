static u32 nft_objname_hash(const void *data, u32 len, u32 seed)
{
	const struct nft_object_hash_key *k = data;

	seed ^= hash_ptr(k->table, 32);

	return jhash(k->name, strlen(k->name), seed);
}