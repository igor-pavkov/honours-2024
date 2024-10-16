static int vhost_net_buf_get_size(struct vhost_net_buf *rxq)
{
	return rxq->tail - rxq->head;
}