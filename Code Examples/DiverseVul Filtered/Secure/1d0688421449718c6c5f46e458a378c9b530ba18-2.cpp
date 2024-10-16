static int virtbt_close(struct hci_dev *hdev)
{
	struct virtio_bluetooth *vbt = hci_get_drvdata(hdev);
	int i;

	cancel_work_sync(&vbt->rx);

	for (i = 0; i < ARRAY_SIZE(vbt->vqs); i++) {
		struct virtqueue *vq = vbt->vqs[i];
		struct sk_buff *skb;

		while ((skb = virtqueue_detach_unused_buf(vq)))
			kfree_skb(skb);
	}

	return 0;
}