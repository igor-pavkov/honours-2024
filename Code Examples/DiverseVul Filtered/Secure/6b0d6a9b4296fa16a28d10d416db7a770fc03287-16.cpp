static void br_multicast_port_group_expired(unsigned long data)
{
	struct net_bridge_port_group *pg = (void *)data;
	struct net_bridge *br = pg->port->br;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || timer_pending(&pg->timer) ||
	    hlist_unhashed(&pg->mglist))
		goto out;

	br_multicast_del_pg(br, pg);

out:
	spin_unlock(&br->multicast_lock);
}