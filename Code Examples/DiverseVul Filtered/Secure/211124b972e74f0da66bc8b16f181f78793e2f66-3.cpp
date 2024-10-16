static int dccp_cksum(netdissect_options *ndo, const struct ip *ip,
	const struct dccp_hdr *dh, u_int len)
{
	return nextproto4_cksum(ndo, ip, (const uint8_t *)(const void *)dh, len,
				dccp_csum_coverage(dh, len), IPPROTO_DCCP);
}