static int __nft_setelem_deactivate(const struct net *net,
				    struct nft_set *set,
				    struct nft_set_elem *elem)
{
	void *priv;

	priv = set->ops->deactivate(net, set, elem);
	if (!priv)
		return -ENOENT;

	kfree(elem->priv);
	elem->priv = priv;
	set->ndeact++;

	return 0;
}