static bool validate_net_dev(struct net_device *net_dev,
			     const struct sockaddr *daddr,
			     const struct sockaddr *saddr)
{
	const struct sockaddr_in *daddr4 = (const struct sockaddr_in *)daddr;
	const struct sockaddr_in *saddr4 = (const struct sockaddr_in *)saddr;
	const struct sockaddr_in6 *daddr6 = (const struct sockaddr_in6 *)daddr;
	const struct sockaddr_in6 *saddr6 = (const struct sockaddr_in6 *)saddr;

	switch (daddr->sa_family) {
	case AF_INET:
		return saddr->sa_family == AF_INET &&
		       validate_ipv4_net_dev(net_dev, daddr4, saddr4);

	case AF_INET6:
		return saddr->sa_family == AF_INET6 &&
		       validate_ipv6_net_dev(net_dev, daddr6, saddr6);

	default:
		return false;
	}
}