const char *csync_inet_ntop(address_t *addr)
{
	char buf[INET6_ADDRSTRLEN];
	sa_family_t af = addr->sa.sa_family;
	return inet_ntop(af,
		af == AF_INET  ? (void*)&addr->sa_in.sin_addr :
		af == AF_INET6 ? (void*)&addr->sa_in6.sin6_addr : NULL,
		buf, sizeof(buf));
}