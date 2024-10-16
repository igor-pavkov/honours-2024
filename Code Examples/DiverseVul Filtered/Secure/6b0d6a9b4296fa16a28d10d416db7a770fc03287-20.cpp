static inline int __br_ip6_hash(struct net_bridge_mdb_htable *mdb,
				const struct in6_addr *ip)
{
	return jhash2((__force u32 *)ip->s6_addr32, 4, mdb->secret) & (mdb->max - 1);
}