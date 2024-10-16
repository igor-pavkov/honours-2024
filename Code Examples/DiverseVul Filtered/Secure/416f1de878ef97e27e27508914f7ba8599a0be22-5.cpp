int verify_peername(const char *name, address_t *peeraddr)
{
	sa_family_t af = peeraddr->sa.sa_family;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int try_mapped_ipv4;
	int s;

	/* Obtain address(es) matching host */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */

	s = getaddrinfo(name, NULL, &hints, &result);
	if (s != 0) {
		csync_debug(1, "getaddrinfo: %s\n", gai_strerror(s));
		return 0;
	}

	try_mapped_ipv4 =
		af == AF_INET6 &&
		!memcmp(&peeraddr->sa_in6.sin6_addr,
			"\0\0\0\0" "\0\0\0\0" "\0\0\xff\xff", 12);

	/* getaddrinfo() returns a list of address structures.
	 * Try each address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		/* both IPv4 */
		if (af == AF_INET && rp->ai_family == AF_INET &&
		    !memcmp(&((struct sockaddr_in*)rp->ai_addr)->sin_addr,
			    &peeraddr->sa_in.sin_addr, sizeof(struct in_addr)))
			break;
		/* both IPv6 */
		if (af == AF_INET6 && rp->ai_family == AF_INET6 &&
		    !memcmp(&((struct sockaddr_in6*)rp->ai_addr)->sin6_addr,
			    &peeraddr->sa_in6.sin6_addr, sizeof(struct in6_addr)))
			break;
		/* peeraddr IPv6, but actually ::ffff:I.P.v.4,
		 * and forward lookup returned IPv4 only */
		if (af == AF_INET6 && rp->ai_family == AF_INET &&
		    try_mapped_ipv4 &&
		    !memcmp(&((struct sockaddr_in*)rp->ai_addr)->sin_addr,
			    (unsigned char*)&peeraddr->sa_in6.sin6_addr + 12,
			    sizeof(struct in_addr)))
			break;
	}
	freeaddrinfo(result);
	if (rp != NULL) /* memcmp found a match */
		return conn_check_peer_cert(name, 0);
	return 0;
}