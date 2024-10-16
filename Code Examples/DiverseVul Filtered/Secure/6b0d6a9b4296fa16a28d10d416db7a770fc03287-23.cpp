static inline int __br_ip4_hash(struct net_bridge_mdb_htable *mdb, __be32 ip)
{
	return jhash_1word(mdb->secret, (__force u32)ip) & (mdb->max - 1);
}