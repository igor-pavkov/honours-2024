static int vhost_net_buf_peek(struct vhost_net_virtqueue *nvq)
{
	struct vhost_net_buf *rxq = &nvq->rxq;

	if (!vhost_net_buf_is_empty(rxq))
		goto out;

	if (!vhost_net_buf_produce(nvq))
		return 0;

out:
	return vhost_net_buf_peek_len(vhost_net_buf_get_ptr(rxq));
}