static void virtbt_rx_work(struct work_struct *work)
{
	struct virtio_bluetooth *vbt = container_of(work,
						    struct virtio_bluetooth, rx);
	struct sk_buff *skb;
	unsigned int len;

	skb = virtqueue_get_buf(vbt->vqs[VIRTBT_VQ_RX], &len);
	if (!skb)
		return;

	skb->len = len;
	virtbt_rx_handle(vbt, skb);

	if (virtbt_add_inbuf(vbt) < 0)
		return;

	virtqueue_kick(vbt->vqs[VIRTBT_VQ_RX]);
}