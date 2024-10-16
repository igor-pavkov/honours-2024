static void br_ip6_multicast_leave_group(struct net_bridge *br,
					 struct net_bridge_port *port,
					 const struct in6_addr *group)
{
	struct br_ip br_group;

	if (ipv6_is_local_multicast(group))
		return;

	ipv6_addr_copy(&br_group.u.ip6, group);
	br_group.proto = htons(ETH_P_IPV6);

	br_multicast_leave_group(br, port, &br_group);
}