static int sco_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr, __u8 type)
{
	register struct sock *sk;
	struct hlist_node *node;
	int lm = 0;

	if (type != SCO_LINK && type != ESCO_LINK)
		return -EINVAL;

	BT_DBG("hdev %s, bdaddr %s", hdev->name, batostr(bdaddr));

	/* Find listening sockets */
	read_lock(&sco_sk_list.lock);
	sk_for_each(sk, node, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		if (!bacmp(&bt_sk(sk)->src, &hdev->bdaddr) ||
				!bacmp(&bt_sk(sk)->src, BDADDR_ANY)) {
			lm |= HCI_LM_ACCEPT;
			break;
		}
	}
	read_unlock(&sco_sk_list.lock);

	return lm;
}