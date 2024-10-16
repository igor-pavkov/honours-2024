static void hci_connect_le_scan_remove(struct hci_conn *conn)
{
	BT_DBG("%s hcon %p", conn->hdev->name, conn);

	/* We can't call hci_conn_del/hci_conn_cleanup here since that
	 * could deadlock with another hci_conn_del() call that's holding
	 * hci_dev_lock and doing cancel_delayed_work_sync(&conn->disc_work).
	 * Instead, grab temporary extra references to the hci_dev and
	 * hci_conn and perform the necessary cleanup in a separate work
	 * callback.
	 */

	hci_dev_hold(conn->hdev);
	hci_conn_get(conn);

	/* Even though we hold a reference to the hdev, many other
	 * things might get cleaned up meanwhile, including the hdev's
	 * own workqueue, so we can't use that for scheduling.
	 */
	schedule_work(&conn->le_scan_cleanup);
}