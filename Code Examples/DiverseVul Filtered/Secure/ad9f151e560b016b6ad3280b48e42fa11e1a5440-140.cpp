static bool nft_setelem_is_catchall(const struct nft_set *set,
				    const struct nft_set_elem *elem)
{
	struct nft_set_ext *ext = nft_set_elem_ext(set, elem->priv);

	if (nft_set_ext_exists(ext, NFT_SET_EXT_FLAGS) &&
	    *nft_set_ext_flags(ext) & NFT_SET_ELEM_CATCHALL)
		return true;

	return false;
}