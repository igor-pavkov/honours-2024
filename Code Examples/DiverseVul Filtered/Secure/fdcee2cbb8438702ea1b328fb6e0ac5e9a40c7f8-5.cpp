static void sctp_inet6_event_msgname(struct sctp_ulpevent *event,
				     char *msgname, int *addrlen)
{
	union sctp_addr *addr;
	struct sctp_association *asoc;
	union sctp_addr *paddr;

	if (!msgname)
		return;

	addr = (union sctp_addr *)msgname;
	asoc = event->asoc;
	paddr = &asoc->peer.primary_addr;

	if (paddr->sa.sa_family == AF_INET) {
		addr->v4.sin_family = AF_INET;
		addr->v4.sin_port = htons(asoc->peer.port);
		addr->v4.sin_addr = paddr->v4.sin_addr;
	} else {
		addr->v6.sin6_family = AF_INET6;
		addr->v6.sin6_flowinfo = 0;
		if (ipv6_addr_type(&paddr->v6.sin6_addr) & IPV6_ADDR_LINKLOCAL)
			addr->v6.sin6_scope_id = paddr->v6.sin6_scope_id;
		else
			addr->v6.sin6_scope_id = 0;
		addr->v6.sin6_port = htons(asoc->peer.port);
		addr->v6.sin6_addr = paddr->v6.sin6_addr;
	}

	*addrlen = sctp_v6_addr_to_user(sctp_sk(asoc->base.sk), addr);
}