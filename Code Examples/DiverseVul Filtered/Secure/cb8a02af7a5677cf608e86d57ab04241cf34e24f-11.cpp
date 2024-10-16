static void parsePCPMAP_version2(const uint8_t *map_v2,
		pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->is_map_op = 1;
	memcpy(pcp_msg_info->nonce, map_v2, 12);
	pcp_msg_info->protocol = map_v2[12];
	pcp_msg_info->int_port = READNU16(map_v2 + 16);
	pcp_msg_info->ext_port = READNU16(map_v2 + 18);

	pcp_msg_info->ext_ip = (struct in6_addr *)(map_v2 + 20);
}