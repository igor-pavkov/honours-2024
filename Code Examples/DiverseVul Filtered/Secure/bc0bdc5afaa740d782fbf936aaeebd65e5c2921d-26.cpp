static inline u8 cma_get_ip_ver(const struct cma_hdr *hdr)
{
	return hdr->ip_version >> 4;
}