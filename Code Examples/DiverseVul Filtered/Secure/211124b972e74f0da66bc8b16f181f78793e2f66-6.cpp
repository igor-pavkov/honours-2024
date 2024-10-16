static inline unsigned int dccp_basic_hdr_len(const struct dccp_hdr *dh)
{
	return DCCPH_X(dh) ? sizeof(struct dccp_hdr_ext) : sizeof(struct dccp_hdr);
}