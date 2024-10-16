static int virtbt_open(struct hci_dev *hdev)
{
	struct virtio_bluetooth *vbt = hci_get_drvdata(hdev);

	if (virtbt_add_inbuf(vbt) < 0)
		return -EIO;

	virtqueue_kick(vbt->vqs[VIRTBT_VQ_RX]);
	return 0;
}