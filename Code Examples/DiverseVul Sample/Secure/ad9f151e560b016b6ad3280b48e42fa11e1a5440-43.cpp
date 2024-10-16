static int nft_setelem_catchall_insert(const struct net *net,
				       struct nft_set *set,
				       const struct nft_set_elem *elem,
				       struct nft_set_ext **pext)
{
	struct nft_set_elem_catchall *catchall;
	u8 genmask = nft_genmask_next(net);
	struct nft_set_ext *ext;

	list_for_each_entry(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (nft_set_elem_active(ext, genmask)) {
			*pext = ext;
			return -EEXIST;
		}
	}

	catchall = kmalloc(sizeof(*catchall), GFP_KERNEL);
	if (!catchall)
		return -ENOMEM;

	catchall->elem = elem->priv;
	list_add_tail_rcu(&catchall->list, &set->catchall_list);

	return 0;
}