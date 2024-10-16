static inline int br_ip_hash(struct net_bridge_mdb_htable *mdb,
			     struct br_ip *ip)
{
	switch (ip->proto) {
	case htons(ETH_P_IP):
		return __br_ip4_hash(mdb, ip->u.ip4);
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	case htons(ETH_P_IPV6):
		return __br_ip6_hash(mdb, &ip->u.ip6);
#endif
	}
	return 0;
}