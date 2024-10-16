static int CheckExternalAddress(pcp_info_t* pcp_msg_info)
{
	/* can contain a IPv4-mapped IPv6 address */
	static struct in6_addr external_addr;
	int af;

	af = IN6_IS_ADDR_V4MAPPED(pcp_msg_info->mapped_ip)
		? AF_INET : AF_INET6;

	pcp_msg_info->is_fw = af == AF_INET6;

	if (pcp_msg_info->is_fw) {
		external_addr = *pcp_msg_info->mapped_ip;
	} else {
		/* TODO : be able to handle case with multiple
		 * external addresses */
		if(use_ext_ip_addr) {
			if (inet_pton(AF_INET, use_ext_ip_addr,
				      ((uint32_t*)external_addr.s6_addr)+3) == 1) {
				((uint32_t*)external_addr.s6_addr)[0] = 0;
				((uint32_t*)external_addr.s6_addr)[1] = 0;
				((uint32_t*)external_addr.s6_addr)[2] = htonl(0xFFFF);
			} else if (inet_pton(AF_INET6, use_ext_ip_addr, external_addr.s6_addr)
				   != 1) {
				pcp_msg_info->result_code = PCP_ERR_NETWORK_FAILURE;
				return -1;
			}
		} else {
			if(!ext_if_name || ext_if_name[0]=='\0') {
				pcp_msg_info->result_code = PCP_ERR_NETWORK_FAILURE;
				return -1;
			}
			if(getifaddr_in6(ext_if_name, af, &external_addr) < 0) {
				pcp_msg_info->result_code = PCP_ERR_NETWORK_FAILURE;
				return -1;
			}
		}
	}
	if (pcp_msg_info->ext_ip == NULL ||
	    IN6_IS_ADDR_UNSPECIFIED(pcp_msg_info->ext_ip) ||
	    (IN6_IS_ADDR_V4MAPPED(pcp_msg_info->ext_ip)
	      && ((uint32_t *)pcp_msg_info->ext_ip->s6_addr)[3] == INADDR_ANY)) {
		/* no suggested external address : use real external address */
		pcp_msg_info->ext_ip = &external_addr;
		return 0;
	}

	if (!IN6_ARE_ADDR_EQUAL(pcp_msg_info->ext_ip, &external_addr)) {
		syslog(LOG_ERR,
		       "PCP: External IP in request didn't match interface IP \n");
#ifdef DEBUG
		{
			char s[INET6_ADDRSTRLEN];
			syslog(LOG_DEBUG, "Interface IP %s \n",
			       inet_ntop(AF_INET6, &external_addr.s6_addr, s, sizeof(s)));
			syslog(LOG_DEBUG, "IP in the PCP request %s \n",
			       inet_ntop(AF_INET6, pcp_msg_info->ext_ip, s, sizeof(s)));
		}
#endif

		if (pcp_msg_info->pfailure_present) {
			pcp_msg_info->result_code = PCP_ERR_CANNOT_PROVIDE_EXTERNAL;
			return -1;
		} else {
			pcp_msg_info->ext_ip = &external_addr;
		}

	}

	return 0;
}