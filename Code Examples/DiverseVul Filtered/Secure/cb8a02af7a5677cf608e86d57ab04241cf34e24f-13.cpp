static void parsePCPMAP_version1(const uint8_t *map_v1,
		pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->is_map_op = 1;
	pcp_msg_info->protocol = map_v1[0];
	pcp_msg_info->int_port = READNU16(map_v1 + 4);
	pcp_msg_info->ext_port = READNU16(map_v1 + 6);

	pcp_msg_info->ext_ip = (struct in6_addr *)(map_v1 + 8);
}