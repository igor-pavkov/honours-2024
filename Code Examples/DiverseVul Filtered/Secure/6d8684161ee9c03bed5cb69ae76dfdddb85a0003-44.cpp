static void ensure_socket_initialization(void)
{
	WSADATA wsa;
	static int initialized = 0;
	const char *libraries[] = { "ws2_32.dll", "wship6.dll", NULL };
	const char **name;

	if (initialized)
		return;

	if (WSAStartup(MAKEWORD(2,2), &wsa))
		die("unable to initialize winsock subsystem, error %d",
			WSAGetLastError());

	for (name = libraries; *name; name++) {
		ipv6_dll = LoadLibrary(*name);
		if (!ipv6_dll)
			continue;

		ipv6_freeaddrinfo = (void (WSAAPI *)(struct addrinfo *))
			GetProcAddress(ipv6_dll, "freeaddrinfo");
		ipv6_getaddrinfo = (int (WSAAPI *)(const char *, const char *,
						   const struct addrinfo *,
						   struct addrinfo **))
			GetProcAddress(ipv6_dll, "getaddrinfo");
		ipv6_getnameinfo = (int (WSAAPI *)(const struct sockaddr *,
						   socklen_t, char *, DWORD,
						   char *, DWORD, int))
			GetProcAddress(ipv6_dll, "getnameinfo");
		if (!ipv6_freeaddrinfo || !ipv6_getaddrinfo || !ipv6_getnameinfo) {
			FreeLibrary(ipv6_dll);
			ipv6_dll = NULL;
		} else
			break;
	}
	if (!ipv6_freeaddrinfo || !ipv6_getaddrinfo || !ipv6_getnameinfo) {
		ipv6_freeaddrinfo = freeaddrinfo_stub;
		ipv6_getaddrinfo = getaddrinfo_stub;
		ipv6_getnameinfo = getnameinfo_stub;
	}

	atexit(socket_cleanup);
	initialized = 1;
}