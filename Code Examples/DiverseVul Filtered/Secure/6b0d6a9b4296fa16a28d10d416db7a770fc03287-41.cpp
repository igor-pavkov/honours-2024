static void br_multicast_send_group_query(struct net_bridge_mdb_entry *mp)
{
	struct net_bridge *br = mp->br;
	struct sk_buff *skb;

	skb = br_multicast_alloc_query(br, &mp->addr);
	if (!skb)
		goto timer;

	netif_rx(skb);

timer:
	if (++mp->queries_sent < br->multicast_last_member_count)
		mod_timer(&mp->query_timer,
			  jiffies + br->multicast_last_member_interval);
}