static int lbs_scan_add_ssid_tlv(struct lbs_private *priv, u8 *tlv)
{
	struct mrvlietypes_ssidparamset *ssid_tlv = (void *)tlv;

	ssid_tlv->header.type = cpu_to_le16(TLV_TYPE_SSID);
	ssid_tlv->header.len = cpu_to_le16(priv->scan_ssid_len);
	memcpy(ssid_tlv->ssid, priv->scan_ssid, priv->scan_ssid_len);
	return sizeof(ssid_tlv->header) + priv->scan_ssid_len;
}