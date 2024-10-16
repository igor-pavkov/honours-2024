static inline __be32 try_6to4(struct in6_addr *v6dst)
{
	__be32 dst = 0;

	if (v6dst->s6_addr16[0] == htons(0x2002)) {
		/* 6to4 v6 addr has 16 bits prefix, 32 v4addr, 16 SLA, ... */
		memcpy(&dst, &v6dst->s6_addr16[1], 4);
	}
	return dst;
}