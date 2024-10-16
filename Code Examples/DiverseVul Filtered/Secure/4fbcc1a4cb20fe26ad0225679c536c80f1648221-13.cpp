static int st21nfca_hci_control_se(struct nfc_hci_dev *hdev, u32 se_idx,
				u8 state)
{
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);
	int r, i;
	struct sk_buff *sk_host_list;
	u8 se_event, host_id;

	switch (se_idx) {
	case NFC_HCI_UICC_HOST_ID:
		se_event = (state == ST21NFCA_SE_MODE_ON ?
					ST21NFCA_EVT_UICC_ACTIVATE :
					ST21NFCA_EVT_UICC_DEACTIVATE);

		info->se_info.count_pipes = 0;
		info->se_info.expected_pipes = ST21NFCA_SE_COUNT_PIPE_UICC;
		break;
	case ST21NFCA_ESE_HOST_ID:
		se_event = (state == ST21NFCA_SE_MODE_ON ?
					ST21NFCA_EVT_SE_ACTIVATE :
					ST21NFCA_EVT_SE_DEACTIVATE);

		info->se_info.count_pipes = 0;
		info->se_info.expected_pipes = ST21NFCA_SE_COUNT_PIPE_EMBEDDED;
		break;
	default:
		return -EINVAL;
	}

	/*
	 * Wait for an EVT_HOT_PLUG in order to
	 * retrieve a relevant host list.
	 */
	reinit_completion(&info->se_info.req_completion);
	r = nfc_hci_send_event(hdev, ST21NFCA_DEVICE_MGNT_GATE, se_event,
			       NULL, 0);
	if (r < 0)
		return r;

	mod_timer(&info->se_info.se_active_timer, jiffies +
		msecs_to_jiffies(ST21NFCA_SE_TO_HOT_PLUG));
	info->se_info.se_active = true;

	/* Ignore return value and check in any case the host_list */
	wait_for_completion_interruptible(&info->se_info.req_completion);

	r = nfc_hci_get_param(hdev, NFC_HCI_ADMIN_GATE,
			NFC_HCI_ADMIN_HOST_LIST,
			&sk_host_list);
	if (r < 0)
		return r;

	for (i = 0; i < sk_host_list->len &&
		sk_host_list->data[i] != se_idx; i++)
		;
	host_id = sk_host_list->data[i];
	kfree_skb(sk_host_list);

	if (state == ST21NFCA_SE_MODE_ON && host_id == se_idx)
		return se_idx;
	else if (state == ST21NFCA_SE_MODE_OFF && host_id != se_idx)
		return se_idx;

	return -1;
}