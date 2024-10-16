static int ValidatePCPMsg(pcp_info_t *pcp_msg_info)
{
	if (pcp_msg_info->result_code) {
		return 0;
	}

	/* RFC 6887, section 8.2: MUST return address mismatch if NAT
	 * in middle. */
	if (memcmp(pcp_msg_info->int_ip,
		   &pcp_msg_info->sender_ip,
		   sizeof(pcp_msg_info->sender_ip)) != 0) {
		pcp_msg_info->result_code = PCP_ERR_ADDRESS_MISMATCH;
		return 0;
	}

	if (pcp_msg_info->thirdp_ip) {
		if (!GETFLAG(PCP_ALLOWTHIRDPARTYMASK)) {
			pcp_msg_info->result_code = PCP_ERR_UNSUPP_OPTION;
			return 0;
		}

		/* RFC687, section 13.1 - if sender ip == THIRD_PARTY,
		 * it's an error. */
		if (memcmp(pcp_msg_info->thirdp_ip,
			   &pcp_msg_info->sender_ip,
			   sizeof(pcp_msg_info->sender_ip)) == 0) {
			pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
			return 0;
		}
	}

	/* Produce mapped_str for future use. */
	if (!inet_n46top(pcp_msg_info->mapped_ip, pcp_msg_info->mapped_str,
		         sizeof(pcp_msg_info->mapped_str))) {
		syslog(LOG_ERR, "inet_ntop(pcpserver): %m");
		return 0;
	}

	/* protocol zero means 'all protocols' : internal port MUST be zero */
	if (pcp_msg_info->protocol == 0 && pcp_msg_info->int_port != 0) {
		syslog(LOG_ERR, "PCP %s: Protocol was ZERO, but internal port "
		       "has non-ZERO value.", getPCPOpCodeStr(pcp_msg_info->opcode));
		pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
		return 0;
	}

	if (pcp_msg_info->pfailure_present) {
		if ( (IN6_IS_ADDR_UNSPECIFIED(pcp_msg_info->ext_ip) ||
		      ((IN6_IS_ADDR_V4MAPPED(pcp_msg_info->ext_ip)) &&
		       (((uint32_t*)pcp_msg_info->ext_ip->s6_addr)[3] == 0))) &&
		     (pcp_msg_info->ext_port == 0)
		   )
		{
			pcp_msg_info->result_code = PCP_ERR_MALFORMED_OPTION;
			return 0;
		}
	}

	if (CheckExternalAddress(pcp_msg_info)) {
		return 0;
	}

	/* Fill in the desc that describes uniquely what flow we're
	 * dealing with (same code used in both create + delete of
	 * MAP/PEER) */
	switch (pcp_msg_info->opcode) {
	case PCP_OPCODE_MAP:
	case PCP_OPCODE_PEER:
		snprintf(pcp_msg_info->desc, sizeof(pcp_msg_info->desc),
			 "PCP %s %08x%08x%08x",
			 getPCPOpCodeStr(pcp_msg_info->opcode),
			 pcp_msg_info->nonce[0],
			 pcp_msg_info->nonce[1], pcp_msg_info->nonce[2]);
		break;
	}
	return 1;
}