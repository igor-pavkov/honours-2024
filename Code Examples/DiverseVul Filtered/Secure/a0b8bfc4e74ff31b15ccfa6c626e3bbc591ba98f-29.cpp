static int send_pptp_call_disconnect_notify(struct pptp_conn_t *conn, int result)
{
	struct pptp_call_clear_ntfy msg = {
		.header = PPTP_HEADER_CTRL(PPTP_CALL_CLEAR_NTFY),
		.call_id = htons(conn->peer_call_id),
		.result_code = result,
		.error_code = 0,
		.cause_code = 0,
	};

	if (conf_verbose)
		log_ppp_info2("send [PPTP Call-Disconnect-Notify <Call-ID %x> <Result %i> <Error %i> <Cause %i>]\n", ntohs(msg.call_id), msg.result_code, msg.error_code, msg.cause_code);

	return post_msg(conn, &msg, sizeof(msg));
}