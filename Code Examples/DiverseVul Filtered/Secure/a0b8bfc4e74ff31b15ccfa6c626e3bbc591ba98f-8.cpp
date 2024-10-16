static void pptp_close(struct triton_context_t *ctx)
{
	struct pptp_conn_t *conn = container_of(ctx, typeof(*conn), ctx);
	if (conn->state == STATE_PPP) {
		__sync_sub_and_fetch(&stat_active, 1);
		conn->state = STATE_CLOSE;
		ap_session_terminate(&conn->ppp.ses, TERM_ADMIN_RESET, 1);
		if (send_pptp_call_disconnect_notify(conn, 3)) {
			triton_context_call(&conn->ctx, (void (*)(void*))disconnect, conn);
			return;
		}
	} else {
		if (send_pptp_stop_ctrl_conn_rqst(conn, 0)) {
			triton_context_call(&conn->ctx, (void (*)(void*))disconnect, conn);
			return;
		}
	}

	if (conn->timeout_timer.tpd)
		triton_timer_mod(&conn->timeout_timer, 0);
	else
		triton_timer_add(ctx, &conn->timeout_timer, 0);
}