static int rsi_usb_host_intf_write_pkt(struct rsi_hw *adapter,
				       u8 *pkt,
				       u32 len)
{
	u32 queueno = ((pkt[1] >> 4) & 0x7);
	u8 endpoint;

	endpoint = ((queueno == RSI_WIFI_MGMT_Q || queueno == RSI_WIFI_DATA_Q ||
		     queueno == RSI_COEX_Q) ? WLAN_EP : BT_EP);

	return rsi_write_multiple(adapter,
				  endpoint,
				  (u8 *)pkt,
				  len);
}