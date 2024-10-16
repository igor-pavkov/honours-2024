int ppp_interface_is_up(struct tunnel *tunnel)
{
	struct ifaddrs *ifap, *ifa;

	log_debug("Got Address: %s\n", inet_ntoa(tunnel->ipv4.ip_addr));

	if (getifaddrs(&ifap)) {
		log_error("getifaddrs: %s\n", strerror(errno));
		return 0;
	}

	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		if ((
#if HAVE_USR_SBIN_PPPD
		            (tunnel->config->pppd_ifname
		             && strstr(ifa->ifa_name, tunnel->config->pppd_ifname)
		             != NULL)
		            || strstr(ifa->ifa_name, "ppp") != NULL
#endif
#if HAVE_USR_SBIN_PPP
		            strstr(ifa->ifa_name, "tun") != NULL
#endif
		    ) && ifa->ifa_flags & IFF_UP) {
			if (&(ifa->ifa_addr->sa_family) != NULL
			    && ifa->ifa_addr->sa_family == AF_INET) {
				struct in_addr if_ip_addr =
				        cast_addr(ifa->ifa_addr)->sin_addr;

				log_debug("Interface Name: %s\n", ifa->ifa_name);
				log_debug("Interface Addr: %s\n", inet_ntoa(if_ip_addr));

				if (tunnel->ipv4.ip_addr.s_addr == if_ip_addr.s_addr) {
					strncpy(tunnel->ppp_iface, ifa->ifa_name,
					        ROUTE_IFACE_LEN - 1);
					freeifaddrs(ifap);
					return 1;
				}
			}
		}
	}
	freeifaddrs(ifap);

	return 0;
}