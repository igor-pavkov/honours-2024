static void nft_setelem_activate(struct net *net, struct nft_set *set,
				 struct nft_set_elem *elem)
{
	struct nft_set_ext *ext = nft_set_elem_ext(set, elem->priv);

	if (nft_setelem_is_catchall(set, elem)) {
		nft_set_elem_change_active(net, set, ext);
		nft_set_elem_clear_busy(ext);
	} else {
		set->ops->activate(net, set, elem);
	}
}