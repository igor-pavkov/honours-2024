static inline u8 llc_ui_addr_null(struct sockaddr_llc *addr)
{
	return !memcmp(addr, &llc_ui_addrnull, sizeof(*addr));
}