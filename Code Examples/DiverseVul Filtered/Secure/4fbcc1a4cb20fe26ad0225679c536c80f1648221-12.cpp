int st21nfca_hci_discover_se(struct nfc_hci_dev *hdev)
{
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);
	int se_count = 0;

	if (test_bit(ST21NFCA_FACTORY_MODE, &hdev->quirks))
		return 0;

	if (info->se_status->is_uicc_present) {
		nfc_add_se(hdev->ndev, NFC_HCI_UICC_HOST_ID, NFC_SE_UICC);
		se_count++;
	}

	if (info->se_status->is_ese_present) {
		nfc_add_se(hdev->ndev, ST21NFCA_ESE_HOST_ID, NFC_SE_EMBEDDED);
		se_count++;
	}

	return !se_count;
}