static int processPCPRequest(void * req, int req_size, pcp_info_t *pcp_msg_info)
{
	int remainingSize;

	/* start with PCP_SUCCESS as result code,
	 * if everything is OK value will be unchanged */
	pcp_msg_info->result_code = PCP_SUCCESS;

	remainingSize = req_size;

	/* discard request that exceeds maximal length,
	   or that is shorter than PCP_MIN_LEN (=24)
	   or that is not the multiple of 4 */
	if (req_size < 3)
		return 0; /* ignore msg */

	if (req_size < PCP_MIN_LEN) {
		pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
		return 1; /* send response */
	}

	if ( (req_size > PCP_MAX_LEN) || ( (req_size & 3) != 0)) {
		syslog(LOG_ERR, "PCP: Size of PCP packet(%d) is larger than %d bytes or "
		       "the size is not multiple of 4.\n", req_size, PCP_MAX_LEN);
		pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
		return 1; /* send response */
	}

	/* first parse request header */
	if (parseCommonRequestHeader(req, pcp_msg_info) ) {
		return 1;
	}

	remainingSize -= PCP_COMMON_REQUEST_SIZE;
	req += PCP_COMMON_REQUEST_SIZE;

	if (pcp_msg_info->version == 1) {
		/* legacy PCP version 1 support */
		switch (pcp_msg_info->opcode) {
		case PCP_OPCODE_MAP:

			remainingSize -= PCP_MAP_V1_SIZE;
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
				return pcp_msg_info->result_code;
			}

#ifdef DEBUG
			printMAPOpcodeVersion1(req);
#endif /* DEBUG */
			parsePCPMAP_version1(req, pcp_msg_info);

			req += PCP_MAP_V1_SIZE;

			parsePCPOptions(req, remainingSize, pcp_msg_info);
			if (ValidatePCPMsg(pcp_msg_info)) {
				if (pcp_msg_info->lifetime == 0) {
					DeletePCPMap(pcp_msg_info);
				} else {
					CreatePCPMap(pcp_msg_info);
				}
			} else {
				syslog(LOG_ERR, "PCP: Invalid PCP v1 MAP message.");
				return pcp_msg_info->result_code;
			}
			break;

#ifdef PCP_PEER
		case PCP_OPCODE_PEER:

			remainingSize -= PCP_PEER_V1_SIZE;
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
				return pcp_msg_info->result_code;
			}

#ifdef DEBUG
			printPEEROpcodeVersion1(req);
#endif /* DEBUG */
			parsePCPPEER_version1(req, pcp_msg_info);

			req += PCP_PEER_V1_SIZE;

			parsePCPOptions(req, remainingSize, pcp_msg_info);

			if (ValidatePCPMsg(pcp_msg_info)) {
				if (pcp_msg_info->lifetime == 0) {
					DeletePCPPeer(pcp_msg_info);
				} else {
					CreatePCPPeer(pcp_msg_info);
				}
			} else {
				syslog(LOG_ERR, "PCP: Invalid PCP v1 PEER message.");
				 return pcp_msg_info->result_code;
			}


			break;
#endif /* PCP_PEER */
		default:
			pcp_msg_info->result_code = PCP_ERR_UNSUPP_OPCODE;
			break;
		}

	} else if (pcp_msg_info->version == 2) {
		/* RFC 6887 PCP support
		 * http://tools.ietf.org/html/rfc6887 */
		switch (pcp_msg_info->opcode) {
		case PCP_OPCODE_ANNOUNCE:
			/* should check PCP Client's IP Address in request */
			/* see http://tools.ietf.org/html/rfc6887#section-14.1 */
			break;
		case PCP_OPCODE_MAP:

			remainingSize -= PCP_MAP_V2_SIZE;
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
				return pcp_msg_info->result_code;
			}

#ifdef DEBUG
			printMAPOpcodeVersion2(req);
#endif /* DEBUG */
			parsePCPMAP_version2(req, pcp_msg_info);
			req += PCP_MAP_V2_SIZE;

			parsePCPOptions(req, remainingSize, pcp_msg_info);

			if (ValidatePCPMsg(pcp_msg_info)) {
				if (pcp_msg_info->lifetime == 0) {
					DeletePCPMap(pcp_msg_info);
				} else {
					CreatePCPMap(pcp_msg_info);
				}
			} else {
				syslog(LOG_ERR, "PCP: Invalid PCP v2 MAP message.");
				return pcp_msg_info->result_code;
			}


			break;

#ifdef PCP_PEER
		case PCP_OPCODE_PEER:

			remainingSize -= PCP_PEER_V2_SIZE;
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
				return pcp_msg_info->result_code;
			}

#ifdef DEBUG
			printPEEROpcodeVersion2(req);
#endif /* DEBUG */
			parsePCPPEER_version2(req, pcp_msg_info);
			req += PCP_PEER_V2_SIZE;

			if (pcp_msg_info->result_code != 0) {
				return pcp_msg_info->result_code;
			}

			parsePCPOptions(req, remainingSize, pcp_msg_info);

			if (ValidatePCPMsg(pcp_msg_info)) {
				if (pcp_msg_info->lifetime == 0) {
					DeletePCPPeer(pcp_msg_info);
				} else {
					CreatePCPPeer(pcp_msg_info);
				}
			} else {
				syslog(LOG_ERR, "PCP: Invalid PCP v2 PEER message.");
			}

		break;
#endif /* PCP_PEER */

#ifdef PCP_SADSCP
		case PCP_OPCODE_SADSCP:
			remainingSize -= PCP_SADSCP_REQ_SIZE;
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
				return pcp_msg_info->result_code;
			}

			remainingSize -= ((uint8_t *)req)[13];	/* app_name_length */
			if (remainingSize < 0) {
				pcp_msg_info->result_code = PCP_ERR_MALFORMED_OPTION;
				return pcp_msg_info->result_code;
			}

#ifdef DEBUG
			printSADSCPOpcode(req);
#endif
			parseSADSCP(req, pcp_msg_info);
			req += PCP_SADSCP_REQ_SIZE;
			if (pcp_msg_info->result_code != 0) {
				return pcp_msg_info->result_code;
			}
			req += pcp_msg_info->app_name_len;

			get_dscp_value(pcp_msg_info);


			break;
#endif
		default:
			pcp_msg_info->result_code = PCP_ERR_UNSUPP_OPCODE;
			break;
		}
	} else {
		pcp_msg_info->result_code = PCP_ERR_UNSUPP_VERSION;
		return pcp_msg_info->result_code;
	}
	return 1;
}