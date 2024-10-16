static inline bool ip6gre_tnl_addr_conflict(const struct ip6_tnl *t,
	const struct ipv6hdr *hdr)
{
	return ipv6_addr_equal(&t->parms.raddr, &hdr->saddr);
}