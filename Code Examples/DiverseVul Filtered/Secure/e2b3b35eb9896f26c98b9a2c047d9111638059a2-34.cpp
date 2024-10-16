static void *vhost_net_buf_consume(struct vhost_net_buf *rxq)
{
	void *ret = vhost_net_buf_get_ptr(rxq);
	++rxq->head;
	return ret;
}