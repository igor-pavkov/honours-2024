static int atusb_get_and_conf_chip(struct atusb *atusb)
{
	struct usb_device *usb_dev = atusb->usb_dev;
	u8 man_id_0, man_id_1, part_num, version_num;
	const char *chip;
	struct ieee802154_hw *hw = atusb->hw;

	man_id_0 = atusb_read_reg(atusb, RG_MAN_ID_0);
	man_id_1 = atusb_read_reg(atusb, RG_MAN_ID_1);
	part_num = atusb_read_reg(atusb, RG_PART_NUM);
	version_num = atusb_read_reg(atusb, RG_VERSION_NUM);

	if (atusb->err)
		return atusb->err;

	hw->flags = IEEE802154_HW_TX_OMIT_CKSUM | IEEE802154_HW_AFILT |
		    IEEE802154_HW_PROMISCUOUS | IEEE802154_HW_CSMA_PARAMS;

	hw->phy->flags = WPAN_PHY_FLAG_TXPOWER | WPAN_PHY_FLAG_CCA_ED_LEVEL |
			 WPAN_PHY_FLAG_CCA_MODE;

	hw->phy->supported.cca_modes = BIT(NL802154_CCA_ENERGY) |
				       BIT(NL802154_CCA_CARRIER) |
				       BIT(NL802154_CCA_ENERGY_CARRIER);
	hw->phy->supported.cca_opts = BIT(NL802154_CCA_OPT_ENERGY_CARRIER_AND) |
				      BIT(NL802154_CCA_OPT_ENERGY_CARRIER_OR);

	hw->phy->cca.mode = NL802154_CCA_ENERGY;

	hw->phy->current_page = 0;

	if ((man_id_1 << 8 | man_id_0) != ATUSB_JEDEC_ATMEL) {
		dev_err(&usb_dev->dev,
			"non-Atmel transceiver xxxx%02x%02x\n",
			man_id_1, man_id_0);
		goto fail;
	}

	switch (part_num) {
	case 2:
		chip = "AT86RF230";
		atusb->hw->phy->supported.channels[0] = 0x7FFF800;
		atusb->hw->phy->current_channel = 11;	/* reset default */
		atusb->hw->phy->symbol_duration = 16;
		atusb->hw->phy->supported.tx_powers = atusb_powers;
		atusb->hw->phy->supported.tx_powers_size = ARRAY_SIZE(atusb_powers);
		hw->phy->supported.cca_ed_levels = atusb_ed_levels;
		hw->phy->supported.cca_ed_levels_size = ARRAY_SIZE(atusb_ed_levels);
		break;
	case 3:
		chip = "AT86RF231";
		atusb->hw->phy->supported.channels[0] = 0x7FFF800;
		atusb->hw->phy->current_channel = 11;	/* reset default */
		atusb->hw->phy->symbol_duration = 16;
		atusb->hw->phy->supported.tx_powers = atusb_powers;
		atusb->hw->phy->supported.tx_powers_size = ARRAY_SIZE(atusb_powers);
		hw->phy->supported.cca_ed_levels = atusb_ed_levels;
		hw->phy->supported.cca_ed_levels_size = ARRAY_SIZE(atusb_ed_levels);
		break;
	case 7:
		chip = "AT86RF212";
		atusb->hw->flags |= IEEE802154_HW_LBT;
		atusb->hw->phy->supported.channels[0] = 0x00007FF;
		atusb->hw->phy->supported.channels[2] = 0x00007FF;
		atusb->hw->phy->current_channel = 5;
		atusb->hw->phy->symbol_duration = 25;
		atusb->hw->phy->supported.lbt = NL802154_SUPPORTED_BOOL_BOTH;
		atusb->hw->phy->supported.tx_powers = at86rf212_powers;
		atusb->hw->phy->supported.tx_powers_size = ARRAY_SIZE(at86rf212_powers);
		atusb->hw->phy->supported.cca_ed_levels = at86rf212_ed_levels_100;
		atusb->hw->phy->supported.cca_ed_levels_size = ARRAY_SIZE(at86rf212_ed_levels_100);
		break;
	default:
		dev_err(&usb_dev->dev,
			"unexpected transceiver, part 0x%02x version 0x%02x\n",
			part_num, version_num);
		goto fail;
	}

	hw->phy->transmit_power = hw->phy->supported.tx_powers[0];
	hw->phy->cca_ed_level = hw->phy->supported.cca_ed_levels[7];

	dev_info(&usb_dev->dev, "ATUSB: %s version %d\n", chip, version_num);

	return 0;

fail:
	atusb->err = -ENODEV;
	return -ENODEV;
}