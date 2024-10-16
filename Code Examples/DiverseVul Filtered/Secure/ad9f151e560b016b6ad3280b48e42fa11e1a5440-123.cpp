static u32 nft_chain_hash(const void *data, u32 len, u32 seed)
{
	const char *name = data;

	return jhash(name, strlen(name), seed);
}