static int parseCommonRequestHeader(const uint8_t *common_req, pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->version = common_req[0] ;
	pcp_msg_info->opcode = common_req[1] & 0x7f;
	pcp_msg_info->lifetime = READNU32(common_req + 4);
	pcp_msg_info->int_ip = (struct in6_addr *)(common_req + 8);
	pcp_msg_info->mapped_ip = (struct in6_addr *)(common_req + 8);


	if ( (pcp_msg_info->version > this_server_info.server_version) ) {
		pcp_msg_info->result_code = PCP_ERR_UNSUPP_VERSION;
		return 1;
	}

	if (pcp_msg_info->lifetime > max_lifetime ) {
		pcp_msg_info->lifetime = max_lifetime;
	}

	if ( (pcp_msg_info->lifetime < min_lifetime) && (pcp_msg_info->lifetime != 0) ) {
		pcp_msg_info->lifetime = min_lifetime;
	}

	return 0;
}