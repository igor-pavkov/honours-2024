static void br_multicast_add_router(struct net_bridge *br,
				    struct net_bridge_port *port)
{
	struct net_bridge_port *p;
	struct hlist_node *n, *slot = NULL;

	hlist_for_each_entry(p, n, &br->router_list, rlist) {
		if ((unsigned long) port >= (unsigned long) p)
			break;
		slot = n;
	}

	if (slot)
		hlist_add_after_rcu(slot, &port->rlist);
	else
		hlist_add_head_rcu(&port->rlist, &br->router_list);
}