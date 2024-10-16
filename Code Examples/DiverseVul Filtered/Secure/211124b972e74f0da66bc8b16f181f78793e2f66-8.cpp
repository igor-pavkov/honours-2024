static inline u_int dccp_csum_coverage(const struct dccp_hdr* dh, u_int len)
{
	u_int cov;

	if (DCCPH_CSCOV(dh) == 0)
		return len;
	cov = (dh->dccph_doff + DCCPH_CSCOV(dh) - 1) * sizeof(uint32_t);
	return (cov > len)? len : cov;
}