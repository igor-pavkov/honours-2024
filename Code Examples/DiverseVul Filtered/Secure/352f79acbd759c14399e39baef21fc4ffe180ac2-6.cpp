static int bcast_or_ucast(struct dhcp_packet *packet, uint32_t ciaddr, uint32_t server)
{
	if (server)
		return udhcp_send_kernel_packet(packet,
			ciaddr, CLIENT_PORT,
			server, SERVER_PORT);
	return raw_bcast_from_client_config_ifindex(packet);
}