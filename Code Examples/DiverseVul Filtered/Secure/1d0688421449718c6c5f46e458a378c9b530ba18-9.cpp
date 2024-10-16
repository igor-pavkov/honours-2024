static void virtbt_tx_done(struct virtqueue *vq)
{
	struct sk_buff *skb;
	unsigned int len;

	while ((skb = virtqueue_get_buf(vq, &len)))
		kfree_skb(skb);
}