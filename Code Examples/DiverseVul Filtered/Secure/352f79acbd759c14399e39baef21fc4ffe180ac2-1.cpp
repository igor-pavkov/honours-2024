static int send_release(uint32_t server, uint32_t ciaddr)
{
	struct dhcp_packet packet;

	/* Fill in: op, htype, hlen, cookie, chaddr, random xid fields,
	 * client-id option (unless -C), message type option:
	 */
	init_packet(&packet, DHCPRELEASE);

	/* DHCPRELEASE uses ciaddr, not "requested ip", to store IP being released */
	packet.ciaddr = ciaddr;

	udhcp_add_simple_option(&packet, DHCP_SERVER_ID, server);

	bb_info_msg("Sending release...");
	/* Note: normally we unicast here since "server" is not zero.
	 * However, there _are_ people who run "address-less" DHCP servers,
	 * and reportedly ISC dhcp client and Windows allow that.
	 */
	return bcast_or_ucast(&packet, ciaddr, server);
}