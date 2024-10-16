int OpenAndConfPCPv6Socket(void)
{
	int s;
	int i = 1;
	struct sockaddr_in6 addr;
	s = socket(PF_INET6, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if(s < 0) {
		syslog(LOG_ERR, "%s: socket(): %m", "OpenAndConfPCPv6Socket");
		return -1;
	}
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
		syslog(LOG_WARNING, "%s: setsockopt(SO_REUSEADDR): %m",
		       "OpenAndConfPCPv6Socket");
	}
#ifdef IPV6_V6ONLY
	/* force IPV6 only for IPV6 socket.
	 * see http://www.ietf.org/rfc/rfc3493.txt section 5.3 */
	if(setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &i, sizeof(i)) < 0) {
		syslog(LOG_WARNING, "%s: setsockopt(IPV6_V6ONLY): %m",
		       "OpenAndConfPCPv6Socket");
	}
#endif
#ifdef IPV6_RECVPKTINFO
	/* see RFC3542 */
	if(setsockopt(s, IPPROTO_IPV6, IPV6_RECVPKTINFO, &i, sizeof(i)) < 0) {
		syslog(LOG_WARNING, "%s: setsockopt(IPV6_RECVPKTINFO): %m",
		       "OpenAndConfPCPv6Socket");
	}
#endif
	if(!set_non_blocking(s)) {
		syslog(LOG_WARNING, "%s: set_non_blocking(): %m",
		       "OpenAndConfPCPv6Socket");
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(NATPMP_PORT);
	addr.sin6_addr = ipv6_bind_addr;
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		syslog(LOG_ERR, "%s: bind(): %m", "OpenAndConfPCPv6Socket");
		close(s);
		return -1;
	}
	return s;
}