static int virtbt_setup_intel(struct hci_dev *hdev)
{
	struct sk_buff *skb;

	/* Intel Read Version */
	skb = __hci_cmd_sync(hdev, 0xfc05, 0, NULL, HCI_CMD_TIMEOUT);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	kfree_skb(skb);
	return 0;
}