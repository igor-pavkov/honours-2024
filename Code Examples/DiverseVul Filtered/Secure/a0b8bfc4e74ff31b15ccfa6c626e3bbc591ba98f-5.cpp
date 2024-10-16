static void disconnect(struct pptp_conn_t *conn)
{
	log_ppp_debug("pptp: disconnect\n");

	triton_md_unregister_handler(&conn->hnd, 1);

	if (conn->timeout_timer.tpd)
		triton_timer_del(&conn->timeout_timer);

	if (conn->echo_timer.tpd)
		triton_timer_del(&conn->echo_timer);

	if (conn->state == STATE_PPP) {
		__sync_sub_and_fetch(&stat_active, 1);
		conn->state = STATE_CLOSE;
		ap_session_terminate(&conn->ppp.ses, TERM_LOST_CARRIER, 1);
	} else if (conn->state != STATE_CLOSE)
		__sync_sub_and_fetch(&stat_starting, 1);

	triton_event_fire(EV_CTRL_FINISHED, &conn->ppp.ses);

	log_ppp_info1("disconnected\n");

	triton_context_unregister(&conn->ctx);

	if (conn->ppp.ses.chan_name)
		_free(conn->ppp.ses.chan_name);

	_free(conn->in_buf);
	_free(conn->out_buf);
	_free(conn->ctrl.calling_station_id);
	_free(conn->ctrl.called_station_id);
	mempool_free(conn);
}