static int parseSADSCP(const uint8_t *buf, pcp_info_t *pcp_msg_info)
{
	pcp_msg_info->delay_tolerance = (buf[12]>>6)&3;
	pcp_msg_info->loss_tolerance = (buf[12]>>4)&3;
	pcp_msg_info->jitter_tolerance = (buf[12]>>2)&3;

	if (pcp_msg_info->delay_tolerance == 3 ||
	    pcp_msg_info->loss_tolerance == 3 ||
	    pcp_msg_info->jitter_tolerance == 3 ) {
		pcp_msg_info->result_code = PCP_ERR_MALFORMED_REQUEST;
		return 1;
	}

	pcp_msg_info->app_name = (const char *)(buf + 14);
	pcp_msg_info->app_name_len = buf[13];

	return 0;
}