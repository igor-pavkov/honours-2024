static void socket_cleanup(void)
{
	WSACleanup();
	if (ipv6_dll)
		FreeLibrary(ipv6_dll);
	ipv6_dll = NULL;
	ipv6_freeaddrinfo = freeaddrinfo_stub;
	ipv6_getaddrinfo = getaddrinfo_stub;
	ipv6_getnameinfo = getnameinfo_stub;
}