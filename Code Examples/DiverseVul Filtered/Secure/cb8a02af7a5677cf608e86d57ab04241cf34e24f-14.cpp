static void CreatePCPPeer(pcp_info_t *pcp_msg_info)
{
	char peerip_s[INET6_ADDRSTRLEN];
	int r = -1;

	if (!inet_n46top(pcp_msg_info->peer_ip, peerip_s, sizeof(peerip_s))) {
		syslog(LOG_ERR, "inet_n46top(peer_ip): %m");
		return;
	}

	if (pcp_msg_info->is_fw) {
#if 0
		/* Someday, something like this is available.. and we're ready! */
#ifdef ENABLE_UPNPPINHOLE
		pcp_msg_info->ext_port = pcp_msg_info->int_port;
		r = upnp_add_outbound_pinhole(peerip_s,
					      pcp_msg_info->peer_port,
					      pcp_msg_info->mapped_str,
					      pcp_msg_info->int_port,
					      pcp_msg_info->protocol,
					      pcp_msg_info->desc,
					      pcp_msg_info->lifetime, NULL);
#endif /* ENABLE_UPNPPINHOLE */
#else
		r = PCP_ERR_UNSUPP_OPCODE;
#endif /* 0 */
	} else {
		r = CreatePCPPeer_NAT(pcp_msg_info);
	}
	/* TODO: add upnp function for PI */
	pcp_msg_info->result_code = r;
	syslog(r == PCP_SUCCESS ? LOG_INFO : LOG_ERR,
	       "PCP PEER: %s peer mapping %s %s:%hu(%hu)->%s:%hu '%s'",
	       r == PCP_SUCCESS ? "added" : "failed to add",
	       (pcp_msg_info->protocol==IPPROTO_TCP)?"TCP":"UDP",
	       pcp_msg_info->mapped_str,
	       pcp_msg_info->int_port,
	       pcp_msg_info->ext_port,
	       peerip_s,
	       pcp_msg_info->peer_port,
	       pcp_msg_info->desc);
}