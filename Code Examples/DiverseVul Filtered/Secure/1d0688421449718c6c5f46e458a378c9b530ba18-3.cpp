static int virtbt_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct virtio_bluetooth *vbt = hci_get_drvdata(hdev);
	struct scatterlist sg[1];
	int err;

	memcpy(skb_push(skb, 1), &hci_skb_pkt_type(skb), 1);

	sg_init_one(sg, skb->data, skb->len);
	err = virtqueue_add_outbuf(vbt->vqs[VIRTBT_VQ_TX], sg, 1, skb,
				   GFP_KERNEL);
	if (err) {
		kfree_skb(skb);
		return err;
	}

	virtqueue_kick(vbt->vqs[VIRTBT_VQ_TX]);
	return 0;
}