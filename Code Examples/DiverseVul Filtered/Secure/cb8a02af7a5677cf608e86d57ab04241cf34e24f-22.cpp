static void parsePCPPEER_version1(const uint8_t *buf,
		pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->is_peer_op = 1;
	pcp_msg_info->protocol = buf[0];
	pcp_msg_info->int_port = READNU16(buf + 4);
	pcp_msg_info->ext_port = READNU16(buf + 6);
	pcp_msg_info->peer_port = READNU16(buf + 24);

	pcp_msg_info->ext_ip = (struct in6_addr *)(buf + 8);
	pcp_msg_info->peer_ip = (struct in6_addr *)(buf + 28);
}