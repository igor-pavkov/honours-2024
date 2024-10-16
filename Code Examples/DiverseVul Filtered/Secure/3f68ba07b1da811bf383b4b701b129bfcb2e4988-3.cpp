void hci_send_to_sock(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct sock *sk;
	struct hlist_node *node;
	struct sk_buff *skb_copy = NULL;

	BT_DBG("hdev %p len %d", hdev, skb->len);

	read_lock(&hci_sk_list.lock);

	sk_for_each(sk, node, &hci_sk_list.head) {
		struct hci_filter *flt;
		struct sk_buff *nskb;

		if (sk->sk_state != BT_BOUND || hci_pi(sk)->hdev != hdev)
			continue;

		/* Don't send frame to the socket it came from */
		if (skb->sk == sk)
			continue;

		if (hci_pi(sk)->channel != HCI_CHANNEL_RAW)
			continue;

		/* Apply filter */
		flt = &hci_pi(sk)->filter;

		if (!test_bit((bt_cb(skb)->pkt_type == HCI_VENDOR_PKT) ?
			      0 : (bt_cb(skb)->pkt_type & HCI_FLT_TYPE_BITS),
			      &flt->type_mask))
			continue;

		if (bt_cb(skb)->pkt_type == HCI_EVENT_PKT) {
			int evt = (*(__u8 *)skb->data & HCI_FLT_EVENT_BITS);

			if (!hci_test_bit(evt, &flt->event_mask))
				continue;

			if (flt->opcode &&
			    ((evt == HCI_EV_CMD_COMPLETE &&
			      flt->opcode !=
			      get_unaligned((__le16 *)(skb->data + 3))) ||
			     (evt == HCI_EV_CMD_STATUS &&
			      flt->opcode !=
			      get_unaligned((__le16 *)(skb->data + 4)))))
				continue;
		}

		if (!skb_copy) {
			/* Create a private copy with headroom */
			skb_copy = __pskb_copy(skb, 1, GFP_ATOMIC);
			if (!skb_copy)
				continue;

			/* Put type byte before the data */
			memcpy(skb_push(skb_copy, 1), &bt_cb(skb)->pkt_type, 1);
		}

		nskb = skb_clone(skb_copy, GFP_ATOMIC);
		if (!nskb)
			continue;

		if (sock_queue_rcv_skb(sk, nskb))
			kfree_skb(nskb);
	}

	read_unlock(&hci_sk_list.lock);

	kfree_skb(skb_copy);
}