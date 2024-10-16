static int CreatePCPMap_NAT(pcp_info_t *pcp_msg_info)
{
	int r = 0;
	char iaddr_old[INET6_ADDRSTRLEN];
	uint16_t iport_old, eport_first = 0;
	int any_eport_allowed = 0;
	unsigned int timestamp = upnp_time() + pcp_msg_info->lifetime;

	if (pcp_msg_info->ext_port == 0) {
		pcp_msg_info->ext_port = pcp_msg_info->int_port;
	}

	/* TODO: Support non-TCP/UDP */
	if (pcp_msg_info->ext_port == 0) {
		return PCP_ERR_MALFORMED_REQUEST;
	}

	do {
		if (eport_first == 0) { /* first time in loop */
			eport_first = pcp_msg_info->ext_port;
		} else if (pcp_msg_info->ext_port == eport_first) { /* no eport available */
                        /* all eports rejected by permissions? */
			if (any_eport_allowed == 0)
				return PCP_ERR_NOT_AUTHORIZED;
			/* at least one eport allowed (but none available) */
			return PCP_ERR_NO_RESOURCES;
		}
		if ((IN6_IS_ADDR_V4MAPPED(pcp_msg_info->mapped_ip) &&
		     (!check_upnp_rule_against_permissions(upnppermlist,
							   num_upnpperm, pcp_msg_info->ext_port,
							   ((struct in_addr*)pcp_msg_info->mapped_ip->s6_addr)[3],
							   pcp_msg_info->int_port)))) {
			if (pcp_msg_info->pfailure_present) {
				return PCP_ERR_CANNOT_PROVIDE_EXTERNAL;
			}
			pcp_msg_info->ext_port++;
			if (pcp_msg_info->ext_port == 0) { /* skip port zero */
				pcp_msg_info->ext_port++;
			}
			continue;
		}
		any_eport_allowed = 1;
#ifdef CHECK_PORTINUSE
		if (port_in_use(ext_if_name, pcp_msg_info->ext_port, pcp_msg_info->protocol,
				pcp_msg_info->mapped_str, pcp_msg_info->int_port) > 0) {
			syslog(LOG_INFO, "port %hu protocol %s already in use",
			       pcp_msg_info->ext_port,
			       (pcp_msg_info->protocol==IPPROTO_TCP)?"tcp":"udp");
			pcp_msg_info->ext_port++;
			if (pcp_msg_info->ext_port == 0) { /* skip port zero */
				pcp_msg_info->ext_port++;
			}
			continue;
		}
#endif
		r = get_redirect_rule(ext_if_name,
				      pcp_msg_info->ext_port,
				      pcp_msg_info->protocol,
				      iaddr_old, sizeof(iaddr_old),
				      &iport_old, 0, 0, 0, 0,
				      NULL/*&timestamp*/, 0, 0);

		if(r==0) {
			if((strcmp(pcp_msg_info->mapped_str, iaddr_old)!=0)
			   || (pcp_msg_info->int_port != iport_old)) {
				/* redirection already existing */
				if (pcp_msg_info->pfailure_present) {
					return PCP_ERR_CANNOT_PROVIDE_EXTERNAL;
				}
			} else {
				syslog(LOG_INFO, "port %hu %s already redirected to %s:%hu, replacing",
				       pcp_msg_info->ext_port, (pcp_msg_info->protocol==IPPROTO_TCP)?"tcp":"udp",
				       iaddr_old, iport_old);
				/* remove and then add again */
				if (_upnp_delete_redir(pcp_msg_info->ext_port,
						       pcp_msg_info->protocol)==0) {
					break;
				} else if (pcp_msg_info->pfailure_present) {
					return PCP_ERR_CANNOT_PROVIDE_EXTERNAL;
				}
			}
			pcp_msg_info->ext_port++;
			if (pcp_msg_info->ext_port == 0) { /* skip port zero */
				pcp_msg_info->ext_port++;
			}
		}
	} while (r==0);

	r = upnp_redirect_internal(NULL,
				   pcp_msg_info->ext_port,
				   pcp_msg_info->mapped_str,
				   pcp_msg_info->int_port,
				   pcp_msg_info->protocol,
				   pcp_msg_info->desc,
				   timestamp);
	if (r < 0)
		return PCP_ERR_NO_RESOURCES;
	return PCP_SUCCESS;
}