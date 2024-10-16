static void le_scan_cleanup(struct work_struct *work)
{
	struct hci_conn *conn = container_of(work, struct hci_conn,
					     le_scan_cleanup);
	struct hci_dev *hdev = conn->hdev;
	struct hci_conn *c = NULL;

	BT_DBG("%s hcon %p", hdev->name, conn);

	hci_dev_lock(hdev);

	/* Check that the hci_conn is still around */
	rcu_read_lock();
	list_for_each_entry_rcu(c, &hdev->conn_hash.list, list) {
		if (c == conn)
			break;
	}
	rcu_read_unlock();

	if (c == conn) {
		hci_connect_le_scan_cleanup(conn);
		hci_conn_cleanup(conn);
	}

	hci_dev_unlock(hdev);
	hci_dev_put(hdev);
	hci_conn_put(conn);
}