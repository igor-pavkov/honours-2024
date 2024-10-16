void br_multicast_init(struct net_bridge *br)
{
	br->hash_elasticity = 4;
	br->hash_max = 512;

	br->multicast_router = 1;
	br->multicast_last_member_count = 2;
	br->multicast_startup_query_count = 2;

	br->multicast_last_member_interval = HZ;
	br->multicast_query_response_interval = 10 * HZ;
	br->multicast_startup_query_interval = 125 * HZ / 4;
	br->multicast_query_interval = 125 * HZ;
	br->multicast_querier_interval = 255 * HZ;
	br->multicast_membership_interval = 260 * HZ;

	spin_lock_init(&br->multicast_lock);
	setup_timer(&br->multicast_router_timer,
		    br_multicast_local_router_expired, 0);
	setup_timer(&br->multicast_querier_timer,
		    br_multicast_local_router_expired, 0);
	setup_timer(&br->multicast_query_timer, br_multicast_query_expired,
		    (unsigned long)br);
}