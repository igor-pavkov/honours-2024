static void *vhost_net_buf_get_ptr(struct vhost_net_buf *rxq)
{
	if (rxq->tail != rxq->head)
		return rxq->queue[rxq->head];
	else
		return NULL;
}