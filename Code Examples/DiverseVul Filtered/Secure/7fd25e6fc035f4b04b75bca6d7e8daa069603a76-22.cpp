static int atusb_set_channel(struct ieee802154_hw *hw, u8 page, u8 channel)
{
	struct atusb *atusb = hw->priv;
	int ret;

	ret = atusb_write_subreg(atusb, SR_CHANNEL, channel);
	if (ret < 0)
		return ret;
	return 0;
}