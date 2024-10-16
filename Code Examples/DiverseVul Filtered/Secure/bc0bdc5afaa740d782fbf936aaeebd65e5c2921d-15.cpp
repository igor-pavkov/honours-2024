static bool validate_ipv6_net_dev(struct net_device *net_dev,
				  const struct sockaddr_in6 *dst_addr,
				  const struct sockaddr_in6 *src_addr)
{
#if IS_ENABLED(CONFIG_IPV6)
	const int strict = ipv6_addr_type(&dst_addr->sin6_addr) &
			   IPV6_ADDR_LINKLOCAL;
	struct rt6_info *rt = rt6_lookup(dev_net(net_dev), &dst_addr->sin6_addr,
					 &src_addr->sin6_addr, net_dev->ifindex,
					 NULL, strict);
	bool ret;

	if (!rt)
		return false;

	ret = rt->rt6i_idev->dev == net_dev;
	ip6_rt_put(rt);

	return ret;
#else
	return false;
#endif
}