static void DeletePCPPeer(pcp_info_t *pcp_msg_info)
{
	uint16_t iport = pcp_msg_info->int_port;  /* private port */
	uint16_t rport = pcp_msg_info->peer_port;  /* private port */
	uint8_t  proto = pcp_msg_info->protocol;
	char rhost[INET6_ADDRSTRLEN];
	int r = -1;

	/* remove requested mappings for this client */
	int index = 0;
	unsigned short eport2, iport2, rport2;
	char iaddr2[INET6_ADDRSTRLEN], rhost2[INET6_ADDRSTRLEN];
	int proto2;
	char desc[64];
	unsigned int timestamp;
#if 0
	int uid;
#endif /* 0 */

	if (pcp_msg_info->is_fw) {
		pcp_msg_info->result_code = PCP_ERR_UNSUPP_OPCODE;
		return;
	}

	inet_n46top((struct in6_addr*)pcp_msg_info->peer_ip, rhost, sizeof(rhost));

	for (index = 0 ;
	     (!pcp_msg_info->is_fw &&
	      get_peer_rule_by_index(index, 0,
				     &eport2, iaddr2, sizeof(iaddr2),
				     &iport2, &proto2,
				     desc, sizeof(desc),
				     rhost2, sizeof(rhost2), &rport2,
				     &timestamp, 0, 0) >= 0)
#if 0
		     /* Some day if outbound pinholes are supported.. */
		     ||
		     (pcp_msg_info->is_fw &&
		      (uid=upnp_get_pinhole_uid_by_index(index))>=0 &&
		      upnp_get_pinhole_info((unsigned short)uid,
					    rhost2, sizeof(rhost2), &rport2,
					    iaddr2, sizeof(iaddr2), &iport2,
					    &proto2, desc, sizeof(desc),
					    &timestamp, NULL) >= 0)
#endif /* 0 */
		     ;
	     index++)
		if((0 == strcmp(iaddr2, pcp_msg_info->mapped_str))
		   && (0 == strcmp(rhost2, rhost))
		   && (proto2==proto)
		   && 0 == strcmp(desc, pcp_msg_info->desc)
		   && (iport2==iport) && (rport2==rport)) {
			if (!pcp_msg_info->is_fw)
				r = _upnp_delete_redir(eport2, proto2);
#if 0
			else
				r = upnp_delete_outboundpinhole(uid);
#endif /* 0 */
			if(r<0) {
				syslog(LOG_ERR, "PCP PEER: failed to remove peer mapping");
			} else {
				syslog(LOG_INFO, "PCP PEER: %s port %hu peer mapping removed",
				       proto2==IPPROTO_TCP?"TCP":"UDP", eport2);
			}
			return;
		}
	if (r==-1) {
		syslog(LOG_ERR, "PCP PEER: Failed to find PCP mapping internal port %hu, protocol %s",
		       iport, (pcp_msg_info->protocol == IPPROTO_TCP)?"TCP":"UDP");
		pcp_msg_info->result_code = PCP_ERR_NO_RESOURCES;
	}
}