static int send_pptp_stop_ctrl_conn_rply(struct pptp_conn_t *conn, int reason, int err_code)
{
	struct pptp_stop_ctrl_conn msg = {
		.header = PPTP_HEADER_CTRL(PPTP_STOP_CTRL_CONN_RPLY),
		.reason_result = hton8(reason),
		.error_code = hton8(err_code),
	};

	if (conf_verbose)
		log_ppp_info2("send [PPTP Stop-Ctrl-Conn-Reply <Result %i> <Error %i>]\n", msg.reason_result, msg.error_code);

	return post_msg(conn, &msg, sizeof(msg));
}