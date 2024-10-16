static int nft_setelem_deactivate(const struct net *net,
				  struct nft_set *set,
				  struct nft_set_elem *elem, u32 flags)
{
	int ret;

	if (flags & NFT_SET_ELEM_CATCHALL)
		ret = nft_setelem_catchall_deactivate(net, set, elem);
	else
		ret = __nft_setelem_deactivate(net, set, elem);

	return ret;
}