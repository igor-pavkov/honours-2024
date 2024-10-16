static int send_pptp_start_ctrl_conn_rply(struct pptp_conn_t *conn, int res_code, int err_code)
{
	struct pptp_start_ctrl_conn msg = {
		.header = PPTP_HEADER_CTRL(PPTP_START_CTRL_CONN_RPLY),
		.version = htons(PPTP_VERSION),
		.result_code = res_code,
		.error_code = err_code,
		.framing_cap = htonl(PPTP_FRAME_ANY),
		.bearer_cap = htonl(PPTP_BEARER_ANY),
		.max_channels = htons(1),
		.firmware_rev = htons(PPTP_FIRMWARE_VERSION),
	};

	memset(msg.hostname, 0, sizeof(msg.hostname));
	strcpy((char*)msg.hostname, PPTP_HOSTNAME);

	memset(msg.vendor, 0, sizeof(msg.vendor));
	strcpy((char*)msg.vendor, PPTP_VENDOR);

	if (conf_verbose)
		log_ppp_info2("send [PPTP Start-Ctrl-Conn-Reply <Version %i> <Result %i> <Error %i> <Framing %x> <Bearer %x> <Max-Chan %i>]\n", msg.version, msg.result_code, msg.error_code, ntohl(msg.framing_cap), ntohl(msg.bearer_cap), ntohs(msg.max_channels));

	return post_msg(conn, &msg, sizeof(msg));
}