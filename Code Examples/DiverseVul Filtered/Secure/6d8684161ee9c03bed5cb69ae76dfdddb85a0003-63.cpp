static int WSAAPI getnameinfo_stub(const struct sockaddr *sa, socklen_t salen,
				   char *host, DWORD hostlen,
				   char *serv, DWORD servlen, int flags)
{
	const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;
	if (sa->sa_family != AF_INET)
		return EAI_FAMILY;
	if (!host && !serv)
		return EAI_NONAME;

	if (host && hostlen > 0) {
		struct hostent *ent = NULL;
		if (!(flags & NI_NUMERICHOST))
			ent = gethostbyaddr((const char *)&sin->sin_addr,
					    sizeof(sin->sin_addr), AF_INET);

		if (ent)
			snprintf(host, hostlen, "%s", ent->h_name);
		else if (flags & NI_NAMEREQD)
			return EAI_NONAME;
		else
			snprintf(host, hostlen, "%s", inet_ntoa(sin->sin_addr));
	}

	if (serv && servlen > 0) {
		struct servent *ent = NULL;
		if (!(flags & NI_NUMERICSERV))
			ent = getservbyport(sin->sin_port,
					    flags & NI_DGRAM ? "udp" : "tcp");

		if (ent)
			snprintf(serv, servlen, "%s", ent->s_name);
		else
			snprintf(serv, servlen, "%d", ntohs(sin->sin_port));
	}

	return 0;
}