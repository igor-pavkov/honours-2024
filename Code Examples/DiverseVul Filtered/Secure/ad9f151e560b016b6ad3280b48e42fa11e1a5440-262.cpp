static u32 nft_chain_hash_obj(const void *data, u32 len, u32 seed)
{
	const struct nft_chain *chain = data;

	return nft_chain_hash(chain->name, 0, seed);
}