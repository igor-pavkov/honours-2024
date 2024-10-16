static size_t ip6gre_get_size(const struct net_device *dev)
{
	return
		/* IFLA_GRE_LINK */
		nla_total_size(4) +
		/* IFLA_GRE_IFLAGS */
		nla_total_size(2) +
		/* IFLA_GRE_OFLAGS */
		nla_total_size(2) +
		/* IFLA_GRE_IKEY */
		nla_total_size(4) +
		/* IFLA_GRE_OKEY */
		nla_total_size(4) +
		/* IFLA_GRE_LOCAL */
		nla_total_size(sizeof(struct in6_addr)) +
		/* IFLA_GRE_REMOTE */
		nla_total_size(sizeof(struct in6_addr)) +
		/* IFLA_GRE_TTL */
		nla_total_size(1) +
		/* IFLA_GRE_ENCAP_LIMIT */
		nla_total_size(1) +
		/* IFLA_GRE_FLOWINFO */
		nla_total_size(4) +
		/* IFLA_GRE_FLAGS */
		nla_total_size(4) +
		/* IFLA_GRE_ENCAP_TYPE */
		nla_total_size(2) +
		/* IFLA_GRE_ENCAP_FLAGS */
		nla_total_size(2) +
		/* IFLA_GRE_ENCAP_SPORT */
		nla_total_size(2) +
		/* IFLA_GRE_ENCAP_DPORT */
		nla_total_size(2) +
		0;
}