static NOINLINE int send_renew(uint32_t xid, uint32_t server, uint32_t ciaddr)
{
	struct dhcp_packet packet;

/*
 * RFC 2131 4.3.2 DHCPREQUEST message
 * ...
 * DHCPREQUEST generated during RENEWING state:
 *
 * 'server identifier' MUST NOT be filled in, 'requested IP address'
 * option MUST NOT be filled in, 'ciaddr' MUST be filled in with
 * client's IP address. In this situation, the client is completely
 * configured, and is trying to extend its lease. This message will
 * be unicast, so no relay agents will be involved in its
 * transmission.  Because 'giaddr' is therefore not filled in, the
 * DHCP server will trust the value in 'ciaddr', and use it when
 * replying to the client.
 */
	/* Fill in: op, htype, hlen, cookie, chaddr fields,
	 * random xid field (we override it below),
	 * client-id option (unless -C), message type option:
	 */
	init_packet(&packet, DHCPREQUEST);

	packet.xid = xid;
	packet.ciaddr = ciaddr;

	/* Add options: maxsize,
	 * optionally: hostname, fqdn, vendorclass,
	 * "param req" option according to -O, and options specified with -x
	 */
	add_client_options(&packet);

	bb_info_msg("Sending renew...");
	return bcast_or_ucast(&packet, ciaddr, server);
}