static int connecthostport(const char * host, unsigned short port, char * result)
{
	int s, n;
	char hostname[INET6_ADDRSTRLEN];
	char port_str[8], ifname[8], tmp[4];
	struct addrinfo *ai, *p;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	/* hints.ai_flags = AI_ADDRCONFIG; */
#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#endif
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC; /* AF_INET, AF_INET6 or AF_UNSPEC */
	/* hints.ai_protocol = IPPROTO_TCP; */
	snprintf(port_str, sizeof(port_str), "%hu", port);
	strcpy(hostname, host);
	if(!strncmp(host, "fe80", 4))
	{
		printf("Using an linklocal address\n");
		strcpy(ifname, "%");
		snprintf(tmp, sizeof(tmp), "%d", linklocal_index);
		strcat(ifname, tmp);
		strcat(hostname, ifname);
		printf("host: %s\n", hostname);
	}
	n = getaddrinfo(hostname, port_str, &hints, &ai);
	if(n != 0)
	{
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(n));
		return -1;
	}
	s = -1;
	for(p = ai; p; p = p->ai_next)
	{
#ifdef DEBUG
		char tmp_host[256];
		char tmp_service[256];
		printf("ai_family=%d ai_socktype=%d ai_protocol=%d ai_addrlen=%d\n ",
		       p->ai_family, p->ai_socktype, p->ai_protocol, p->ai_addrlen);
		getnameinfo(p->ai_addr, p->ai_addrlen, tmp_host, sizeof(tmp_host),
		            tmp_service, sizeof(tmp_service),
		            NI_NUMERICHOST | NI_NUMERICSERV);
		printf(" host=%s service=%s\n", tmp_host, tmp_service);
#endif
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr), result, INET6_ADDRSTRLEN);
		return 0;
	}
	freeaddrinfo(ai);
}