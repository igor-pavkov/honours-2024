static bool nft_set_ops_candidate(const struct nft_set_type *type, u32 flags)
{
	return (flags & type->features) == (flags & NFT_SET_FEATURES);
}