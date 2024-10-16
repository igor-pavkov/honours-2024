static int pptp_call_clear_rqst(struct pptp_conn_t *conn)
{
	struct pptp_call_clear_rqst *rqst = (struct pptp_call_clear_rqst *)conn->in_buf;

	if (conf_verbose)
		log_ppp_info2("recv [PPTP Call-Clear-Request <Call-ID %x>]\n", ntohs(rqst->call_id));

	if (conn->echo_timer.tpd)
		triton_timer_del(&conn->echo_timer);

	if (conn->state == STATE_PPP) {
		__sync_sub_and_fetch(&stat_active, 1);
		conn->state = STATE_CLOSE;
		ap_session_terminate(&conn->ppp.ses, TERM_USER_REQUEST, 1);
	}

	return send_pptp_call_disconnect_notify(conn, 4);
}