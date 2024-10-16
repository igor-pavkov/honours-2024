static void CreatePCPMap(pcp_info_t *pcp_msg_info)
{
	int r;

	if (pcp_msg_info->is_fw)
		r = CreatePCPMap_FW(pcp_msg_info);
	else
		r = CreatePCPMap_NAT(pcp_msg_info);
	pcp_msg_info->result_code = r;
	syslog(r == PCP_SUCCESS ? LOG_INFO : LOG_ERR,
	      "PCP MAP: %s mapping %s %hu->%s:%hu '%s'",
	       r == PCP_SUCCESS ? "added" : "failed to add",
	       (pcp_msg_info->protocol==IPPROTO_TCP)?"TCP":"UDP",
	       pcp_msg_info->ext_port,
	       pcp_msg_info->mapped_str,
	       pcp_msg_info->int_port,
	       pcp_msg_info->desc);
}