static inline uint16_t dns_rnd16(void)
{
	if (!dns_query_id_seed)
		dns_query_id_seed = now_ms;
	dns_query_id_seed ^= dns_query_id_seed << 13;
	dns_query_id_seed ^= dns_query_id_seed >> 7;
	dns_query_id_seed ^= dns_query_id_seed << 17;
	return dns_query_id_seed;
}