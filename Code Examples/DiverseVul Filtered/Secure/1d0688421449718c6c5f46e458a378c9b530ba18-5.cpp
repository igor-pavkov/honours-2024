static int virtbt_add_inbuf(struct virtio_bluetooth *vbt)
{
	struct virtqueue *vq = vbt->vqs[VIRTBT_VQ_RX];
	struct scatterlist sg[1];
	struct sk_buff *skb;
	int err;

	skb = alloc_skb(1000, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	sg_init_one(sg, skb->data, 1000);

	err = virtqueue_add_inbuf(vq, sg, 1, skb, GFP_KERNEL);
	if (err < 0) {
		kfree_skb(skb);
		return err;
	}

	return 0;
}