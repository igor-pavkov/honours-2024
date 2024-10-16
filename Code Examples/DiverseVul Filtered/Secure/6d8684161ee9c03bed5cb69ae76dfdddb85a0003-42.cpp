static int WSAAPI getaddrinfo_stub(const char *node, const char *service,
				   const struct addrinfo *hints,
				   struct addrinfo **res)
{
	struct hostent *h = NULL;
	struct addrinfo *ai;
	struct sockaddr_in *sin;

	if (node) {
		h = gethostbyname(node);
		if (!h)
			return WSAGetLastError();
	}

	ai = xmalloc(sizeof(struct addrinfo));
	*res = ai;
	ai->ai_flags = 0;
	ai->ai_family = AF_INET;
	ai->ai_socktype = hints ? hints->ai_socktype : 0;
	switch (ai->ai_socktype) {
	case SOCK_STREAM:
		ai->ai_protocol = IPPROTO_TCP;
		break;
	case SOCK_DGRAM:
		ai->ai_protocol = IPPROTO_UDP;
		break;
	default:
		ai->ai_protocol = 0;
		break;
	}
	ai->ai_addrlen = sizeof(struct sockaddr_in);
	if (hints && (hints->ai_flags & AI_CANONNAME))
		ai->ai_canonname = h ? xstrdup(h->h_name) : NULL;
	else
		ai->ai_canonname = NULL;

	sin = xcalloc(1, ai->ai_addrlen);
	sin->sin_family = AF_INET;
	/* Note: getaddrinfo is supposed to allow service to be a string,
	 * which should be looked up using getservbyname. This is
	 * currently not implemented */
	if (service)
		sin->sin_port = htons(atoi(service));
	if (h)
		sin->sin_addr = *(struct in_addr *)h->h_addr;
	else if (hints && (hints->ai_flags & AI_PASSIVE))
		sin->sin_addr.s_addr = INADDR_ANY;
	else
		sin->sin_addr.s_addr = INADDR_LOOPBACK;
	ai->ai_addr = (struct sockaddr *)sin;
	ai->ai_next = NULL;
	return 0;
}