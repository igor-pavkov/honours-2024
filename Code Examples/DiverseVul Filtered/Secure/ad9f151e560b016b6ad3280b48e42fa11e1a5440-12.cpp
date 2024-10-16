static int nft_set_catchall_dump(struct net *net, struct sk_buff *skb,
				 const struct nft_set *set)
{
	struct nft_set_elem_catchall *catchall;
	u8 genmask = nft_genmask_cur(net);
	struct nft_set_elem elem;
	struct nft_set_ext *ext;
	int ret = 0;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (!nft_set_elem_active(ext, genmask) ||
		    nft_set_elem_expired(ext))
			continue;

		elem.priv = catchall->elem;
		ret = nf_tables_fill_setelem(skb, set, &elem);
		break;
	}

	return ret;
}