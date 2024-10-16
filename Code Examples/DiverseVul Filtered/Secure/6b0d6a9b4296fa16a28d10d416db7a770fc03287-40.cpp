static void br_multicast_group_expired(unsigned long data)
{
	struct net_bridge_mdb_entry *mp = (void *)data;
	struct net_bridge *br = mp->br;
	struct net_bridge_mdb_htable *mdb;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || timer_pending(&mp->timer))
		goto out;

	if (!hlist_unhashed(&mp->mglist))
		hlist_del_init(&mp->mglist);

	if (mp->ports)
		goto out;

	mdb = mlock_dereference(br->mdb, br);

	hlist_del_rcu(&mp->hlist[mdb->ver]);
	mdb->size--;

	del_timer(&mp->query_timer);
	call_rcu_bh(&mp->rcu, br_multicast_free_group);

out:
	spin_unlock(&br->multicast_lock);
}