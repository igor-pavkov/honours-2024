static int nft_setelem_insert(const struct net *net,
			      struct nft_set *set,
			      const struct nft_set_elem *elem,
			      struct nft_set_ext **ext, unsigned int flags)
{
	int ret;

	if (flags & NFT_SET_ELEM_CATCHALL)
		ret = nft_setelem_catchall_insert(net, set, elem, ext);
	else
		ret = set->ops->insert(net, set, elem, ext);

	return ret;
}