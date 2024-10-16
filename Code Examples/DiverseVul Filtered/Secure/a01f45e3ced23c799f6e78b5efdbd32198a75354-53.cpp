static struct task *h2_io_cb(struct task *t, void *ctx, unsigned short status)
{
	struct h2c *h2c = ctx;
	int ret = 0;

	if (!(h2c->wait_event.events & SUB_RETRY_SEND))
		ret = h2_send(h2c);
	if (!(h2c->wait_event.events & SUB_RETRY_RECV))
		ret |= h2_recv(h2c);
	if (ret || b_data(&h2c->dbuf))
		h2_process(h2c);
	return NULL;
}