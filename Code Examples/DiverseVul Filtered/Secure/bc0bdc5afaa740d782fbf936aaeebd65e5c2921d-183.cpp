static inline bool cma_any_addr(const struct sockaddr *addr)
{
	return cma_zero_addr(addr) || cma_loopback_addr(addr);
}