static void parsePCPPEER_version2(const uint8_t *buf, pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->is_peer_op = 1;
	memcpy(pcp_msg_info->nonce, buf, 12);
	pcp_msg_info->protocol = buf[12];
	pcp_msg_info->int_port = READNU16(buf + 16);
	pcp_msg_info->ext_port = READNU16(buf + 18);
	pcp_msg_info->peer_port = READNU16(buf + 36);

	pcp_msg_info->ext_ip = (struct in6_addr *)(buf + 20);
	pcp_msg_info->peer_ip = (struct in6_addr *)(buf + 40);
}