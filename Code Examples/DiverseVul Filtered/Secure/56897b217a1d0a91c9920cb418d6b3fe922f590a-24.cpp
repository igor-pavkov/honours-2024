static inline struct sk_buff *hci_uart_dequeue(struct hci_uart *hu)
{
	struct sk_buff *skb = hu->tx_skb;

	if (!skb) {
		percpu_down_read(&hu->proto_lock);

		if (test_bit(HCI_UART_PROTO_READY, &hu->flags))
			skb = hu->proto->dequeue(hu);

		percpu_up_read(&hu->proto_lock);
	} else {
		hu->tx_skb = NULL;
	}

	return skb;
}