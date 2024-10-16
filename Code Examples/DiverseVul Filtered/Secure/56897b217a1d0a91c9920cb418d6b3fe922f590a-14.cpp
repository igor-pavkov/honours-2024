static int hci_uart_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_uart *hu = hci_get_drvdata(hdev);

	BT_DBG("%s: type %d len %d", hdev->name, hci_skb_pkt_type(skb),
	       skb->len);

	percpu_down_read(&hu->proto_lock);

	if (!test_bit(HCI_UART_PROTO_READY, &hu->flags)) {
		percpu_up_read(&hu->proto_lock);
		return -EUNATCH;
	}

	hu->proto->enqueue(hu, skb);
	percpu_up_read(&hu->proto_lock);

	hci_uart_tx_wakeup(hu);

	return 0;
}