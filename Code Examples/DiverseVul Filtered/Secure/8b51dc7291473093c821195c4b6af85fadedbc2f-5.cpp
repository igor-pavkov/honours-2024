static void rsi_disconnect(struct usb_interface *pfunction)
{
	struct rsi_hw *adapter = usb_get_intfdata(pfunction);

	if (!adapter)
		return;

	rsi_mac80211_detach(adapter);

	if (IS_ENABLED(CONFIG_RSI_COEX) && adapter->priv->coex_mode > 1 &&
	    adapter->priv->bt_adapter) {
		rsi_bt_ops.detach(adapter->priv->bt_adapter);
		adapter->priv->bt_adapter = NULL;
	}

	rsi_reset_card(adapter);
	rsi_deinit_usb_interface(adapter);
	rsi_91x_deinit(adapter);

	rsi_dbg(INFO_ZONE, "%s: Deinitialization completed\n", __func__);
}