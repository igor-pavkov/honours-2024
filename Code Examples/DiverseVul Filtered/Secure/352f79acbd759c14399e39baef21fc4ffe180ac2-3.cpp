static NOINLINE int udhcp_recv_raw_packet(struct dhcp_packet *dhcp_pkt, int fd)
{
	int bytes;
	struct ip_udp_dhcp_packet packet;
	uint16_t check;
	unsigned char cmsgbuf[CMSG_LEN(sizeof(struct tpacket_auxdata))];
	struct iovec iov;
	struct msghdr msg;
	struct cmsghdr *cmsg;

	/* used to use just safe_read(fd, &packet, sizeof(packet))
	 * but we need to check for TP_STATUS_CSUMNOTREADY :(
	 */
	iov.iov_base = &packet;
	iov.iov_len = sizeof(packet);
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);
	for (;;) {
		bytes = recvmsg(fd, &msg, 0);
		if (bytes < 0) {
			if (errno == EINTR)
				continue;
			log1("Packet read error, ignoring");
			/* NB: possible down interface, etc. Caller should pause. */
			return bytes; /* returns -1 */
		}
		break;
	}

	if (bytes < (int) (sizeof(packet.ip) + sizeof(packet.udp))) {
		log1("Packet is too short, ignoring");
		return -2;
	}

	if (bytes < ntohs(packet.ip.tot_len)) {
		/* packet is bigger than sizeof(packet), we did partial read */
		log1("Oversized packet, ignoring");
		return -2;
	}

	/* ignore any extra garbage bytes */
	bytes = ntohs(packet.ip.tot_len);

	/* make sure its the right packet for us, and that it passes sanity checks */
	if (packet.ip.protocol != IPPROTO_UDP
	 || packet.ip.version != IPVERSION
	 || packet.ip.ihl != (sizeof(packet.ip) >> 2)
	 || packet.udp.dest != htons(CLIENT_PORT)
	/* || bytes > (int) sizeof(packet) - can't happen */
	 || ntohs(packet.udp.len) != (uint16_t)(bytes - sizeof(packet.ip))
	) {
		log1("Unrelated/bogus packet, ignoring");
		return -2;
	}

	/* verify IP checksum */
	check = packet.ip.check;
	packet.ip.check = 0;
	if (check != inet_cksum((uint16_t *)&packet.ip, sizeof(packet.ip))) {
		log1("Bad IP header checksum, ignoring");
		return -2;
	}

	for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		if (cmsg->cmsg_level == SOL_PACKET
		 && cmsg->cmsg_type == PACKET_AUXDATA
		) {
			/* some VMs don't checksum UDP and TCP data
			 * they send to the same physical machine,
			 * here we detect this case:
			 */
			struct tpacket_auxdata *aux = (void *)CMSG_DATA(cmsg);
			if (aux->tp_status & TP_STATUS_CSUMNOTREADY)
				goto skip_udp_sum_check;
		}
	}

	/* verify UDP checksum. IP header has to be modified for this */
	memset(&packet.ip, 0, offsetof(struct iphdr, protocol));
	/* ip.xx fields which are not memset: protocol, check, saddr, daddr */
	packet.ip.tot_len = packet.udp.len; /* yes, this is needed */
	check = packet.udp.check;
	packet.udp.check = 0;
	if (check && check != inet_cksum((uint16_t *)&packet, bytes)) {
		log1("Packet with bad UDP checksum received, ignoring");
		return -2;
	}
 skip_udp_sum_check:

	if (packet.data.cookie != htonl(DHCP_MAGIC)) {
		bb_info_msg("Packet with bad magic, ignoring");
		return -2;
	}

	log1("Received a packet");
	udhcp_dump_packet(&packet.data);

	bytes -= sizeof(packet.ip) + sizeof(packet.udp);
	memcpy(dhcp_pkt, &packet.data, bytes);
	return bytes;
}