int st21nfca_apdu_reader_event_received(struct nfc_hci_dev *hdev,
					u8 event, struct sk_buff *skb)
{
	int r = 0;
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);

	pr_debug("apdu reader gate event: %x\n", event);

	switch (event) {
	case ST21NFCA_EVT_TRANSMIT_DATA:
		del_timer_sync(&info->se_info.bwi_timer);
		info->se_info.bwi_active = false;
		r = nfc_hci_send_event(hdev, ST21NFCA_DEVICE_MGNT_GATE,
				ST21NFCA_EVT_SE_END_OF_APDU_TRANSFER, NULL, 0);
		if (r < 0)
			goto exit;

		info->se_info.cb(info->se_info.cb_context,
			skb->data, skb->len, 0);
		break;
	case ST21NFCA_EVT_WTX_REQUEST:
		mod_timer(&info->se_info.bwi_timer, jiffies +
				msecs_to_jiffies(info->se_info.wt_timeout));
		break;
	default:
		nfc_err(&hdev->ndev->dev, "Unexpected event on apdu reader gate\n");
		return 1;
	}

exit:
	kfree_skb(skb);
	return r;
}