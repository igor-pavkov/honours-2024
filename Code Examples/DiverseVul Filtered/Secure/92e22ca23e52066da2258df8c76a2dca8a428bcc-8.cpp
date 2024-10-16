int setup_allrouters_membership(struct Interface *iface)
{
	struct ipv6_mreq mreq;

	memset(&mreq, 0, sizeof(mreq));
	mreq.ipv6mr_interface = iface->if_index;

	/* ipv6-allrouters: ff02::2 */
	mreq.ipv6mr_multiaddr.s6_addr32[0] = htonl(0xFF020000);
	mreq.ipv6mr_multiaddr.s6_addr32[3] = htonl(0x2);

	if (setsockopt(sock, SOL_IPV6, IPV6_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		/* linux-2.6.12-bk4 returns error with HUP signal but keep listening */
		if (errno != EADDRINUSE)
		{
			flog(LOG_ERR, "can't join ipv6-allrouters on %s", iface->Name);
			return (-1);
		}
	}

	return (0);
}