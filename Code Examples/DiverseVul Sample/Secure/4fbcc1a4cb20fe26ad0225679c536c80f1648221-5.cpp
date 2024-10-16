static u8 st21nfca_se_get_bwi(struct nfc_hci_dev *hdev)
{
	int i;
	u8 td;
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);

	/* Bits 8 to 5 of the first TB for T=1 encode BWI from zero to nine */
	for (i = 1; i < ST21NFCA_ESE_MAX_LENGTH; i++) {
		td = ST21NFCA_ATR_GET_Y_FROM_TD(info->se_info.atr[i]);
		if (ST21NFCA_ATR_TA_PRESENT(td))
			i++;
		if (ST21NFCA_ATR_TB_PRESENT(td)) {
			i++;
			return info->se_info.atr[i] >> 4;
		}
	}
	return ST21NFCA_ATR_DEFAULT_BWI;
}