int mingw_setsockopt(int sockfd, int lvl, int optname, void *optval, int optlen)
{
	SOCKET s = (SOCKET)_get_osfhandle(sockfd);
	return setsockopt(s, lvl, optname, (const char*)optval, optlen);
}