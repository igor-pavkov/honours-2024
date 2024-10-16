static void addr_hash_set(u32 *mask, const u8 *addr)
{
	int n = ether_crc(ETH_ALEN, addr) >> 26;
	mask[n >> 5] |= (1 << (n & 31));
}