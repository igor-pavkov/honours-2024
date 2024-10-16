int ProcessIncomingPCPPacket(int s, unsigned char *buff, int len,
                             const struct sockaddr *senderaddr,
                             const struct sockaddr_in6 *receiveraddr)
{
	pcp_info_t pcp_msg_info;
	struct lan_addr_s * lan_addr;
	char addr_str[64];

	memset(&pcp_msg_info, 0, sizeof(pcp_info_t));

	if(senderaddr->sa_family == AF_INET) {
		const struct sockaddr_in * senderaddr_v4 =
			(const struct sockaddr_in *)senderaddr;
		pcp_msg_info.sender_ip.s6_addr[11] = 0xff;
		pcp_msg_info.sender_ip.s6_addr[10] = 0xff;
		memcpy(pcp_msg_info.sender_ip.s6_addr+12,
		       &senderaddr_v4->sin_addr, 4);
	} else if(senderaddr->sa_family == AF_INET6) {
		const struct sockaddr_in6 * senderaddr_v6 =
			(const struct sockaddr_in6 *)senderaddr;
		pcp_msg_info.sender_ip = senderaddr_v6->sin6_addr;
	} else {
		syslog(LOG_WARNING, "unknown PCP packet sender address family %d",
		       senderaddr->sa_family);
		return 0;
	}

	if(sockaddr_to_string(senderaddr, addr_str, sizeof(addr_str)))
		syslog(LOG_DEBUG, "PCP request received from %s %dbytes",
		       addr_str, len);

	if(buff[1] & 128) {
		/* discarding PCP responses silently */
		return 0;
	}

	/* If we're in allow third party-mode, we probably don't care
	 * about locality either. Let's hope firewall is ok. */
	if (!GETFLAG(PCP_ALLOWTHIRDPARTYMASK)) {
		lan_addr = get_lan_for_peer(senderaddr);
		if(lan_addr == NULL) {
			syslog(LOG_WARNING, "PCP packet sender %s not from a LAN, ignoring",
			       addr_str);
			return 0;
		}
	}

	if (processPCPRequest(buff, len, &pcp_msg_info) ) {

		createPCPResponse(buff, &pcp_msg_info);

		if(len < PCP_MIN_LEN)
			len = PCP_MIN_LEN;
		else
			len = (len + 3) & ~3;	/* round up resp. length to multiple of 4 */
		len = sendto_or_schedule2(s, buff, len, 0, senderaddr,
		           (senderaddr->sa_family == AF_INET) ?
		                  sizeof(struct sockaddr_in) :
		                  sizeof(struct sockaddr_in6),
		           receiveraddr);
		if( len < 0 ) {
			syslog(LOG_ERR, "sendto(pcpserver): %m");
		}
	}

	return 0;
}