void PCPSendUnsolicitedAnnounce(int * sockets, int n_sockets)
#endif
{
	int i;
	unsigned char buff[PCP_MIN_LEN];
	pcp_info_t info;
	ssize_t len;
	struct sockaddr_in addr;
#ifdef ENABLE_IPV6
	struct sockaddr_in6 addr6;
#endif /* ENABLE_IPV6 */
	/* this is an Unsolicited ANNOUNCE response */

	info.version = this_server_info.server_version;
	info.opcode = PCP_OPCODE_ANNOUNCE;
	info.result_code = PCP_SUCCESS;
	info.lifetime = 0;
	createPCPResponse(buff, &info);
	/* Multicast PCP restart announcements are sent to
	 * 224.0.0.1:5350 and/or [ff02::1]:5350 */
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("224.0.0.1");
	addr.sin_port = htons(5350);
	for(i = 0; i < n_sockets; i++) {
		len = sendto_or_schedule(sockets[i], buff, PCP_MIN_LEN, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
		if( len < 0 ) {
			syslog(LOG_ERR, "PCPSendUnsolicitedAnnounce() sendto(): %m");
		}
	}
#ifdef ENABLE_IPV6
	memset(&addr6, 0, sizeof(struct sockaddr_in6));
	addr6.sin6_family = AF_INET6;
	inet_pton(AF_INET6, "FF02::1", &(addr6.sin6_addr));
	addr6.sin6_port = htons(5350);
	len = sendto_or_schedule(socket6, buff, PCP_MIN_LEN, 0, (struct sockaddr *)&addr6, sizeof(struct sockaddr_in6));
	if( len < 0 ) {
		syslog(LOG_ERR, "PCPSendUnsolicitedAnnounce() IPv6 sendto(): %m");
	}
#endif /* ENABLE_IPV6 */
}