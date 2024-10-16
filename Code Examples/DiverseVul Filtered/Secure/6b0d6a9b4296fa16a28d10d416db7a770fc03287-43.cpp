static inline int ipv6_is_local_multicast(const struct in6_addr *addr)
{
	if (ipv6_addr_is_multicast(addr) &&
	    IPV6_ADDR_MC_SCOPE(addr) <= IPV6_ADDR_SCOPE_LINKLOCAL)
		return 1;
	return 0;
}