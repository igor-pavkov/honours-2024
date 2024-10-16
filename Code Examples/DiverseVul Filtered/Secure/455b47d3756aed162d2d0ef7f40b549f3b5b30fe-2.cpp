void convert_address(struct sockaddr_storage *ss, char *buf, size_t len)
{
	switch (ss->ss_family) {
	case AF_INET:
		inet_ntop(ss->ss_family,
			  &((struct sockaddr_in *)ss)->sin_addr, buf, len);
		break;

	case AF_INET6:
		inet_ntop(ss->ss_family,
			  &((struct sockaddr_in6 *)ss)->sin6_addr, buf, len);
		break;
	}
}