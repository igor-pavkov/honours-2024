static int dccp6_cksum(netdissect_options *ndo, const struct ip6_hdr *ip6,
	const struct dccp_hdr *dh, u_int len)
{
	return nextproto6_cksum(ndo, ip6, (const uint8_t *)(const void *)dh, len,
				dccp_csum_coverage(dh, len), IPPROTO_DCCP);
}