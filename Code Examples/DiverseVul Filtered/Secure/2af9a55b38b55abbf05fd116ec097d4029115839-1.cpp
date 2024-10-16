static bool ndp_msg_addrto_validate_link_local(struct in6_addr *addr)
{
	return IN6_IS_ADDR_LINKLOCAL (addr);
}