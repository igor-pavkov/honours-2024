static int send_pptp_stop_ctrl_conn_rqst(struct pptp_conn_t *conn, int reason)
{
	struct pptp_stop_ctrl_conn msg = {
		.header = PPTP_HEADER_CTRL(PPTP_STOP_CTRL_CONN_RQST),
		.reason_result = hton8(reason),
	};

	if (conf_verbose)
		log_ppp_info2("send [PPTP Stop-Ctrl-Conn-Request <Reason %i>]\n", reason);

	return post_msg(conn, &msg, sizeof(msg));
}