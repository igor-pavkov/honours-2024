RADIUS_PACKET *fr_dhcp_recv(int sockfd)
{
	uint32_t		magic;
	struct sockaddr_storage	src;
	struct sockaddr_storage	dst;
	socklen_t		sizeof_src;
	socklen_t	        sizeof_dst;
	RADIUS_PACKET		*packet;
	int port;
	uint8_t			*code;

	packet = rad_alloc(0);
	if (!packet) {
		fr_strerror_printf("Failed allocating packet");
		return NULL;
	}
	memset(packet, 0, sizeof(packet));

	packet->data = malloc(MAX_PACKET_SIZE);
	if (!packet->data) {
		fr_strerror_printf("Failed in malloc");
		rad_free(&packet);
		return NULL;
	}

	packet->sockfd = sockfd;
	sizeof_src = sizeof(src);
#ifdef WITH_UDPFROMTO
	sizeof_dst = sizeof(dst);
	packet->data_len = recvfromto(sockfd, packet->data, MAX_PACKET_SIZE, 0,
				      (struct sockaddr *)&src, &sizeof_src,
				      (struct sockaddr *)&dst, &sizeof_dst);
#else
	packet->data_len = recvfrom(sockfd, packet->data, MAX_PACKET_SIZE, 0,
				    (struct sockaddr *)&src, &sizeof_src);
#endif

	if (packet->data_len <= 0) {
		fr_strerror_printf("Failed reading DHCP socket: %s", strerror(errno));
		rad_free(&packet);
		return NULL;
	}

	if (packet->data_len < MIN_PACKET_SIZE) {
		fr_strerror_printf("DHCP packet is too small (%d < %d)",
		      packet->data_len, MIN_PACKET_SIZE);
		rad_free(&packet);
		return NULL;
	}

	if (packet->data[1] != 1) {
		fr_strerror_printf("DHCP can only receive ethernet requests, not type %02x",
		      packet->data[1]);
		rad_free(&packet);
		return NULL;
	}

	if (packet->data[2] != 6) {
		fr_strerror_printf("Ethernet HW length is wrong length %d",
			packet->data[2]);
		rad_free(&packet);
		return NULL;
	}

	memcpy(&magic, packet->data + 236, 4);
	magic = ntohl(magic);
	if (magic != DHCP_OPTION_MAGIC_NUMBER) {
		fr_strerror_printf("Cannot do BOOTP");
		rad_free(&packet);
		return NULL;
	}

	/*
	 *	Create unique keys for the packet.
	 */
	memcpy(&magic, packet->data + 4, 4);
	packet->id = ntohl(magic);

	code = dhcp_get_option((dhcp_packet_t *) packet->data,
			       packet->data_len, 53);
	if (!code) {
		fr_strerror_printf("No message-type option was found in the packet");
		rad_free(&packet);
		return NULL;
	}

	if ((code[1] < 1) || (code[2] == 0) || (code[2] > 8)) {
		fr_strerror_printf("Unknown value for message-type option");
		rad_free(&packet);
		return NULL;
	}

	packet->code = code[2] | PW_DHCP_OFFSET;

	/*
	 *	Create a unique vector from the MAC address and the
	 *	DHCP opcode.  This is a hack for the RADIUS
	 *	infrastructure in the rest of the server.
	 *
	 *	Note: packet->data[2] == 6, which is smaller than
	 *	sizeof(packet->vector)
	 *
	 *	FIXME:  Look for client-identifier in packet,
	 *      and use that, too?
	 */
	memset(packet->vector, 0, sizeof(packet->vector));
	memcpy(packet->vector, packet->data + 28, packet->data[2]);
	packet->vector[packet->data[2]] = packet->code & 0xff;

	/*
	 *	FIXME: for DISCOVER / REQUEST: src_port == dst_port + 1
	 *	FIXME: for OFFER / ACK       : src_port = dst_port - 1
	 */

	/*
	 *	Unique keys are xid, client mac, and client ID?
	 */

	/*
	 *	FIXME: More checks, like DHCP packet type?
	 */

	sizeof_dst = sizeof(dst);

#ifndef WITH_UDPFROMTO
	/*
	 *	This should never fail...
	 */
	getsockname(sockfd, (struct sockaddr *) &dst, &sizeof_dst);
#endif
	
	fr_sockaddr2ipaddr(&dst, sizeof_dst, &packet->dst_ipaddr, &port);
	packet->dst_port = port;

	fr_sockaddr2ipaddr(&src, sizeof_src, &packet->src_ipaddr, &port);
	packet->src_port = port;

	if (fr_debug_flag > 1) {
		char type_buf[64];
		const char *name = type_buf;
		char src_ip_buf[256], dst_ip_buf[256];
		
		if ((packet->code >= PW_DHCP_DISCOVER) &&
		    (packet->code <= PW_DHCP_INFORM)) {
			name = dhcp_message_types[packet->code - PW_DHCP_OFFSET];
		} else {
			snprintf(type_buf, sizeof(type_buf), "%d",
				 packet->code - PW_DHCP_OFFSET);
		}

		DEBUG("Received %s of id %08x from %s:%d to %s:%d",
		       name, (unsigned int) packet->id,
		       inet_ntop(packet->src_ipaddr.af,
				 &packet->src_ipaddr.ipaddr,
				 src_ip_buf, sizeof(src_ip_buf)),
		       packet->src_port,
		       inet_ntop(packet->dst_ipaddr.af,
				 &packet->dst_ipaddr.ipaddr,
				 dst_ip_buf, sizeof(dst_ip_buf)),
		       packet->dst_port);
	}

	return packet;
}