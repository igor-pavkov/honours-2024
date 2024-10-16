static void ssdpDiscover(int s, int ipv6, const char * search)
{
	static const char MSearchMsgFmt[] =
	"M-SEARCH * HTTP/1.1\r\n"
	"HOST: %s:" XSTR(PORT) "\r\n"
	"ST: %s\r\n"
	"MAN: \"ssdp:discover\"\r\n"
	"MX: %u\r\n"
	"\r\n";
	char bufr[512];
	int n;
	int mx = 3;
	int linklocal = 1;
	struct sockaddr_storage sockudp_w;

	{
		n = snprintf(bufr, sizeof(bufr),
		             MSearchMsgFmt,
		             ipv6 ?
		             (linklocal ? "[" UPNP_MCAST_LL_ADDR "]" :  "[" UPNP_MCAST_SL_ADDR "]")
		             : UPNP_MCAST_ADDR,
		             (search ? search : "ssdp:all"), mx);
		memset(&sockudp_w, 0, sizeof(struct sockaddr_storage));
		if(ipv6) {
			struct sockaddr_in6 * p = (struct sockaddr_in6 *)&sockudp_w;
			p->sin6_family = AF_INET6;
			p->sin6_port = htons(PORT);
			inet_pton(AF_INET6,
			          linklocal ? UPNP_MCAST_LL_ADDR : UPNP_MCAST_SL_ADDR,
			          &(p->sin6_addr));
		} else {
			struct sockaddr_in * p = (struct sockaddr_in *)&sockudp_w;
			p->sin_family = AF_INET;
			p->sin_port = htons(PORT);
			p->sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);
		}

		n = sendto_or_schedule(s, bufr, n, 0, (const struct sockaddr *)&sockudp_w,
		                       ipv6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in));
		if (n < 0) {
			syslog(LOG_ERR, "%s: sendto: %m", __func__);
		}
	}
}