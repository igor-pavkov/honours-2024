static int atusb_set_hw_addr_filt(struct ieee802154_hw *hw,
				  struct ieee802154_hw_addr_filt *filt,
				  unsigned long changed)
{
	struct atusb *atusb = hw->priv;
	struct device *dev = &atusb->usb_dev->dev;

	if (changed & IEEE802154_AFILT_SADDR_CHANGED) {
		u16 addr = le16_to_cpu(filt->short_addr);

		dev_vdbg(dev, "%s called for saddr\n", __func__);
		atusb_write_reg(atusb, RG_SHORT_ADDR_0, addr);
		atusb_write_reg(atusb, RG_SHORT_ADDR_1, addr >> 8);
	}

	if (changed & IEEE802154_AFILT_PANID_CHANGED) {
		u16 pan = le16_to_cpu(filt->pan_id);

		dev_vdbg(dev, "%s called for pan id\n", __func__);
		atusb_write_reg(atusb, RG_PAN_ID_0, pan);
		atusb_write_reg(atusb, RG_PAN_ID_1, pan >> 8);
	}

	if (changed & IEEE802154_AFILT_IEEEADDR_CHANGED) {
		u8 i, addr[IEEE802154_EXTENDED_ADDR_LEN];

		memcpy(addr, &filt->ieee_addr, IEEE802154_EXTENDED_ADDR_LEN);
		dev_vdbg(dev, "%s called for IEEE addr\n", __func__);
		for (i = 0; i < 8; i++)
			atusb_write_reg(atusb, RG_IEEE_ADDR_0 + i, addr[i]);
	}

	if (changed & IEEE802154_AFILT_PANC_CHANGED) {
		dev_vdbg(dev, "%s called for panc change\n", __func__);
		if (filt->pan_coord)
			atusb_write_subreg(atusb, SR_AACK_I_AM_COORD, 1);
		else
			atusb_write_subreg(atusb, SR_AACK_I_AM_COORD, 0);
	}

	return atusb_get_and_clear_error(atusb);
}