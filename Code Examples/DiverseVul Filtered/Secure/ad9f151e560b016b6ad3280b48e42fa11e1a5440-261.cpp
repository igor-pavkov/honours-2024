struct nft_set_ext *nft_set_catchall_lookup(const struct net *net,
					    const struct nft_set *set)
{
	struct nft_set_elem_catchall *catchall;
	u8 genmask = nft_genmask_cur(net);
	struct nft_set_ext *ext;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (nft_set_elem_active(ext, genmask) &&
		    !nft_set_elem_expired(ext))
			return ext;
	}

	return NULL;
}