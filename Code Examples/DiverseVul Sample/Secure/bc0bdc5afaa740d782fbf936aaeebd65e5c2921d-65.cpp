static inline int cma_any_port(const struct sockaddr *addr)
{
	return !cma_port(addr);
}