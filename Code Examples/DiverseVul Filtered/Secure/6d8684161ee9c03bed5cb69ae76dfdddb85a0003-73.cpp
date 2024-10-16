struct hostent *mingw_gethostbyname(const char *host)
{
	ensure_socket_initialization();
	return gethostbyname(host);
}