static inline void h2c_restart_reading(const struct h2c *h2c)
{
	if (!h2_recv_allowed(h2c))
		return;
	if (!b_data(&h2c->dbuf) && (h2c->wait_event.events & SUB_RETRY_RECV))
		return;
	tasklet_wakeup(h2c->wait_event.task);
}