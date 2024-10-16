static void virtbt_remove(struct virtio_device *vdev)
{
	struct virtio_bluetooth *vbt = vdev->priv;
	struct hci_dev *hdev = vbt->hdev;

	hci_unregister_dev(hdev);
	vdev->config->reset(vdev);

	hci_free_dev(hdev);
	vbt->hdev = NULL;

	vdev->config->del_vqs(vdev);
	kfree(vbt);
}