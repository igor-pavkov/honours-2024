static int CreatePCPPeer_NAT(pcp_info_t *pcp_msg_info)
{
	struct sockaddr_storage intip;
	struct sockaddr_storage peerip;
	struct sockaddr_storage extip;
	struct sockaddr_storage ret_extip;

	uint8_t  proto = pcp_msg_info->protocol;

	uint16_t eport = pcp_msg_info->ext_port;  /* public port */

	char peerip_s[INET6_ADDRSTRLEN], extip_s[INET6_ADDRSTRLEN];
	time_t timestamp = upnp_time() + pcp_msg_info->lifetime;
	int r;

	FillSA((struct sockaddr*)&intip, pcp_msg_info->mapped_ip,
	       pcp_msg_info->int_port);
	FillSA((struct sockaddr*)&peerip, pcp_msg_info->peer_ip,
	       pcp_msg_info->peer_port);
	FillSA((struct sockaddr*)&extip, pcp_msg_info->ext_ip,
	       eport);

	inet_satop((struct sockaddr*)&peerip, peerip_s, sizeof(peerip_s));
	inet_satop((struct sockaddr*)&extip, extip_s, sizeof(extip_s));

	/* check if connection with given peer exists, if it was */
	/* already established use this external port */
	if (get_nat_ext_addr( (struct sockaddr*)&intip, (struct sockaddr*)&peerip,
			      proto, (struct sockaddr*)&ret_extip) == 1) {
		if (ret_extip.ss_family == AF_INET) {
			struct sockaddr_in* ret_ext4 = (struct sockaddr_in*)&ret_extip;
			uint16_t ret_eport = ntohs(ret_ext4->sin_port);
			eport = ret_eport;
		} else if (ret_extip.ss_family == AF_INET6) {
			struct sockaddr_in6* ret_ext6 = (struct sockaddr_in6*)&ret_extip;
			uint16_t ret_eport = ntohs(ret_ext6->sin6_port);
			eport = ret_eport;
		} else {
			return PCP_ERR_CANNOT_PROVIDE_EXTERNAL;
		}
	}
	/* Create Peer Mapping */
	if (eport == 0) {
		eport = pcp_msg_info->int_port;
	}

#ifdef PCP_FLOWP
	if (pcp_msg_info->flowp_present && pcp_msg_info->dscp_up) {
		if (add_peer_dscp_rule2(ext_if_name, peerip_s,
					pcp_msg_info->peer_port, pcp_msg_info->dscp_up,
					pcp_msg_info->mapped_str, pcp_msg_info->int_port,
					proto, pcp_msg_info->desc, timestamp) < 0 ) {
			syslog(LOG_ERR, "PCP: failed to add flowp upstream mapping %s:%hu->%s:%hu '%s'",
			       pcp_msg_info->mapped_str,
			       pcp_msg_info->int_port,
			       peerip_s,
			       pcp_msg_info->peer_port,
			       pcp_msg_info->desc);
			return PCP_ERR_NO_RESOURCES;
		}
	}

	if (pcp_msg_info->flowp_present && pcp_msg_info->dscp_down) {
		if (add_peer_dscp_rule2(ext_if_name,  pcp_msg_info->mapped_str,
					pcp_msg_info->int_port, pcp_msg_info->dscp_down,
					peerip_s, pcp_msg_info->peer_port, proto, pcp_msg_info->desc, timestamp)
		    < 0 ) {
			syslog(LOG_ERR, "PCP: failed to add flowp downstream mapping %s:%hu->%s:%hu '%s'",
			       pcp_msg_info->mapped_str,
			       pcp_msg_info->int_port,
			       peerip_s,
			       pcp_msg_info->peer_port,
			       pcp_msg_info->desc);
			pcp_msg_info->result_code = PCP_ERR_NO_RESOURCES;
			return PCP_ERR_NO_RESOURCES;
		}
	}
#endif

	r = add_peer_redirect_rule2(ext_if_name,
				    peerip_s,
				    pcp_msg_info->peer_port,
				    extip_s,
				    eport,
				    pcp_msg_info->mapped_str,
				    pcp_msg_info->int_port,
				    pcp_msg_info->protocol,
				    pcp_msg_info->desc,
				    timestamp);
	if (r < 0)
		return PCP_ERR_NO_RESOURCES;
	pcp_msg_info->ext_port = eport;
	return PCP_SUCCESS;
}