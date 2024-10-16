int st21nfca_hci_disable_se(struct nfc_hci_dev *hdev, u32 se_idx)
{
	int r;

	/*
	 * According to upper layer, se_idx == NFC_SE_UICC when
	 * info->se_status->is_uicc_enable is true should never happen
	 * Same for eSE.
	 */
	r = st21nfca_hci_control_se(hdev, se_idx, ST21NFCA_SE_MODE_OFF);
	if (r < 0)
		return r;

	return 0;
}