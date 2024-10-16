static int vhost_net_buf_is_empty(struct vhost_net_buf *rxq)
{
	return rxq->tail == rxq->head;
}