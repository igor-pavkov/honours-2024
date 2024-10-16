static int br_ip4_multicast_add_group(struct net_bridge *br,
				      struct net_bridge_port *port,
				      __be32 group)
{
	struct br_ip br_group;

	if (ipv4_is_local_multicast(group))
		return 0;

	br_group.u.ip4 = group;
	br_group.proto = htons(ETH_P_IP);

	return br_multicast_add_group(br, port, &br_group);
}