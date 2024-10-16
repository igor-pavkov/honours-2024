void mingw_freeaddrinfo(struct addrinfo *res)
{
	ipv6_freeaddrinfo(res);
}