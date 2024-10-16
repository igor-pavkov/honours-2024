static struct device *rfcomm_get_device(struct rfcomm_dev *dev)
{
	struct hci_dev *hdev;
	struct hci_conn *conn;

	hdev = hci_get_route(&dev->dst, &dev->src);
	if (!hdev)
		return NULL;

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &dev->dst);

	hci_dev_put(hdev);

	return conn ? &conn->dev : NULL;
}