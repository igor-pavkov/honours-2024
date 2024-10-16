static void ppp_finished(struct ap_session *ses)
{
	struct ppp_t *ppp = container_of(ses, typeof(*ppp), ses);
	struct pptp_conn_t *conn = container_of(ppp, typeof(*conn), ppp);

	if (conn->state != STATE_CLOSE) {
		log_ppp_debug("pptp: ppp finished\n");
		conn->state = STATE_CLOSE;
		__sync_sub_and_fetch(&stat_active, 1);

		if (send_pptp_call_disconnect_notify(conn, 3))
			triton_context_call(&conn->ctx, (void (*)(void*))disconnect, conn);
		else if (send_pptp_stop_ctrl_conn_rqst(conn, 0))
			triton_context_call(&conn->ctx, (void (*)(void*))disconnect, conn);
		else {
			if (conn->timeout_timer.tpd)
				triton_timer_mod(&conn->timeout_timer, 0);
			else
				triton_timer_add(&conn->ctx, &conn->timeout_timer, 0);
		}
	}
}