static inline int br_ip_equal(const struct br_ip *a, const struct br_ip *b)
{
	if (a->proto != b->proto)
		return 0;
	switch (a->proto) {
	case htons(ETH_P_IP):
		return a->u.ip4 == b->u.ip4;
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	case htons(ETH_P_IPV6):
		return ipv6_addr_equal(&a->u.ip6, &b->u.ip6);
#endif
	}
	return 0;
}