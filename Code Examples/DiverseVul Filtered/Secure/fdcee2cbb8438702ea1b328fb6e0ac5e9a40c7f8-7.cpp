static int sctp_v6_cmp_addr(const union sctp_addr *addr1,
			    const union sctp_addr *addr2)
{
	if (addr1->sa.sa_family != addr2->sa.sa_family) {
		if (addr1->sa.sa_family == AF_INET &&
		    addr2->sa.sa_family == AF_INET6 &&
		    ipv6_addr_v4mapped(&addr2->v6.sin6_addr)) {
			if (addr2->v6.sin6_port == addr1->v4.sin_port &&
			    addr2->v6.sin6_addr.s6_addr32[3] ==
			    addr1->v4.sin_addr.s_addr)
				return 1;
		}
		if (addr2->sa.sa_family == AF_INET &&
		    addr1->sa.sa_family == AF_INET6 &&
		    ipv6_addr_v4mapped(&addr1->v6.sin6_addr)) {
			if (addr1->v6.sin6_port == addr2->v4.sin_port &&
			    addr1->v6.sin6_addr.s6_addr32[3] ==
			    addr2->v4.sin_addr.s_addr)
				return 1;
		}
		return 0;
	}
	if (addr1->v6.sin6_port != addr2->v6.sin6_port)
		return 0;
	if (!ipv6_addr_equal(&addr1->v6.sin6_addr, &addr2->v6.sin6_addr))
		return 0;
	/* If this is a linklocal address, compare the scope_id. */
	if (ipv6_addr_type(&addr1->v6.sin6_addr) & IPV6_ADDR_LINKLOCAL) {
		if (addr1->v6.sin6_scope_id && addr2->v6.sin6_scope_id &&
		    (addr1->v6.sin6_scope_id != addr2->v6.sin6_scope_id)) {
			return 0;
		}
	}

	return 1;
}