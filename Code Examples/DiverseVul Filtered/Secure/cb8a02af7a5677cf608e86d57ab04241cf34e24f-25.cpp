static void createPCPResponse(unsigned char *response, const pcp_info_t *pcp_msg_info)
{
	response[2] = 0;	/* reserved */
	memset(response + 12, 0, 12);	/* reserved */
	if (pcp_msg_info->result_code == PCP_ERR_UNSUPP_VERSION ) {
		/* highest supported version */
		response[0] = this_server_info.server_version;
	} else {
		response[0] = pcp_msg_info->version;
	}

	response[1] = pcp_msg_info->opcode | 0x80;	/* r_opcode */
	response[3] = pcp_msg_info->result_code;
	if(epoch_origin == 0) {
		epoch_origin = startup_time;
	}
	WRITENU32(response + 8, upnp_time() - epoch_origin); /* epochtime */
	switch (pcp_msg_info->result_code) {
	/*long lifetime errors*/
	case PCP_ERR_UNSUPP_VERSION:
	case PCP_ERR_NOT_AUTHORIZED:
	case PCP_ERR_MALFORMED_REQUEST:
	case PCP_ERR_UNSUPP_OPCODE:
	case PCP_ERR_UNSUPP_OPTION:
	case PCP_ERR_MALFORMED_OPTION:
	case PCP_ERR_UNSUPP_PROTOCOL:
	case PCP_ERR_ADDRESS_MISMATCH:
	case PCP_ERR_CANNOT_PROVIDE_EXTERNAL:
	case PCP_ERR_EXCESSIVE_REMOTE_PEERS:
		WRITENU32(response + 4, 0);	/* lifetime */
		break;

	case PCP_ERR_NETWORK_FAILURE:
	case PCP_ERR_NO_RESOURCES:
	case PCP_ERR_USER_EX_QUOTA:
		WRITENU32(response + 4, 30);	/* lifetime */
		break;

	case PCP_SUCCESS:
	default:
		WRITENU32(response + 4, pcp_msg_info->lifetime);	/* lifetime */
		break;
	}

	if (response[1] == 0x81) { /* MAP response */
		if (response[0] == 1) {	/* version */
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 4, pcp_msg_info->int_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 6, pcp_msg_info->ext_port);
			copyIPv6IfDifferent(response + PCP_COMMON_RESPONSE_SIZE + 8,
			                    pcp_msg_info->ext_ip);
		}
		else if (response[0] == 2) {
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 16, pcp_msg_info->int_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 18, pcp_msg_info->ext_port);
			copyIPv6IfDifferent(response + PCP_COMMON_RESPONSE_SIZE + 20,
			                    pcp_msg_info->ext_ip);
		}
	}
#ifdef PCP_PEER
	else if (response[1] == 0x82) { /* PEER response */
		if (response[0] == 1) {
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 4, pcp_msg_info->int_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 6, pcp_msg_info->ext_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 24, pcp_msg_info->peer_port);
			copyIPv6IfDifferent(response + PCP_COMMON_RESPONSE_SIZE + 8,
			                    pcp_msg_info->ext_ip);
		}
		else if (response[0] == 2) {
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 16, pcp_msg_info->int_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 18, pcp_msg_info->ext_port);
			WRITENU16(response + PCP_COMMON_RESPONSE_SIZE + 36, pcp_msg_info->peer_port);
			copyIPv6IfDifferent(response + PCP_COMMON_RESPONSE_SIZE + 20,
			                    pcp_msg_info->ext_ip);
		}
	}
#endif /* PCP_PEER */

#ifdef PCP_SADSCP
	else if (response[1] == 0x83) { /*SADSCP response*/
		response[PCP_COMMON_RESPONSE_SIZE + 12]
			= ((pcp_msg_info->matched_name<<7) & ~(1<<6)) |
			  (pcp_msg_info->sadscp_dscp & PCP_SADSCP_MASK);
		memset(response + PCP_COMMON_RESPONSE_SIZE + 13, 0, 3);
	}
#endif /* PCP_SADSCP */
}