static void pptp_timeout(struct triton_timer_t *t)
{
	struct pptp_conn_t *conn = container_of(t, typeof(*conn), timeout_timer);
	disconnect(conn);
}