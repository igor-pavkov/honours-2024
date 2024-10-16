static void br_multicast_send_query(struct net_bridge *br,
				    struct net_bridge_port *port, u32 sent)
{
	unsigned long time;
	struct br_ip br_group;

	if (!netif_running(br->dev) || br->multicast_disabled ||
	    timer_pending(&br->multicast_querier_timer))
		return;

	memset(&br_group.u, 0, sizeof(br_group.u));

	br_group.proto = htons(ETH_P_IP);
	__br_multicast_send_query(br, port, &br_group);

#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	br_group.proto = htons(ETH_P_IPV6);
	__br_multicast_send_query(br, port, &br_group);
#endif

	time = jiffies;
	time += sent < br->multicast_startup_query_count ?
		br->multicast_startup_query_interval :
		br->multicast_query_interval;
	mod_timer(port ? &port->multicast_query_timer :
			 &br->multicast_query_timer, time);
}