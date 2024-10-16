static int CreatePCPMap_FW(pcp_info_t *pcp_msg_info)
{
#ifdef ENABLE_UPNPPINHOLE
	int uid;
	int r;
	/* first check if pinhole already exists */
	uid = upnp_find_inboundpinhole(NULL, 0,
					pcp_msg_info->mapped_str,
					pcp_msg_info->int_port,
					pcp_msg_info->protocol,
					NULL, 0, /* desc */
					NULL /* lifetime */);
	if(uid >= 0) {
		/* pinhole already exists, updating */
		syslog(LOG_INFO, "updating pinhole to %s:%hu %s",
		       pcp_msg_info->mapped_str, pcp_msg_info->int_port,
		       (pcp_msg_info->protocol == IPPROTO_TCP)?"TCP":"UDP");
		r = upnp_update_inboundpinhole((unsigned short)uid, pcp_msg_info->lifetime);
		return r >= 0 ? PCP_SUCCESS : PCP_ERR_NO_RESOURCES;
	} else {
		r = upnp_add_inboundpinhole(NULL, 0,
						pcp_msg_info->mapped_str,
						pcp_msg_info->int_port,
						pcp_msg_info->protocol,
						pcp_msg_info->desc,
						pcp_msg_info->lifetime,
						&uid);
		if (r < 0)
			return PCP_ERR_NO_RESOURCES;
		pcp_msg_info->ext_port = pcp_msg_info->int_port;
		return PCP_SUCCESS;
	}
#else
	UNUSED(pcp_msg_info);
	return PCP_ERR_NO_RESOURCES;
#endif /* ENABLE_UPNPPINHOLE */
}