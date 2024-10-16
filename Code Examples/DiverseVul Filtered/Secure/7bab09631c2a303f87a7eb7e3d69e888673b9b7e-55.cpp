static void xfrm_dst_hash_transfer(struct net *net,
				   struct hlist_head *list,
				   struct hlist_head *ndsttable,
				   unsigned int nhashmask,
				   int dir)
{
	struct hlist_node *tmp, *entry0 = NULL;
	struct xfrm_policy *pol;
	unsigned int h0 = 0;
	u8 dbits;
	u8 sbits;

redo:
	hlist_for_each_entry_safe(pol, tmp, list, bydst) {
		unsigned int h;

		__get_hash_thresh(net, pol->family, dir, &dbits, &sbits);
		h = __addr_hash(&pol->selector.daddr, &pol->selector.saddr,
				pol->family, nhashmask, dbits, sbits);
		if (!entry0) {
			hlist_del_rcu(&pol->bydst);
			hlist_add_head_rcu(&pol->bydst, ndsttable + h);
			h0 = h;
		} else {
			if (h != h0)
				continue;
			hlist_del_rcu(&pol->bydst);
			hlist_add_behind_rcu(&pol->bydst, entry0);
		}
		entry0 = &pol->bydst;
	}
	if (!hlist_empty(list)) {
		entry0 = NULL;
		goto redo;
	}
}