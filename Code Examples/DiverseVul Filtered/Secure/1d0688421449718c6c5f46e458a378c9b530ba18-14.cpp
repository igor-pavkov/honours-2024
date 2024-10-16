static int virtbt_setup_zephyr(struct hci_dev *hdev)
{
	struct sk_buff *skb;

	/* Read Build Information */
	skb = __hci_cmd_sync(hdev, 0xfc08, 0, NULL, HCI_INIT_TIMEOUT);
	if (IS_ERR(skb))
		return PTR_ERR(skb);

	bt_dev_info(hdev, "%s", (char *)(skb->data + 1));

	hci_set_fw_info(hdev, "%s", skb->data + 1);

	kfree_skb(skb);
	return 0;
}