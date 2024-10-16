static int virtbt_set_bdaddr_zephyr(struct hci_dev *hdev,
				    const bdaddr_t *bdaddr)
{
	struct sk_buff *skb;

	/* Write BD_ADDR */
	skb = __hci_cmd_sync(hdev, 0xfc06, 6, bdaddr, HCI_INIT_TIMEOUT);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	kfree_skb(skb);
	return 0;
}