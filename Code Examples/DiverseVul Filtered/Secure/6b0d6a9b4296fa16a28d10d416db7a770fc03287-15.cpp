static int br_mdb_copy(struct net_bridge_mdb_htable *new,
		       struct net_bridge_mdb_htable *old,
		       int elasticity)
{
	struct net_bridge_mdb_entry *mp;
	struct hlist_node *p;
	int maxlen;
	int len;
	int i;

	for (i = 0; i < old->max; i++)
		hlist_for_each_entry(mp, p, &old->mhash[i], hlist[old->ver])
			hlist_add_head(&mp->hlist[new->ver],
				       &new->mhash[br_ip_hash(new, &mp->addr)]);

	if (!elasticity)
		return 0;

	maxlen = 0;
	for (i = 0; i < new->max; i++) {
		len = 0;
		hlist_for_each_entry(mp, p, &new->mhash[i], hlist[new->ver])
			len++;
		if (len > maxlen)
			maxlen = len;
	}

	return maxlen > elasticity ? -EINVAL : 0;
}