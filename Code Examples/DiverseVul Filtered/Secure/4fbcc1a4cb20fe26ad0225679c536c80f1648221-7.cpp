int st21nfca_hci_se_io(struct nfc_hci_dev *hdev, u32 se_idx,
			u8 *apdu, size_t apdu_length,
			se_io_cb_t cb, void *cb_context)
{
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);

	pr_debug("se_io %x\n", se_idx);

	switch (se_idx) {
	case ST21NFCA_ESE_HOST_ID:
		info->se_info.cb = cb;
		info->se_info.cb_context = cb_context;
		mod_timer(&info->se_info.bwi_timer, jiffies +
			  msecs_to_jiffies(info->se_info.wt_timeout));
		info->se_info.bwi_active = true;
		return nfc_hci_send_event(hdev, ST21NFCA_APDU_READER_GATE,
					ST21NFCA_EVT_TRANSMIT_DATA,
					apdu, apdu_length);
	default:
		return -ENODEV;
	}
}