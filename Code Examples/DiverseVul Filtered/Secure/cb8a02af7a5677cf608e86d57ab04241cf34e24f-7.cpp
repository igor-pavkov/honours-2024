static void parsePCPOptions(void* pcp_buf, int remain, pcp_info_t *pcp_msg_info)
{
	int option_length;

	while (remain > 0) {
		option_length = parsePCPOption(pcp_buf, remain, pcp_msg_info);
		if (!option_length)
			break;
		remain -= option_length;
		pcp_buf += option_length;
	}
	if (remain > 0) {
		syslog(LOG_WARNING, "%s: remain=%d", "parsePCPOptions", remain);
	}
}