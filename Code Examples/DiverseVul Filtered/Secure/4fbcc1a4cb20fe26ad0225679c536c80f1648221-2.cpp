static void st21nfca_se_get_atr(struct nfc_hci_dev *hdev)
{
	int r;
	struct sk_buff *skb;
	struct st21nfca_hci_info *info = nfc_hci_get_clientdata(hdev);

	r = nfc_hci_get_param(hdev, ST21NFCA_APDU_READER_GATE,
			ST21NFCA_PARAM_ATR, &skb);
	if (r < 0)
		return;

	if (skb->len <= ST21NFCA_ESE_MAX_LENGTH) {
		memcpy(info->se_info.atr, skb->data, skb->len);
		info->se_info.wt_timeout =
			ST21NFCA_BWI_TO_TIMEOUT(st21nfca_se_get_bwi(hdev));
	}
	kfree_skb(skb);
}