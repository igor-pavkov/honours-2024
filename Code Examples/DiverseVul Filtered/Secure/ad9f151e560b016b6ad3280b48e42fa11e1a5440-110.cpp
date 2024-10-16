static void *nft_setelem_catchall_get(const struct net *net,
				      const struct nft_set *set)
{
	struct nft_set_elem_catchall *catchall;
	u8 genmask = nft_genmask_cur(net);
	struct nft_set_ext *ext;
	void *priv = NULL;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (!nft_set_elem_active(ext, genmask) ||
		    nft_set_elem_expired(ext))
			continue;

		priv = catchall->elem;
		break;
	}

	return priv;
}