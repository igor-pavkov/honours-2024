static u32 HASH_ADDR(const struct in6_addr *addr)
{
	u32 hash = ipv6_addr_hash(addr);

	return hash_32(hash, IP6_GRE_HASH_SIZE_SHIFT);
}