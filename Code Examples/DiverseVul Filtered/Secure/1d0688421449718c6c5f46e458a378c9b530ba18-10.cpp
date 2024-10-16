static int virtbt_shutdown_generic(struct hci_dev *hdev)
{
	struct sk_buff *skb;

	/* Reset */
	skb = __hci_cmd_sync(hdev, HCI_OP_RESET, 0, NULL, HCI_INIT_TIMEOUT);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	kfree_skb(skb);
	return 0;
}