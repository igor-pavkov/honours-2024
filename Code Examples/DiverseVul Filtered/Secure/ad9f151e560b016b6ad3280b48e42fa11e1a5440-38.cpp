static void nft_setelem_remove(const struct net *net,
			       const struct nft_set *set,
			       const struct nft_set_elem *elem)
{
	if (nft_setelem_is_catchall(set, elem))
		nft_setelem_catchall_remove(net, set, elem);
	else
		set->ops->remove(net, set, elem);
}