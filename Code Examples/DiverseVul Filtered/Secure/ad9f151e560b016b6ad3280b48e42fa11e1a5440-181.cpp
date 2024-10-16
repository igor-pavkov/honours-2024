void *nft_set_catchall_gc(const struct nft_set *set)
{
	struct nft_set_elem_catchall *catchall, *next;
	struct nft_set_ext *ext;
	void *elem = NULL;

	list_for_each_entry_safe(catchall, next, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);

		if (!nft_set_elem_expired(ext) ||
		    nft_set_elem_mark_busy(ext))
			continue;

		elem = catchall->elem;
		list_del_rcu(&catchall->list);
		kfree_rcu(catchall, rcu);
		break;
	}

	return elem;
}