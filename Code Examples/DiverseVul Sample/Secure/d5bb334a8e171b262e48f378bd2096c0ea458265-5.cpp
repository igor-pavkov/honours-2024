void hci_chan_del(struct hci_chan *chan)
{
	struct hci_conn *conn = chan->conn;
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("%s hcon %p chan %p", hdev->name, conn, chan);

	list_del_rcu(&chan->list);

	synchronize_rcu();

	/* Prevent new hci_chan's to be created for this hci_conn */
	set_bit(HCI_CONN_DROP, &conn->flags);

	hci_conn_put(conn);

	skb_queue_purge(&chan->data_q);
	kfree(chan);
}