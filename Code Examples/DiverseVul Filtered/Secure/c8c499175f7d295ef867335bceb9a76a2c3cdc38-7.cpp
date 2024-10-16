static int sco_connect(struct sock *sk)
{
	bdaddr_t *src = &bt_sk(sk)->src;
	bdaddr_t *dst = &bt_sk(sk)->dst;
	struct sco_conn *conn;
	struct hci_conn *hcon;
	struct hci_dev  *hdev;
	int err, type;

	BT_DBG("%pMR -> %pMR", src, dst);

	hdev = hci_get_route(dst, src);
	if (!hdev)
		return -EHOSTUNREACH;

	hci_dev_lock(hdev);

	if (lmp_esco_capable(hdev) && !disable_esco)
		type = ESCO_LINK;
	else
		type = SCO_LINK;

	hcon = hci_connect(hdev, type, dst, BDADDR_BREDR, BT_SECURITY_LOW,
			   HCI_AT_NO_BONDING);
	if (IS_ERR(hcon)) {
		err = PTR_ERR(hcon);
		goto done;
	}

	conn = sco_conn_add(hcon);
	if (!conn) {
		hci_conn_put(hcon);
		err = -ENOMEM;
		goto done;
	}

	/* Update source addr of the socket */
	bacpy(src, conn->src);

	err = sco_chan_add(conn, sk, NULL);
	if (err)
		goto done;

	if (hcon->state == BT_CONNECTED) {
		sco_sock_clear_timer(sk);
		sk->sk_state = BT_CONNECTED;
	} else {
		sk->sk_state = BT_CONNECT;
		sco_sock_set_timer(sk, sk->sk_sndtimeo);
	}

done:
	hci_dev_unlock(hdev);
	hci_dev_put(hdev);
	return err;
}