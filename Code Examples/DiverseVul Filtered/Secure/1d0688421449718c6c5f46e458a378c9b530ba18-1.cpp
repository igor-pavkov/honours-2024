static int virtbt_setup_realtek(struct hci_dev *hdev)
{
	struct sk_buff *skb;

	/* Read ROM Version */
	skb = __hci_cmd_sync(hdev, 0xfc6d, 0, NULL, HCI_INIT_TIMEOUT);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	bt_dev_info(hdev, "ROM version %u", *((__u8 *) (skb->data + 1)));

	kfree_skb(skb);
	return 0;
}