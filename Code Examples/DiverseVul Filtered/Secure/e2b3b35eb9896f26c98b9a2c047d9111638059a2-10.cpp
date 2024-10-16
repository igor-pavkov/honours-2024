static void vhost_net_buf_init(struct vhost_net_buf *rxq)
{
	rxq->head = rxq->tail = 0;
}