static void hci_conn_cleanup(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;

	if (test_bit(HCI_CONN_PARAM_REMOVAL_PEND, &conn->flags))
		hci_conn_params_del(conn->hdev, &conn->dst, conn->dst_type);

	hci_chan_list_flush(conn);

	hci_conn_hash_del(hdev, conn);

	if (hdev->notify)
		hdev->notify(hdev, HCI_NOTIFY_CONN_DEL);

	hci_conn_del_sysfs(conn);

	debugfs_remove_recursive(conn->debugfs);

	hci_dev_put(hdev);

	hci_conn_put(conn);
}